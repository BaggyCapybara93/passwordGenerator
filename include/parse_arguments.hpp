#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include <cstdint>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct Settings {
    size_t desired_length = 12;
    std::optional<uint64_t> seed = std::nullopt;
    int num_passwords = 1;
    bool req_uppercase = true;
    bool req_lowercase = true;
    bool req_digits = true;
    bool req_special = true;
    bool no_color = false;
    std::string custom_chars = "";           // Custom character pool
    std::string exclude_chars = "";          // Characters to exclude from default pools
    std::string blacklist = "";              // Comma-separated list of passwords to blacklist (e.g., "{pass1,pass2,pass3}")
    bool exclude_ambiguous = false;          // Exclude ambiguous characters (0/O, 1/l/I)
    double min_entropy = 0.0;                // Minimum entropy threshold in bits (default: 0 means no minimum)
    bool is_honeypassword = false;
};

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
