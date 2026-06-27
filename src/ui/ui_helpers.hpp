#pragma once
#include <string>
#include "ansi_printer.hpp"

namespace UI {

    /**
         * @brief Print separator
    */
    void print_separator();

    /**
         * @brief Print charset info
         * @param custom_chars Custom characters to include
         * @param exclude_chars Characters to exclude
         * @param no_color Disable color output
         * @param exclude_ambiguous Whether to exclude ambiguous characters
    */
    void print_charset_info(
        const std::string& custom_chars,
        const std::string& exclude_chars,
        bool no_color = false,
        bool exclude_ambiguous = false
    );

}