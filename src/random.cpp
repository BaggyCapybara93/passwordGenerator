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

/**
 * @brief Build the default character pool (uppercase + lowercase + digits + special)
 * @return The default 92-character pool
 */
std::string RNG::build_default_pool() {
    return uppercase_string + lowercase_string + digits_string + special_string;
}

/**
 * @brief Build a custom character pool from provided characters
 * @param chars The custom character string
 * @return The custom character pool
 */
std::string RNG::build_custom_pool(const std::string& chars) {
    if (chars.empty()) {
        return "";
    }
    
    // Remove duplicates while preserving order
    std::string unique_chars;
    unique_chars.reserve(chars.size());
    for (char c : chars) {
        // Only include printable ASCII characters
        if (c >= ' ' && c <= '~' && unique_chars.find(c) == std::string::npos) {
            unique_chars += c;
        }
    }
    
    return unique_chars;
}

/**
 * @brief Exclude specified characters from a pool
 * @param pool The original character pool
 * @param exclude Characters to remove
 * @return The pool with excluded characters removed
 */
std::string RNG::exclude_chars_from_pool(const std::string& pool, const std::string& exclude) {
    std::string result;
    result.reserve(pool.size());
    
    for (char c : pool) {
        if (exclude.find(c) == std::string::npos) {
            result += c;
        }
    }
    
    return result;
}

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

std::string RNG::generate(size_t length, bool requires_uppercase, bool requires_lowercase, bool requires_digits, bool requires_special, const std::string& custom_chars, const std::string& exclude_chars) {
    std::vector<char> result;
    
    // Determine which character pools to use
    std::string up_pool, low_pool, dig_pool, sp_pool;
    
    if (!custom_chars.empty()) {
        // Use custom character pool
        std::string custom_pool = build_custom_pool(custom_chars);
        
        // Distribute custom characters to pools based on requirements
        if (requires_uppercase) {
            up_pool = custom_pool;
        }
        if (requires_lowercase) {
            low_pool = custom_pool;
        }
        if (requires_digits) {
            dig_pool = custom_pool;
        }
        if (requires_special) {
            sp_pool = custom_pool;
        }
    } else {
        // Use default pools, optionally excluding characters
        if (!exclude_chars.empty()) {
            std::string default_pool = build_default_pool();
            std::string excluded_pool = exclude_chars_from_pool(default_pool, exclude_chars);
            
            if (requires_uppercase) up_pool = excluded_pool;
            if (requires_lowercase) low_pool = excluded_pool;
            if (requires_digits) dig_pool = excluded_pool;
            if (requires_special) sp_pool = excluded_pool;
        } else {
            // Use default pools as-is
            if (requires_uppercase) up_pool = uppercase_string;
            if (requires_lowercase) low_pool = lowercase_string;
            if (requires_digits) dig_pool = digits_string;
            if (requires_special) sp_pool = special_string;
        }
    }

    auto ensure_char = [&](const std::string& s, bool req) {
        if (req && !s.empty()) {
            result.push_back(select_char(s));
        }
    };

    ensure_char(up_pool, requires_uppercase);
    ensure_char(low_pool, requires_lowercase);
    ensure_char(dig_pool, requires_digits);
    ensure_char(sp_pool, requires_special);

    size_t current_size = result.size();
    if (length < current_size) {
        std::string msg = "Requested password length (" + std::to_string(length) + 
                          ") is too short for requirements (" + 
                          std::to_string(current_size) + " minimum).";
        throw std::invalid_argument(msg);
    }
    size_t remaining = length - current_size;

    // Build the combined pool for remaining characters
    std::string all_pool = "";
    if (requires_uppercase) all_pool += up_pool;
    if (requires_lowercase) all_pool += low_pool;
    if (requires_digits) all_pool += dig_pool;
    if (requires_special) all_pool += sp_pool;

    if (all_pool.empty()) {
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