#pragma once
#include <string>

namespace UI {

    enum class Color {
        Default, Black, Red, Green, Yellow, Blue, Magenta, Cyan, White
    };

    const char* to_ansi(Color c, bool no_color = false);

}
