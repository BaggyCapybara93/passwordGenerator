#include "password_entropy.hpp"
#include <iostream>

double calculate_entropy(const std::string& password, std::shared_ptr<Settings> settings) {
    if (password.empty()) {
        return 0.0;
    }

    bool has_upper = false;
    bool has_lower = false;
    bool has_digit = false;
    bool has_special = false;

    for (char c : password) {
        if (std::isupper(static_cast<unsigned char>(c))) {
            has_upper = true;
        } else if (std::islower(static_cast<unsigned char>(c))) {
            has_lower = true;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            has_digit = true;
        } else {
            has_special = true;
        }
    }

    size_t pool_size = 0;
    if (has_upper)  pool_size += settings->uppercase_string.size();
    if (has_lower)  pool_size += settings->lowercase_string.size();
    if (has_digit)  pool_size += settings->digits_string.size();
    if (has_special) pool_size += settings->special_string.size();

    return static_cast<double>(password.size()) * std::log2(static_cast<double>(pool_size));
}

std::string calculate_security_score(const double& entropy, std::shared_ptr<Settings> settings) {
    try{
        // expected guesses = 2^(entropy - 1)
        long double log2_expected_guesses = entropy - 1.0L;

        long double log2_seconds = log2_expected_guesses - std::log2(settings->guesses_per_second);

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