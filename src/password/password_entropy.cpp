#include "password_entropy.hpp"
#include <cmath>
#include <iostream>

std::string build_effective_character_pool(const Settings& settings) {
    std::string pool;
    if (!settings.custom_chars.empty()) {
        pool = settings.custom_chars;
    } else {
        if (settings.req_uppercase) pool += settings.uppercase_string;
        if (settings.req_lowercase) pool += settings.lowercase_string;
        if (settings.req_digits) pool += settings.digits_string;
        if (settings.req_special) pool += settings.special_string;
    }

    std::string unique_pool;
    unique_pool.reserve(pool.size());
    for (char c : pool) {
        const bool printable_ascii = c >= ' ' && c <= '~';
        const bool excluded = settings.exclude_chars.find(c) != std::string::npos;
        const bool ambiguous = settings.exclude_ambiguous && settings.ambiguous_chars.find(c) != std::string::npos;
        if (printable_ascii && !excluded && !ambiguous && unique_pool.find(c) == std::string::npos) {
            unique_pool += c;
        }
    }

    return unique_pool;
}

double calculate_entropy(const std::string& password, const Settings& settings) {
    if (password.empty()) {
        return 0.0;
    }

    const std::string pool = build_effective_character_pool(settings);
    if (pool.empty()) {
        return 0.0;
    }

    return static_cast<double>(password.size()) * std::log2(static_cast<double>(pool.size()));
}

std::string calculate_security_score(double entropy, const Settings& settings) {
    try{
        // expected guesses = 2^(entropy - 1)
        long double log2_expected_guesses = entropy - 1.0L;

        long double log2_seconds = log2_expected_guesses - std::log2(settings.guesses_per_second);

        // Convert thresholds to log2 space
        const long double log2_minute = std::log2(60.0L);
        const long double log2_hour   = std::log2(3600.0L);
        const long double log2_month  = std::log2(2629800.0L);
        const long double log2_century = std::log2(3.15576e9L);

        if (log2_seconds < log2_minute)
            return "Very Weak";
        else if (log2_seconds < log2_hour)
            return "Weak";
        else if (log2_seconds < log2_month)
            return "Moderate";
        else if (log2_seconds < log2_century)
            return "Strong";
        else
            return "Very Strong";
    } catch(const std::exception& e){
        std::cout << "Error calculating security score: " << e.what() << std::endl;
        throw;
    }
}
