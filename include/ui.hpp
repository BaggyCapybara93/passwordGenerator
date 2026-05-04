#pragma once
#include <string>
#include <iostream>
#include <vector>

namespace UI {

    extern size_t desiredLength_;
    extern int numPasswords_;
    extern bool reqUppercase_ ;
    extern bool reqLowercase_;
    extern bool reqDigits_;
    extern bool reqSpecial_;

    enum class Color{
        Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    };

    const char* getColor(Color c);

    void printColored(std::string text, Color color, bool newLine = true);

    void printReset();

    void printSeparator();

    int intInput(const std::string& userPrompt, size_t minValue, size_t maxValue);

    bool boolInput(const std::string& userPrompt);

    bool parseArgs(int argc, char* arg[]);
};
