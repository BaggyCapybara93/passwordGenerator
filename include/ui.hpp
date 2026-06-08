#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <optional>

namespace UI {

    enum class Color{
        Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    };

    const char* get_color(Color c);

    void print_colored(std::string text, Color color, bool new_line = true);

    void print_reset();

    void print_separator();
};
