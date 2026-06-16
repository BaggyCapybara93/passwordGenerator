#pragma once
#include <iostream>
#include <vector>
#include <optional>
#include <cstdint>
#include <boost/program_options.hpp>

#include "settings.hpp"

namespace po = boost::program_options;

namespace parse_arguments {

    /**
     * @brief Parse command line arguments using boost::program_options
     * @param argc Number of command line arguments
     * @param argv Array of command line arguments
     * @param settings Reference to Settings struct to populate
     * @return true if parsing succeeded, false if help was requested or error occurred
     */
    bool parse_args(int argc, char* argv[], Settings& settings);

    /**
     * @brief Print usage information and exit
     */
    void print_help();

} // namespace parse_arguments
