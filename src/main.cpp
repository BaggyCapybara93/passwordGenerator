#include "random.hpp"
#include "ui.hpp"
#include <iostream>


int main(int argc, char* arg[]){
    RNG::seed();
    if (!UI::parseArgs(argc, arg)) {
        return 1; 
    }

    for (int i = 1; i <= UI::numPasswords_; i++){
        std::string password;
        try {
            password = RNG::generate(
                UI::desiredLength_, 
                UI::reqUppercase_, 
                UI::reqLowercase_, 
                UI::reqDigits_, 
                UI::reqSpecial_
            );
        } catch (const std::invalid_argument& e) {
            UI::printColored("Error generating password: " + std::string(e.what()), UI::Color::Red);
            return 1;
        } catch (const std::exception& e) {
            UI::printColored("An unexpected error occurred: " + std::string(e.what()), UI::Color::Red);
            return 1;
        }

        UI::printColored("==================================================", UI::Color::Blue);
        UI::printColored("GENERATED PASSWORD:", UI::Color::Cyan);
        UI::printColored("==================================================", UI::Color::Blue);
        
        UI::printColored(password, UI::Color::Green, false); // No newline after the password
        
        
        UI::printColored("\nPassword generation complete.", UI::Color::Green);

        // Reset terminal colors
        UI::printReset();
    }

    return 0;
}