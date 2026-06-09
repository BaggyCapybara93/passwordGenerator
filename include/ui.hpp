#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <optional>

namespace UI {

    enum class Color{
        Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    };

    const char* get_color(Color c, bool no_color = false);

    void print_colored(std::string text, Color color, bool new_line = true, bool no_color = false);

    void print_reset(bool no_color = false);

    void print_separator();
};
