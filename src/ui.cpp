#include "ui.hpp"
#include <algorithm>
#include <cctype>

namespace UI{

    size_t desiredLength_ = 12;
    int numPasswords_ = 1;
    bool reqUppercase_ = true;
    bool reqLowercase_ = true;
    bool reqDigits_ = true;
    bool reqSpecial_ = true;

    const char* getColor(UI::Color c) {
        switch (c) {
            case Color::Black:   return "30"; break;
            case Color::Red:     return "31"; break; // ⚠️  Errors/Warnings
            case Color::Green:   return "32"; break; // ✅ Success
            case Color::Yellow:  return "33"; break; // ℹ️  Info
            case Color::Blue:    return "34"; break; // 🔵 Header/Title
            case Color::Magenta: return "35"; break;
            case Color::Cyan:    return "36"; break;
            case Color::White:   return "37"; break; // ⚪️ Default reset
            default:             return "00"; break;
        }
    }

    void printEscapeCode(int code, int color_value = 0) {
        std::cout << "\033[" << code << "m";
    }

    void printColored(std::string text, Color color, bool newLine){
        const char* code_str = UI::getColor(color);
        std::cout << "\033[" << std::stoi(code_str) << "m" << text; 
        if (newLine) std::cout << "\033[0m\n";
    }
    
    void printReset(){
        std::cout << "\033[0m";
    }
    
    void printSeparator(){
        std::cout << "\n--------------------------------------------------\n";
    }
    
    int intInput(const std::string& userPrompt, size_t minValue, size_t maxValue){
        int value;
        while(true){
            printColored(userPrompt + " ", Color::Blue);
            if(!(std::cin >> value)){
                printColored("Invalid input. Please enter a number.", Color::Red);
                std::cin.clear(); //Clear Input
                std::cin.ignore(1000, '\n');
                continue;
            }
    
            if(value >= minValue && value <= maxValue){
                return value;
            }else{
                printColored("Value must be between " + std::to_string(minValue) + 
                              " and " + std::to_string(maxValue), Color::Red);
                std::cin.ignore(1000, '\n'); // Clear input buffer
            }
        } 
    }
    
    bool boolInput(const std::string& userPrompt){
        printColored(userPrompt + " [y/n]: ", Color::Blue);
        char response;
        while (true) {
            if (!(std::cin >> std::noskipws >> response)) {
                break;
            }
            
            if (response == 'y' || response == 'Y') return true;
            if (response == 'n' || response == 'N') return false;
            
            printColored("Please enter 'y' or 'n'", Color::Red);
            std::cin.ignore(1000, '\n'); // Clear input buffer
        }
        return false; // Default if invalid
    }
    
    bool parseArgs(int argc, char* arg[]) {
        // Reset configuration to defaults before parsing
        desiredLength_ = 12;
        reqUppercase_ = true;
        reqLowercase_ = true;
        reqDigits_ = true;
        reqSpecial_ = true;
    
        std::cout << "\n--- Parsing Command Line Arguments ---\n";
    
        // Start loop from i = 1 to skip the program name (arg[0])
        for (int i = 1; i < argc; ++i) {
            std::string currentArg = arg[i];
    
            if (currentArg == "--help" || currentArg == "-h") {
                std::cout << "Usage: ./program [--length N] [--no-uppercase]\n"
                          << "Options:\n"
                          << "  --length N   Set desired password length (e.g., --length 32)\n"
                          << "  --no-uppercase Disable uppercase requirement\n"
                          << "  --no-lowercase Disable lowercase requirement\n"
                          << "  --no-digits    Disable digit requirement\n"
                          << "  --no-special   Disable special char requirement\n";
                return false;
            }
    
            if (currentArg == "--length") {
                // Expect the value to be in the next argument
                if (i + 1 < argc) {
                    try {
                        // Safely convert the string argument to size_t
                        desiredLength_ = std::stoul(arg[i + 1]);
                        i++;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: Invalid length specified. Use a valid number.\n";
                        return false;
                    }
                } else {
                    std::cerr << "Error: --length requires a following value.\n";
                    return false;
                }
            }
            
            else if (currentArg == "--no-uppercase" || currentArg == "-u") {
                reqUppercase_ = false;
            }
            else if (currentArg == "--no-lowercase" || currentArg == "-l") {
                reqLowercase_ = false;
            }
            else if (currentArg == "--no-digits" || currentArg == "-d") {
                reqDigits_ = false;
            }
            else if (currentArg == "--no-special" || currentArg == "-s") {
                reqSpecial_ = false;
            }
            else if (currentArg == "--num-passwords" || currentArg == "-nP") {
                if (i + 1 < argc) {
                    try {
                        // Safely convert the string argument to size_t
                        numPasswords_ = std::stoul(arg[i + 1]);
                        i++;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: Invalid num-passwords specified. Use a valid number.\n";
                        return false;
                    }
                } else {
                    std::cerr << "Error: --num-passwords requires a following value.\n";
                    return false;
                }
            }
            
            else {
                std::cout << "Warning: Unknown argument encountered: " << currentArg << "\n";
                return false;
            }
        }
    
        std::cout << "Arguments successfully parsed. Configuration loaded.\n";
        return true;
    }
}

