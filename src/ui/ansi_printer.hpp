#pragma once
#include <string>
#include "color.hpp"

namespace UI {

    void print_raw(const std::string& text);
    void print_with_color(const std::string& text, Color color, bool no_color, bool newline);

    void reset_color(bool no_color);
}
