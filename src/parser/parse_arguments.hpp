#pragma once
#include <iostream>
#include <vector>
#include <optional>
#include <cstdint>
#include <boost/program_options.hpp>

#include "settings.hpp"

namespace po = boost::program_options;

enum class ParseResult { success, help, error };

class ParseArguments{
    private:
        std::string program_name = "PasswordGenerator";
    public:
        /**
         * @brief Parse command line arguments using boost::program_options
         * @param argc Number of command line arguments
         * @param argv Array of command line arguments
         * @param settings Reference to Settings struct to populate
         * @return Whether parsing succeeded, help was requested, or an error occurred
         */
        ParseResult parse_args(int argc, char* argv[], Settings& settings);

        /**
         * @brief Validate settings and build the final character pool
         * @param settings Reference to Settings struct to validate and modify
         * @return true if validation succeeded, false if validation failed
         */
        bool validate_settings(Settings& settings);

        /**
         * @brief Print usage information and exit
         */
        void print_help();
};
