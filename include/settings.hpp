#pragma once

#include <string>
#include <optional>
#include <cstdint>

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