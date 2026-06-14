#include "random.hpp"
#include "ui.hpp"
#include "parse_arguments.hpp"
#include "settings.hpp"
#include <iostream>


int main(int argc, char* arg[]){
    // Set program name for help output
    std::string program_name = arg[0];
    
    Settings settings;

    if (!parse_arguments::parse_args(argc, arg, settings)) {
        return 0; // Exit if argument parsing failed or help was requested 
    }

    RNG::seed(settings.seed); 

    // Parse blacklist if provided
    std::set<std::string> blacklist = RNG::parse_blacklist(settings.blacklist);

    for (int i = 1; i <= settings.num_passwords; i++){
        std::string password;
        try {
            // Use honey password generation if the flag is set
            if (settings.is_honeypassword) {
                password = RNG::generate_honey_password(settings.desired_length);
            } else {
                password = RNG::generate(
                    settings.desired_length, 
                    settings.req_uppercase, 
                    settings.req_lowercase, 
                    settings.req_digits, 
                    settings.req_special,
                    settings.custom_chars,
                    settings.exclude_chars,
                    blacklist,
                    settings.exclude_ambiguous
                );
            }
        } catch (const std::invalid_argument& e) {
            UI::print_colored("Error generating password: " + std::string(e.what()), UI::Color::Red);
            return 1;
        } catch (const std::exception& e) {
            UI::print_colored("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red);
            return 1;
        }

        // Check minimum entropy requirement first
        bool entropy_check_passed = true;
        double entropy = 0.0;
        std::string security_rating;
        
        if (settings.min_entropy > 0) {
            entropy = RNG::calculate_entropy(password);
            
            // Determine security rating based on entropy
            if (entropy < 40.0) {
                security_rating = "Weak";
            } else if (entropy < 60.0) {
                security_rating = "Moderate";
            } else {
                security_rating = "Strong";
            }
            
            // Check if password meets minimum entropy requirement
            if (entropy < settings.min_entropy) {
                entropy_check_passed = false;
            }
        } else {
            // No minimum entropy set, calculate and display entropy
            entropy = RNG::calculate_entropy(password);
            
            // Determine security rating based on entropy
            if (entropy < 40.0) {
                security_rating = "Weak";
            } else if (entropy < 60.0) {
                security_rating = "Moderate";
            } else {
                security_rating = "Strong";
            }
        }
        
        // Only display password if it passes entropy check (or if no check is enabled)
        if (entropy_check_passed) {
            // Display warning for honey passwords
            if (settings.is_honeypassword) {
                UI::print_colored("⚠️  HONEY PASSWORD WARNING: This password is intentionally weak!", UI::Color::Red, true, settings.no_color);
                UI::print_colored("==================================================", UI::Color::Blue, true, settings.no_color);
                UI::print_colored("GENERATED PASSWORD:", UI::Color::Cyan, true, settings.no_color);
                UI::print_colored("==================================================", UI::Color::Blue, true, settings.no_color);
                
                UI::print_colored(password, UI::Color::Yellow, true, settings.no_color); // Yellow color for honey password
                
                // Display entropy and security rating
                UI::print_colored("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Red, true, settings.no_color);
                UI::print_colored("Security Rating: " + security_rating, UI::Color::Red, true, settings.no_color);
                UI::print_colored("⚠️  This password is designed to be compromised for security testing purposes.", UI::Color::Red, true, settings.no_color);
            } else {
                UI::print_colored("==================================================", UI::Color::Blue, true, settings.no_color);
                UI::print_colored("GENERATED PASSWORD:", UI::Color::Cyan, true, settings.no_color);
                UI::print_colored("==================================================", UI::Color::Blue, true, settings.no_color);
                
                UI::print_colored(password, UI::Color::Green, true, settings.no_color); // No newline after the password
                
                // Display entropy and security rating
                UI::print_colored("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Yellow, true, settings.no_color);
                UI::print_colored("Security Rating: " + security_rating, UI::Color::Yellow, true, settings.no_color);
            }
        } else {
            // Password failed entropy check, show warning without displaying password
            UI::print_colored("WARNING: Generated password does not meet minimum entropy requirement!", UI::Color::Red, true, settings.no_color);
            UI::print_colored("Entropy: " + std::to_string(static_cast<long long>(entropy)) + " bits (minimum: " + 
                            std::to_string(static_cast<long long>(settings.min_entropy)) + " bits)", UI::Color::Red, true, settings.no_color);
            UI::print_colored("Regenerating password...", UI::Color::Cyan, true, settings.no_color);
            continue; // Regenerate this password
        }

        // Display charset information if custom chars or exclude chars were used
        UI::print_charset_info(settings.custom_chars, settings.exclude_chars, settings.no_color, settings.exclude_ambiguous);

        UI::print_colored("Password generation complete.", UI::Color::Green, true, settings.no_color);

        // Reset terminal colors
        UI::print_reset(settings.no_color);
    }

    return 0;
}