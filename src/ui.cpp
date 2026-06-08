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
}

