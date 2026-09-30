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
#include "file_manager/file_manager.hpp"

std::string Password_Generator::generate_password() {
    try{
        const std::string pool = build_effective_character_pool(*settings_);
        if (pool.empty()) throw std::invalid_argument("No characters available for password generation.");

        // Rejection sampling preserves a uniform distribution over the valid strings.
        constexpr size_t max_candidate_attempts = 1000000;
        for (size_t attempt = 0; attempt < max_candidate_attempts; ++attempt) {
            std::string password;
            password.reserve(settings_->length);
            for (size_t i = 0; i < settings_->length; ++i) {
                password.push_back(rng_->select_char(pool));
            }
            if (!password_meets_character_requirements(password, *settings_)) continue;
            if (blacklist_ && blacklist_->find(password) != blacklist_->end()) continue;
            return password;
        }
        throw std::runtime_error("Could not sample a valid password after 1,000,000 attempts. The required character groups may be too rare in the selected pool.");
    }catch(const std::exception& e) {
        std::cout << "Error generating password: " << e.what() << std::endl;
        throw;
    }
}

void Password_Generator::display_password(const std::string& password) {
    try{
        const std::string security_rating = calculate_security_score(search_space_entropy_, *settings_);

        if (settings_.get()->is_honeypassword) {
                UI::print_with_color("⚠️  HONEY PASSWORD WARNING: This password is intentionally weak!", UI::Color::Red, settings_.get()->no_color, true);
                UI::print_with_color("==================================================", UI::Color::Blue, settings_.get()->no_color, true);
                UI::print_with_color("GENERATED PASSWORD:", UI::Color::Cyan, settings_.get()->no_color, true);
                UI::print_with_color("==================================================", UI::Color::Blue, settings_.get()->no_color, true);
                
                UI::print_with_color(password, UI::Color::Yellow, settings_.get()->no_color, true); // Yellow color for honey password
                
                UI::print_with_color("Strength: Intentionally weak (entropy estimate unavailable)", UI::Color::Red, settings_.get()->no_color, true);
                UI::print_with_color("⚠️  This password is designed to be compromised for security testing purposes.", UI::Color::Red, settings_.get()->no_color, true);
        } else {
                UI::print_with_color("==================================================", UI::Color::Blue, settings_.get()->no_color, true);
                UI::print_with_color("GENERATED PASSWORD:", UI::Color::Cyan, settings_.get()->no_color, true);
                UI::print_with_color("==================================================", UI::Color::Blue, settings_.get()->no_color, true);
                
                UI::print_with_color(password, UI::Color::Green, settings_.get()->no_color, true); // No newline after the password
                
                // Display the size of the valid output space, independent of the sampled value.
                UI::print_with_color("Search-space entropy: " + std::to_string(search_space_entropy_) + " bits", UI::Color::Yellow, settings_.get()->no_color, true);
                UI::print_with_color("Security Rating: " + security_rating, UI::Color::Yellow, settings_.get()->no_color, true);
        }
    }catch(const std::exception& e) {
        UI::print_with_color("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red, settings_.get()->no_color, true);
        throw;
    }
}

void Password_Generator::generate_passwords(int num_passwords) {
    try{
        generated_passwords_.clear();
        
        for (int i = 1; i <= num_passwords; i++){
            std::string password;
            try {
                password = settings_->is_honeypassword
                    ? generate_honey_password(rng_, settings_)
                    : generate_password();
            } catch (const std::invalid_argument& e) {
                UI::print_with_color("Error generating password: " + std::string(e.what()), UI::Color::Red, settings_.get()->no_color, true);
                return;
            } catch (const std::exception& e) {
                UI::print_with_color("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red, settings_.get()->no_color, true);
                return;
            }

            display_password(password);
            generated_passwords_.push_back(password);
        }

        UI::print_with_color("Password generation complete.", UI::Color::Green, settings_.get()->no_color, true);

        // Reset terminal colors
        UI::reset_color(settings_.get()->no_color);
    }catch(const std::exception& e) {
        UI::print_with_color("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red, settings_.get()->no_color, true);
        throw;
    }
}

void Password_Generator::initialize() {
    try{
        blacklist_ = std::make_shared<std::unordered_set<std::string>>();
        
        // Load blacklist from string or file
        if (!settings_.get()->blacklist.empty()) {
            *blacklist_.get() = parse_blacklist(settings_.get()->blacklist);
        }
        
        // Load blacklist from file if specified
        if (!settings_.get()->blacklist_file.empty()) {
            std::string blacklist_content = file_manager_->load_blacklist(settings_.get()->blacklist_file);
            if (!blacklist_content.empty()) {
                // Parse the file content as if it were in the same format as the string blacklist
                // For file-based blacklist, we assume one password per line
                std::stringstream ss(blacklist_content);
                std::string entry;
                while (std::getline(ss, entry)) {
                    if (!entry.empty()) {
                        blacklist_.get()->emplace(entry);
                    }
                }
            }
        }

        if (!settings_->is_honeypassword && !has_valid_output_space(*settings_, *blacklist_)) {
            throw std::invalid_argument("The blacklist excludes every password allowed by these settings.");
        }
        if (!settings_->is_honeypassword) {
            search_space_entropy_ = calculate_generation_entropy(*settings_, *blacklist_);
            if (search_space_entropy_ < settings_->min_entropy) {
                throw std::invalid_argument("The available passwords do not meet the minimum entropy requirement after applying the blacklist.");
            }
        }

        // Initialize the RNG with settings and optional seed
        if (settings_.get()->seed.has_value()) {
            rng_.get()->seed(settings_.get()->seed);
        } else {
            // If no seed is provided, use default seeding (typically time-based)
            rng_.get()->seed(std::nullopt);
        }
        
        UI::print_with_color("Password Generator initialized.", UI::Color::Green, settings_.get()->no_color, true);
        
        // Generate the specified number of passwords
        generate_passwords(settings_.get()->num_passwords);
        
        // Save passwords to file if specified
        if (!settings_.get()->save_file.empty()) {
            save_passwords_to_file();
        }
    }catch(const std::exception& e) {
        UI::print_with_color("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red, settings_.get()->no_color, true);
        throw;
    }
}

void Password_Generator::save_passwords_to_file() {
    try{
        bool success = file_manager_->save_passwords(settings_.get()->save_file, generated_passwords_);
        if (success) {
            UI::print_with_color("Passwords saved to " + settings_.get()->save_file + " successfully.", UI::Color::Green, settings_.get()->no_color, true);
        } else {
            UI::print_with_color("Failed to save passwords to " + settings_.get()->save_file + ".", UI::Color::Red, settings_.get()->no_color, true);
        }
    } catch(const std::exception& e) {
        UI::print_with_color("An unexpected error occurred while saving passwords: " + std::string(e.what()), UI::Color::Red, settings_.get()->no_color, true);
        throw;
    }
}
