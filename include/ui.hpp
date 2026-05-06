#pragma once
#include <string>
#include <iostream>
#include <vector>

struct Settings {
    size_t desiredLength = 12;
    std::optional<uint64_t> seed = std::nullopt;
    int numPasswords = 1;
    bool reqUppercase = true;
    bool reqLowercase = true;
    bool reqDigits = true;
    bool reqSpecial = true;
};

namespace UI {

    enum class Color{
        Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    };

    const char* getColor(Color c);

    void printColored(std::string text, Color color, bool newLine = true);

    void printReset();

    void printSeparator();

    bool parseArgs(int argc, char* arg[], Settings& settings);
};
