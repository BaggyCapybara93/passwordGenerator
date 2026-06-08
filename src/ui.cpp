#include "ui.hpp"
#include <algorithm>
#include <cctype>

namespace UI{

    const char* get_color(UI::Color c) {
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

    void print_escape_code(const char*& code) {
        std::cout << "\033[" << code << "m";
    }

    void print_colored(std::string text, Color color, bool new_line){
        const char* code_str = UI::get_color(color);
        print_escape_code(code_str);
        std::cout << text; 
        if (new_line) std::cout << "\033[0m\n";
    }
    
    void print_reset(){
        std::cout << "\033[0m";
    }
    
    void print_separator(){
        std::cout << "\n--------------------------------------------------\n";
    }
    
    bool parse_args(int argc, char* argv[], Settings& settings) {
        // Reset configuration to defaults before parsing
        settings.desired_length = 12;
        settings.req_uppercase = true;
        settings.req_lowercase = true;
        settings.req_digits = true;
        settings.req_special = true;
        settings.num_passwords = 1;
        settings.seed = std::nullopt;

        // Start loop from i = 1 to skip the program name (arg[0])
        for (int i = 1; i < argc; ++i) {
            std::string current_arg = argv[i];
    
            if (current_arg == "--help" || current_arg == "-h") {
                std::cout << "Usage: ./program [--length N] [--no-uppercase]\n"
                          << "Options:\n"
                          << "  --length N   Set desired password length (e.g., --length 32)\n"
                          << "  --no-uppercase Disable uppercase requirement\n"
                          << "  --no-lowercase Disable lowercase requirement\n"
                          << "  --no-digits    Disable digit requirement\n"
                          << "  --no-special   Disable special char requirement\n"
                          << "  --seed   Use deterministic seed\n";
                return false;
            }
    
            if (current_arg == "--length") {
                // Expect the value to be in the next argument
                if (i + 1 < argc) {
                    try {
                        // Safely convert the string argument to size_t
                        settings.desired_length = std::stoul(argv[i + 1]);
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
            
            else if (current_arg == "--no-uppercase" || current_arg == "-u") {
                settings.req_uppercase = false;
            }
            else if (current_arg == "--no-lowercase" || current_arg == "-l") {
                settings.req_lowercase = false;
            }
            else if (current_arg == "--no-digits" || current_arg == "-d") {
                settings.req_digits = false;
            }
            else if (current_arg == "--no-special" || current_arg == "-s") {
                settings.req_special = false;
            }
            else if (current_arg == "--num-passwords" || current_arg == "-nP") {
                if (i + 1 < argc) {
                    try {
                        settings.num_passwords = std::stoi(argv[i + 1]);
                        i++;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: Invalid num-passwords specified. Use a valid number.\n";
                        return false;
                    }
                } else {
                    std::cerr << "Error: --num-passwords requires a following value.\n";
                    return false;
                }
            } else if (current_arg == "--seed") {
                if (i + 1 < argc) {
                    try {
                        // Parse the seed as a 64-bit unsigned integer
                        uint64_t seed_val = std::stoull(argv[i + 1]);
                        settings.seed = seed_val;
                        i++;
                    } catch (const std::exception& e) {
                        std::cerr << "Error: Invalid seed value specified. Use a valid unsigned integer.\n";
                        return false;
                    }
                } else {
                    std::cerr << "Error: --seed requires a following value.\n";
                    return false;
                }
            }
            
            else {
                std::cerr << "Warning: Unknown argument encountered: " << current_arg << "\n";
                return false;
            }
        }
    
        std::cout << "Arguments successfully parsed. Configuration loaded.\n";
        return true;
    }
}

