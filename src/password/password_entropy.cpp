#include "password_entropy.hpp"
#include <iostream>

double calculate_entropy(const std::string& password, std::shared_ptr<Settings> settings) {
    if (password.empty()) {
        return 0.0;
    }
    
    // Pool sizes for each character type
    const size_t uppercase_size = settings.get()->uppercase_string.size();
    const size_t lowercase_size = settings.get()->lowercase_string.size();
    const size_t digits_size = settings.get()->digits_string.size();
    const size_t special_size = settings.get()->special_string.size();
    
    // Calculate entropy: sum of log2(pool_size) for each character
    double entropy = 0.0;
    for (size_t i = 0; i < password.size(); ++i) {
        char c = password[i];
        double pool_size = 0.0;
        
        if (c >= 'A' && c <= 'Z') {
            pool_size = static_cast<double>(uppercase_size);
        } else if (c >= 'a' && c <= 'z') {
            pool_size = static_cast<double>(lowercase_size);
        } else if (c >= '0' && c <= '9') {
            pool_size = static_cast<double>(digits_size);
        } else if (c >= '!' && c <= '~') {  // Special chars are in ASCII range 33-126
            pool_size = static_cast<double>(special_size);
        }
        
        if (pool_size > 0) {
            entropy += std::log2(pool_size);
        }
    }
    
    return entropy;
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