#pragma once
#include <string>
#include "color.hpp"

namespace UI {

    /**
         * @brief Print text with no color or formatting
         * @param text The text to print
    */
    void print_raw(const std::string& text);

    /**
         * @brief Print text with color and/or formatting
         * @param text The text to print
         * @param color Color of the text to print
         * @param no_color Disable color output
         * @param newline Whether to add a newline at the end
    */
    void print_with_color(const std::string& text, Color color, bool no_color, bool newline);

     /**
         * @brief Reset color of text
         * @param no_color Disable color output
    */
    void reset_color(bool no_color);
}
