#pragma once
#include <string>
#include "ansi_printer.hpp"

namespace UI {

    void print_separator();
    void print_charset_info(
        const std::string& custom_chars,
        const std::string& exclude_chars,
        bool no_color = false,
        bool exclude_ambiguous = false
    );

}