#include <sstream>
#include <cmath>
#include <random>
#include <algorithm>
#include <cctype>

#include "password_generator.hpp"
#include "password_entropy.hpp"
#include "password_blacklist.hpp"
#include "password_honey.hpp"
#include "ui/ui.hpp"

std::string Password_Generator::generate_password() {
    std::vector<char> result;
    
    // Determine which character pools to use
    std::string up_pool, low_pool, dig_pool, sp_pool;
    
    if (!settings_.get()->custom_chars.empty()) {
        // Use custom character pool
        std::string custom_pool = rng_.get()->build_custom_pool(settings_.get()->custom_chars);
        
        // Distribute custom characters to pools based on requirements
        if (settings_.get()->req_uppercase) {
            up_pool = custom_pool;
        }
        if (settings_.get()->req_lowercase) {
            low_pool = custom_pool;
        }
        if (settings_.get()->req_digits) {
            dig_pool = custom_pool;
        }
        if (settings_.get()->req_special) {
            sp_pool = custom_pool;
        }
    } else {
        // Use default pools, optionally excluding characters
        if (!settings_.get()->exclude_chars.empty()) {
            std::string default_pool = rng_.get()->build_default_pool();
            std::string excluded_pool = rng_.get()->exclude_chars_from_pool(default_pool, settings_.get()->exclude_chars);
            
            // Apply ambiguous character exclusion if requested
            if (settings_.get()->exclude_ambiguous) {
                excluded_pool = rng_.get()->exclude_ambiguous_from_pool(excluded_pool, settings_.get()->exclude_ambiguous);
            }
            
            if (settings_.get()->req_uppercase) up_pool = excluded_pool;
            if (settings_.get()->req_lowercase) low_pool = excluded_pool;
            if (settings_.get()->req_digits) dig_pool = excluded_pool;
            if (settings_.get()->req_special) sp_pool = excluded_pool;
        } else {
            // Use default pools as-is
            if (settings_.get()->req_uppercase) up_pool = settings_.get()->uppercase_string;
            if (settings_.get()->req_lowercase) low_pool = settings_.get()->lowercase_string;
            if (settings_.get()->req_digits) dig_pool = settings_.get()->digits_string;
            if (settings_.get()->req_special) sp_pool = settings_.get()->special_string;
            
            // Apply ambiguous character exclusion to default pools if requested
            if (settings_.get()->exclude_ambiguous) {
                if (settings_.get()->req_uppercase) up_pool = rng_.get()->exclude_ambiguous_from_pool(up_pool, settings_.get()->exclude_ambiguous);
                if (settings_.get()->req_lowercase) low_pool = rng_.get()->exclude_ambiguous_from_pool(low_pool, settings_.get()->exclude_ambiguous);
                if (settings_.get()->req_digits) dig_pool = rng_.get()->exclude_ambiguous_from_pool(dig_pool, settings_.get()->exclude_ambiguous);
                if (settings_.get()->req_special) sp_pool = rng_.get()->exclude_ambiguous_from_pool(sp_pool, settings_.get()->exclude_ambiguous);
            }
        }
    }

    auto ensure_char = [&](const std::string& s, bool req) {
        if (req && !s.empty()) {
            result.push_back(rng_.get()->select_char(s));
        }
    };

    ensure_char(up_pool, settings_.get()->req_uppercase);
    ensure_char(low_pool, settings_.get()->req_lowercase);
    ensure_char(dig_pool, settings_.get()->req_digits);
    ensure_char(sp_pool, settings_.get()->req_special);

    size_t current_size = result.size();
    if (settings_.get()->length < current_size) {
        std::string msg = "Requested password length (" + std::to_string(settings_.get()->length) + 
                          ") is too short for requirements (" + 
                          std::to_string(current_size) + " minimum).";
        throw std::invalid_argument(msg);
    }
    size_t remaining = settings_.get()->length - current_size;

    // Build the combined pool for remaining characters
    std::string all_pool = "";
    if (settings_.get()->req_uppercase) all_pool += up_pool;
    if (settings_.get()->req_lowercase) all_pool += low_pool;
    if (settings_.get()->req_digits) all_pool += dig_pool;
    if (settings_.get()->req_special) all_pool += sp_pool;

    if (all_pool.empty()) {
        throw std::invalid_argument("No character set defined to generate remaining characters!");
    }

    while(static_cast<size_t>(result.size()) < settings_.get()->length && remaining > 0){
        result.push_back(rng_.get()->select_char(all_pool));
        --remaining;
    }

    {
        std::lock_guard<std::mutex> lock(rng_.get()->engine_mutex_);
        std::shuffle(result.begin(), result.end(), rng_.get()->engine_);
    }

    std::string password = std::string(result.begin(), result.end());
    
    // Check if password is in blacklist
    if (!settings_.get()->blacklist.empty() && settings_.get()->blacklist.find(password) != std::string::npos) {
        // Password is blacklisted, regenerate
        return generate_password();
    }

    return password;
}

