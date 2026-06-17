#include "ui_helpers.hpp"
#include <iostream>

namespace UI {

    void print_separator() {
        print_with_color("==================================================",
                         Color::Blue, false, true);
    }

    void print_charset_info(const std::string& custom_chars,
                            const std::string& exclude_chars,
                            bool no_color,
                            bool exclude_ambiguous)
    {
        print_with_color("Character Set Information:", Color::Cyan, true, no_color);

        if (!custom_chars.empty()) {
            print_with_color("  Custom characters: " + custom_chars,
                             Color::Green, true, no_color);
        }

        if (!exclude_chars.empty()) {
            print_with_color("  Excluded characters: " + exclude_chars,
                             Color::Yellow, true, no_color);
        }

        if (exclude_ambiguous) {
            print_with_color("  Ambiguous characters excluded",
                             Color::Magenta, true, no_color);
        }
    }

}
