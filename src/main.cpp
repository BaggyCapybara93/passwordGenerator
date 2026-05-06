#include "random.hpp"
#include "ui.hpp"
#include <iostream>


int main(int argc, char* arg[]){
    RNG::seed();

    Settings settings;

    if (!UI::parseArgs(argc, arg, settings)) {
        return 1; 
    }

    for (int i = 1; i <= settings.numPasswords; i++){
        std::string password;
        try {
            password = RNG::generate(
                settings.desiredLength, 
                settings.reqUppercase, 
                settings.reqLowercase, 
                settings.reqDigits, 
                settings.reqSpecial
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