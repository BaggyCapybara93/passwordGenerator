#include "random.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

static const std::string uppercase_string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const std::string lowercase_string = "abcdefghijklmnopqrstuvwxyz";
static const std::string digits_string = "0123456789";
static const std::string special_string = "!@#$%^&*()_+~`|}{[]:;?><,./-=";

std::mt19937_64 RNG::engine_;
std::random_device RNG::device_;
std::mutex RNG::engine_mutex_;

void RNG::seed(std::optional<uint64_t> seed_value){
    std::lock_guard<std::mutex> lock(engine_mutex_);

    if (seed_value.has_value()) {
        uint64_t specific_seed = seed_value.value();
        engine_.seed(specific_seed);
    } else {
        engine_.seed(device_());
    }
}

char RNG::select_char(const std::string& charset){
    if (charset.empty()) {
        throw std::invalid_argument("Character set is empty - cannot select character!");
    }

    size_t max_index = static_cast<size_t>(charset.size() - 1); 
    std::uniform_int_distribution<size_t> dist(0, max_index);

    size_t index = dist(engine_);

    if (index >= charset.size()) {
        throw std::runtime_error("Random number distribution out of bounds!");
    }

    return charset[index];
}

std::string RNG::generate(size_t length, bool requires_uppercase, bool requires_lowercase, bool requires_digits, bool requires_special){
    std::vector<char> result;
    
    // Define pools
    const std::string& up = uppercase_string;
    const std::string& low = lowercase_string;
    const std::string& dig = digits_string;
    const std::string& sp = special_string;

    auto ensure_char = [&](const std::string& s, bool req) {
        if (req && !s.empty()) {
            result.push_back(select_char(s));
        }
    };

    ensure_char(up, requires_uppercase);
    ensure_char(low, requires_lowercase);
    ensure_char(dig, requires_digits);
    ensure_char(sp, requires_special);

    size_t current_size = result.size();
    if (length < current_size) {
        std::string msg = "Requested password length (" + std::to_string(length) + 
                          ") is too short for requirements (" + 
                          std::to_string(current_size) + " minimum).";
        throw std::invalid_argument(msg);
    }
    size_t remaining = length - current_size;

    std::string all_pool = "";
    if(requires_uppercase) all_pool += up;
    if(requires_lowercase) all_pool += low;
    if(requires_digits) all_pool += dig;
    if(requires_special) all_pool += sp;

    if(all_pool.empty()) {
        throw std::invalid_argument("No character set defined to generate remaining characters!");
    }

    while(static_cast<size_t>(result.size()) < length && remaining > 0){
        result.push_back(select_char(all_pool));
        --remaining;
    }

    {
        std::lock_guard<std::mutex> lock(engine_mutex_);
        std::shuffle(result.begin(), result.end(), engine_);
    }

    return std::string(result.begin(), result.end());
}

/**
 * @brief Calculate the entropy of a password in bits
 * Entropy is calculated as the sum of log2(pool_size) for each character
 * Higher entropy = more unpredictable = more secure
 */
double RNG::calculate_entropy(const std::string& password) {
    if (password.empty()) {
        return 0.0;
    }
    
    // Pool sizes for each character type
    const size_t uppercase_size = uppercase_string.size();
    const size_t lowercase_size = lowercase_string.size();
    const size_t digits_size = digits_string.size();
    const size_t special_size = special_string.size();
    
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