void Password_Generator::display_password(const std::string& password) {
    // Check minimum entropy requirement first
    bool entropy_check_passed = true;
    double entropy = 0.0;
    std::string security_rating;
    
    if (settings_.get()->min_entropy > 0) {
        entropy = calculate_entropy(password, settings_);
        
        // Determine security rating based on entropy
        security_rating = calculate_security_score(entropy, settings_);
        
        // Check if password meets minimum entropy requirement
        if (entropy < settings_.get()->min_entropy) {
            entropy_check_passed = false;
        }
    } else {
        // No minimum entropy set, calculate and display entropy
        entropy = calculate_entropy(password, settings_);
        security_rating = calculate_security_score(entropy, settings_);
    }
    
    // Only display password if it passes entropy check (or if no check is enabled)
    if (entropy_check_passed) {
        // Display warning for honey passwords
        if (settings_.get()->is_honeypassword) {
            UI::print_with_color("⚠️  HONEY PASSWORD WARNING: This password is intentionally weak!", UI::Color::Red, true, settings_.get()->no_color);
            UI::print_with_color("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            UI::print_with_color("GENERATED PASSWORD:", UI::Color::Cyan, true, settings_.get()->no_color);
            UI::print_with_color("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            
            UI::print_with_color(password, UI::Color::Yellow, true, settings_.get()->no_color); // Yellow color for honey password
            
            // Display entropy and security rating
            UI::print_with_color("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Red, true, settings_.get()->no_color);
            UI::print_with_color("Security Rating: " + security_rating, UI::Color::Red, true, settings_.get()->no_color);
            UI::print_with_color("⚠️  This password is designed to be compromised for security testing purposes.", UI::Color::Red, true, settings_.get()->no_color);
        } else {
            UI::print_with_color("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            UI::print_with_color("GENERATED PASSWORD:", UI::Color::Cyan, true, settings_.get()->no_color);
            UI::print_with_color("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            
            UI::print_with_color(password, UI::Color::Green, true, settings_.get()->no_color); // No newline after the password
            
            // Display entropy and security rating
            UI::print_with_color("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Yellow, true, settings_.get()->no_color);
            UI::print_with_color("Security Rating: " + security_rating, UI::Color::Yellow, true, settings_.get()->no_color);
        }
    } else {
        // Password failed entropy check, show warning without displaying password
        UI::print_with_color("WARNING: Generated password does not meet minimum entropy requirement!", UI::Color::Red, true, settings_.get()->no_color);
        UI::print_with_color("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits (minimum: " + 
                        std::to_string(static_cast<long long>(settings_.get()->min_entropy)) + " bits)", UI::Color::Red, true, settings_.get()->no_color);
        UI::print_with_color("Regenerating password...", UI::Color::Cyan, true, settings_.get()->no_color);
    }
}

void Password_Generator::generate_passwords(int num_passwords) {
    // Parse blacklist if provided
    std::unordered_set<std::string> blacklist = parse_blacklist(settings_.get()->blacklist);

    for (int i = 1; i <= num_passwords; i++){
        std::string password;
        try {
            // Use honey password generation if the flag is set
            if (settings_.get()->is_honeypassword) {
                password = generate_honey_password(rng_, settings_);
            } else {
                password = generate_password();
            }
        } catch (const std::invalid_argument& e) {
            UI::print_with_color("Error generating password: " + std::string(e.what()), UI::Color::Red, true, settings_.get()->no_color);
            return;
        } catch (const std::exception& e) {
            UI::print_with_color("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red, true, settings_.get()->no_color);
            return;
        }

        display_password(password);

        // Add the generated password to the blacklist, preventing repeated password being generated
        blacklist.emplace(password);
    }

    UI::print_with_color("Password generation complete.", UI::Color::Green, true, settings_.get()->no_color);

    // Reset terminal colors
    UI::reset_color(settings_.get()->no_color);
}

void Password_Generator::initialize() {
    // Initialize the RNG with settings and optional seed
    if (settings_.get()->seed.has_value()) {
        rng_.get()->seed(settings_.get()->seed);
    } else {
        // If no seed is provided, use default seeding (typically time-based)
        rng_.get()->seed(std::nullopt);
    }
    
    UI::print_with_color("Password Generator initialized.", UI::Color::Green, true, settings_.get()->no_color);
    
    // Generate the specified number of passwords
    generate_passwords(settings_.get()->num_passwords);
}