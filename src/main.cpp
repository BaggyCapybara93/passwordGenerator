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

        UI::print_colored("==================================================", UI::Color::Blue);
        UI::print_colored("GENERATED PASSWORD:", UI::Color::Cyan);
        UI::print_colored("==================================================", UI::Color::Blue);
        
        UI::print_colored(password, UI::Color::Green, false); // No newline after the password
        
        
        UI::print_colored("\nPassword generation complete.", UI::Color::Green);

        // Reset terminal colors
        UI::print_reset();
    }

    return 0;
}