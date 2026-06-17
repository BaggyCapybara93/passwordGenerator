#include "color.hpp"

namespace UI {
    const char* to_ansi(Color c, bool no_color) {
        if (no_color) return "";

        switch (c) {
            case Color::Black:   return "\033[30m";
            case Color::Red:     return "\033[31m";
            case Color::Green:   return "\033[32m";
            case Color::Yellow:  return "\033[33m";
            case Color::Blue:    return "\033[34m";
            case Color::Magenta: return "\033[35m";
            case Color::Cyan:    return "\033[36m";
            case Color::White:   return "\033[37m";
            default:             return "\033[0m";
        }
    }
}
