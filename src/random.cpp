#include "random.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <set>
#include <string>

std::mt19937_64 RNG::engine_;
std::random_device RNG::device_;
std::mutex RNG::engine_mutex_;

/**
 * @brief Randomly generate a work from a list
 */
std::string RNG::random_word() {
    try{
        //Replace this with a txt based list of possible works
        static const std::vector<std::string> words = {
            "cat", "sun", "blue", "tree", "star", "moon",
            "happy", "cool", "water", "light", "shadow",
            "password", "car", "secert", "word", "cheese"
        };
        std::uniform_int_distribution<size_t> dist(0, words.size() - 1);
        return words[dist(engine_)];
    }catch(const std::exception& e) {
        throw std::runtime_error("Error generating random word: " + std::string(e.what()));
    }
}

/**
 * @brief Build the default character pool (uppercase + lowercase + digits + special)
 * @return The default 92-character pool
 */
std::string RNG::build_default_pool() {
    return settings_.get()->uppercase_string + settings_.get()->lowercase_string + settings_.get()->digits_string + settings_.get()->special_string;
}

/**
 * @brief Build a custom character pool from provided characters
 * @param chars The custom character string
 * @return The custom character pool
 */
std::string RNG::build_custom_pool(const std::string& chars) {
    try{
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
    } catch(const std::exception& e) {
        throw std::runtime_error("Error building custom character pool: " + std::string(e.what()));
    }
}

/**
 * @brief Exclude specified characters from a pool
 * @param pool The original character pool
 * @param exclude Characters to remove
 * @return The pool with excluded characters removed
 */
std::string RNG::exclude_chars_from_pool(const std::string& pool, const std::string& exclude) {
    try{
        std::string result;
        result.reserve(pool.size());
        
        for (char c : pool) {
            if (exclude.find(c) == std::string::npos) {
                result += c;
            }
        }
        
        return result;
    }catch(const std::exception& e) {
        throw std::runtime_error("Error excluding characters from pool: " + std::string(e.what()));
    }
}

/**
 * @brief Exclude ambiguous characters from a pool
 * @param pool The original character pool
 * @param exclude_ambiguous Whether to exclude ambiguous characters
 * @return The pool with ambiguous characters removed if requested
 */
std::string RNG::exclude_ambiguous_from_pool(const std::string& pool, bool exclude_ambiguous) {
    try{
        if (!exclude_ambiguous) {
            return pool;
        }
        
        std::string result;
        result.reserve(pool.size());
        
        for (char c : pool) {
            if (settings_.get()->ambiguous_chars.find(c) == std::string::npos) {
                result += c;
            }
        }
        
        return result;
    }catch(const std::exception& e) {
        throw std::runtime_error("Error excluding ambiguous characters from pool: " + std::string(e.what()));
    }
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
    try{
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
    }catch(const std::exception& e) {
        throw std::runtime_error("Error selecting character: " + std::string(e.what()));
    }
}