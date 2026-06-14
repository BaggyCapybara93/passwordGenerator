#include "password_generator.hpp"
#include "ui.hpp"
#include <sstream>
#include <cmath>
#include <random>
#include <algorithm>
#include <cctype>

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

std::unordered_set<std::string> Password_Generator::parse_blacklist(const std::string& blacklist_str) {
    std::unordered_set<std::string> blacklist;
    
    if (blacklist_str.empty() || blacklist_str.front() != '{' || blacklist_str.back() != '}') {
        return blacklist;
    }
            
    // Remove braces and split by comma
    std::string content = blacklist_str.substr(1, blacklist_str.length() - 2);
    std::stringstream ss(content);
    std::string entry;
    
    while (std::getline(ss, entry, ',')) {
        if (!entry.empty()) {
            blacklist.insert(entry);
        }
    }
    
    return blacklist;
}

std::string Password_Generator::generate_honey_password() {
    std::string word = rng_.get()->random_word();
    std::string pwd = word;

    // If the word is already too long, truncate it
    if (pwd.size() > settings_.get()->length) {
        pwd = pwd.substr(0, settings_.get()->length);
        return pwd;
    }

    // Choose a weak pattern - still random but with more predictable suffixes
    std::uniform_int_distribution<int> pattern_dist(0, 4);
    int pattern = pattern_dist(rng_.get()->engine_);

    switch (pattern) {
        case 0: {
            // word + random 3-digit number (weak: 000-999)
            std::uniform_int_distribution<int> num_dist(0, 999);
            pwd += std::to_string(num_dist(rng_.get()->engine_));
            break;
        }
        case 1: {
            // word + random 4-digit number (weak: 0000-9999)
            std::uniform_int_distribution<int> num_dist2(0, 9999);
            pwd += std::to_string(num_dist2(rng_.get()->engine_));
            break;
        }
        case 2: {
            // word + random 2-digit number (weak: 00-99)
            std::uniform_int_distribution<int> num_dist3(0, 99);
            pwd += std::to_string(num_dist3(rng_.get()->engine_));
            break;
        }
        case 3: {
            // word + random repeated digit (weak: 00, 11, 22, ..., 99)
            std::uniform_int_distribution<int> d(0, 9);
            char digit = '0' + static_cast<char>(d(rng_.get()->engine_));
            pwd += std::string(2, digit);
            break;
        }
        case 4: {
            // word + random lowercase letter repeated (weak: aa, bb, cc, ..., zz)
            std::uniform_int_distribution<int> l(0, 25);
            char letter = 'a' + static_cast<char>(l(rng_.get()->engine_));
            pwd += std::string(2, letter);
            break;
        }
    }

    // Trim or pad to requested length
    if (pwd.size() > settings_.get()->length) {
        pwd = pwd.substr(0, settings_.get()->length);
    } else {
        // pad with lowercase letters (weak)
        while (pwd.size() < settings_.get()->length) {
            pwd += rng_.get()->select_char(settings_.get()->lowercase_string);
        }
    }

    return pwd;
}

double Password_Generator::calculate_entropy(const std::string& password) {
    if (password.empty()) {
        return 0.0;
    }
    
    // Pool sizes for each character type
    const size_t uppercase_size = settings_.get()->uppercase_string.size();
    const size_t lowercase_size = settings_.get()->lowercase_string.size();
    const size_t digits_size = settings_.get()->digits_string.size();
    const size_t special_size = settings_.get()->special_string.size();
    
    // Calculate entropy: sum of log2(pool_size) for each character
    double entropy = 0.0;
    for (size_t i = 0; i < password.size(); ++i) {
        char c = password[i];
        double pool_size = 0.0;
        
        if (c >= 'A' && c <= 'Z') {
            pool_size = static_cast<double>(uppercase_size);
        } else if (c >= 'a' && c <= 'z') {
            pool_size = static_cast<double>(lowercase_size);
        } else if (c >= '0' && c <= '9') {
            pool_size = static_cast<double>(digits_size);
        } else if (c >= '!' && c <= '~') {  // Special chars are in ASCII range 33-126
            pool_size = static_cast<double>(special_size);
        }
        
        if (pool_size > 0) {
            entropy += std::log2(pool_size);
        }
    }
    
    return entropy;
}

