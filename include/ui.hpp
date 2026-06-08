#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <optional>

struct Settings {
    size_t desired_length = 12;
    std::optional<uint64_t> seed = std::nullopt;
    int num_passwords = 1;
    bool req_uppercase = true;
    bool req_lowercase = true;
    bool req_digits = true;
    bool req_special = true;
};

namespace UI {

    enum class Color{
        Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    };

    const char* get_color(Color c);

    void print_colored(std::string text, Color color, bool new_line = true);

    void print_reset();

    void print_separator();

    bool parse_args(int argc, char* argv[], Settings& settings);
};
