#include "ui.hpp"
#include <algorithm>
#include <cctype>

namespace UI{

    const char* get_color(UI::Color c, bool no_color) {
        if (no_color) return "";
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

    void print_colored(std::string text, Color color, bool new_line, bool no_color){
        if (!no_color) {
            const char* code_str = UI::get_color(color);
            print_escape_code(code_str);
        }
        std::cout << text; 
        if (new_line) std::cout << "\033[0m\n";
    }
    
    void print_reset(bool no_color){
        if (!no_color) std::cout << "\033[0m";
    }
    
    void print_separator(){
        std::cout << "\n--------------------------------------------------\n";
    }
    
    void print_charset_info(const std::string& custom_chars, const std::string& exclude_chars, bool no_color, bool exclude_ambiguous){
        if (custom_chars.empty() && exclude_chars.empty() && !exclude_ambiguous) {
            return;
        }
        
        if (!no_color) {
            std::cout << "\033[34m"; // Blue
        }
        std::cout << "\nCharacter Set Information:\n";
        
        if (!custom_chars.empty()) {
            if (!no_color) std::cout << "\033[32m"; // Green
            std::cout << "  Custom Character Pool: " << custom_chars << "\n";
            std::cout << "  Pool Size: " << custom_chars.size() << " characters\n";
            if (!no_color) std::cout << "\033[0m";
        }
        
        if (!exclude_chars.empty()) {
            if (!no_color) std::cout << "\033[33m"; // Yellow
            std::cout << "  Excluded Characters: " << exclude_chars << "\n";
            std::cout << "  Excluded Count: " << exclude_chars.size() << " characters\n";
            if (!no_color) std::cout << "\033[0m";
        }
        
        if (exclude_ambiguous) {
            if (!no_color) std::cout << "\033[36m"; // Cyan
            std::cout << "  Excluded Ambiguous Characters: 0/O, 1/l/I\n";
            std::cout << "  This prevents confusion between similar-looking characters\n";
            if (!no_color) std::cout << "\033[0m";
        }
        
        if (!no_color) std::cout << "\033[0m";
    }
}

