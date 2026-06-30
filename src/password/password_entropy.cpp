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
        long double total_possibilities = powl(2.0L, entropy);
        long double expected_guesses = total_possibilities / 2.0L;
        long double seconds = expected_guesses / settings.get()->guesses_per_second;

        if (seconds < 60)
            return "Very Weak";
        else if (seconds < 3600)
            return "Weak";
        else if (seconds < 2629800) // ~1 month
            return "Moderate";
        else if (seconds < 3.15576e9) // ~100 years
            return "Strong";
        else
            return "Very Strong";
    } catch(const std::exception& e){
        std::cout << "Error calculating security score: " << e.what() << std::endl;
        throw;
    }
}