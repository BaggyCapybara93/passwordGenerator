#include "ansi_printer.hpp"
#include "color.hpp"
#include <iostream>

namespace UI {

    void print_raw(const std::string& text) {
        std::cout << text;
    }

    void print_with_color(const std::string& text, Color color, bool no_color, bool newline) {
        const char* code = UI::to_ansi(color, no_color);
        const char* reset = no_color ? "" : "\033[0m";

        std::cout << code << text << reset;
        if (newline) {
            std::cout << "\n";
        }
    }

    void reset_color(bool no_color) {
        if (!no_color) {
            std::cout << "\033[0m";
        }
    }

}