std::string Password_Generator::calculate_security_score(const double& entropy) {
    std::string security_rating;
    // Determine security rating based on entropy
    if (entropy < 40.0) {
        security_rating = "Weak";
    } else if (entropy < 60.0) {
        security_rating = "Moderate";
    } else {
        security_rating = "Strong";
    }
    return security_rating;
}

void Password_Generator::display_password(const std::string& password) {
    // Check minimum entropy requirement first
    bool entropy_check_passed = true;
    double entropy = 0.0;
    std::string security_rating;
    
    if (settings_.get()->min_entropy > 0) {
        entropy = calculate_entropy(password);
        
        // Determine security rating based on entropy
        security_rating = calculate_security_score(entropy);
        
        // Check if password meets minimum entropy requirement
        if (entropy < settings_.get()->min_entropy) {
            entropy_check_passed = false;
        }
    } else {
        // No minimum entropy set, calculate and display entropy
        entropy = calculate_entropy(password);
        security_rating = calculate_security_score(entropy);
    }
    
    // Only display password if it passes entropy check (or if no check is enabled)
    if (entropy_check_passed) {
        // Display warning for honey passwords
        if (settings_.get()->is_honeypassword) {
            UI::print_colored("⚠️  HONEY PASSWORD WARNING: This password is intentionally weak!", UI::Color::Red, true, settings_.get()->no_color);
            UI::print_colored("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            UI::print_colored("GENERATED PASSWORD:", UI::Color::Cyan, true, settings_.get()->no_color);
            UI::print_colored("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            
            UI::print_colored(password, UI::Color::Yellow, true, settings_.get()->no_color); // Yellow color for honey password
            
            // Display entropy and security rating
            UI::print_colored("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Red, true, settings_.get()->no_color);
            UI::print_colored("Security Rating: " + security_rating, UI::Color::Red, true, settings_.get()->no_color);
            UI::print_colored("⚠️  This password is designed to be compromised for security testing purposes.", UI::Color::Red, true, settings_.get()->no_color);
        } else {
            UI::print_colored("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            UI::print_colored("GENERATED PASSWORD:", UI::Color::Cyan, true, settings_.get()->no_color);
            UI::print_colored("==================================================", UI::Color::Blue, true, settings_.get()->no_color);
            
            UI::print_colored(password, UI::Color::Green, true, settings_.get()->no_color); // No newline after the password
            
            // Display entropy and security rating
            UI::print_colored("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Yellow, true, settings_.get()->no_color);
            UI::print_colored("Security Rating: " + security_rating, UI::Color::Yellow, true, settings_.get()->no_color);
        }
    } else {
        // Password failed entropy check, show warning without displaying password
        UI::print_colored("WARNING: Generated password does not meet minimum entropy requirement!", UI::Color::Red, true, settings_.get()->no_color);
        UI::print_colored("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits (minimum: " + 
                        std::to_string(static_cast<long long>(settings_.get()->min_entropy)) + " bits)", UI::Color::Red, true, settings_.get()->no_color);
        UI::print_colored("Regenerating password...", UI::Color::Cyan, true, settings_.get()->no_color);
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
                password = generate_honey_password();
            } else {
                password = generate_password();
            }
        } catch (const std::invalid_argument& e) {
            UI::print_colored("Error generating password: " + std::string(e.what()), UI::Color::Red);
            return;
        } catch (const std::exception& e) {
            UI::print_colored("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red);
            return;
        }

        display_password(password);
    }

    UI::print_colored("Password generation complete.", UI::Color::Green, true, settings_.get()->no_color);

    // Reset terminal colors
    UI::print_reset(settings_.get()->no_color);
}

void Password_Generator::initialize() {
    // Initialize the RNG with settings and optional seed
    if (settings_.get()->seed.has_value()) {
        rng_.get()->seed(settings_.get()->seed);
    } else {
        // If no seed is provided, use default seeding (typically time-based)
        rng_.get()->seed(std::nullopt);
    }
    
    UI::print_colored("Password Generator initialized.", UI::Color::Green, true, settings_.get()->no_color);
    
    // Generate the specified number of passwords
    generate_passwords(settings_.get()->num_passwords);
}