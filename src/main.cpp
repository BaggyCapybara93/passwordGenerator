#include "random.hpp"
#include "ui.hpp"
#include "parse_arguments.hpp"
#include <iostream>


int main(int argc, char* arg[]){
    // Set program name for help output
    std::string program_name = arg[0];
    
    Settings settings;

    if (!parse_arguments::parse_args(argc, arg, settings)) {
        return 0; // Exit if argument parsing failed or help was requested 
    }

    RNG::seed(settings.seed); 

    for (int i = 1; i <= settings.num_passwords; i++){
        std::string password;
        try {
            password = RNG::generate(
                settings.desired_length, 
                settings.req_uppercase, 
                settings.req_lowercase, 
                settings.req_digits, 
                settings.req_special
            );
        } catch (const std::invalid_argument& e) {
            UI::print_colored("Error generating password: " + std::string(e.what()), UI::Color::Red);
            return 1;
        } catch (const std::exception& e) {
            UI::print_colored("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red);
            return 1;
        }

        UI::print_colored("==================================================", UI::Color::Blue, true, settings.no_color);
        UI::print_colored("GENERATED PASSWORD:", UI::Color::Cyan, true, settings.no_color);
        UI::print_colored("==================================================", UI::Color::Blue, true, settings.no_color);
        
        UI::print_colored(password, UI::Color::Green, true, settings.no_color); // No newline after the password
        
        // Calculate and display entropy
        double entropy = RNG::calculate_entropy(password);
        
        // Determine security rating based on entropy
        std::string security_rating;
        if (entropy < 40.0) {
            security_rating = "Weak";
        } else if (entropy < 60.0) {
            security_rating = "Moderate";
        } else {
            security_rating = "Strong";
        }
        
        UI::print_colored("\nEntropy: " + std::to_string(static_cast<long long>(entropy)) + " bits", UI::Color::Yellow, true, settings.no_color);
        UI::print_colored("Security Rating: " + security_rating, UI::Color::Yellow, true, settings.no_color);

        UI::print_colored("\nPassword generation complete.", UI::Color::Green, true, settings.no_color);

        // Reset terminal colors
        UI::print_reset(settings.no_color);
    }

    return 0;
}