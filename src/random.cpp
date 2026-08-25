#include "random.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <bcrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

uint64_t RNG::secure_random_uint64() {
    uint64_t value = 0;

#ifdef _WIN32
    const NTSTATUS status = BCryptGenRandom(
        nullptr,
        reinterpret_cast<PUCHAR>(&value),
        static_cast<ULONG>(sizeof(value)),
        BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (status < 0) {
        throw std::runtime_error("The operating system random-number generator failed.");
    }
#else
    const int random_fd = open("/dev/urandom", O_RDONLY);
    if (random_fd < 0) {
        throw std::runtime_error("Unable to open the operating system random-number generator.");
    }

    size_t bytes_read = 0;
    while (bytes_read < sizeof(value)) {
        const ssize_t count = read(
            random_fd,
            reinterpret_cast<char*>(&value) + bytes_read,
            sizeof(value) - bytes_read);
        if (count <= 0) {
            close(random_fd);
            throw std::runtime_error("Unable to read from the operating system random-number generator.");
        }
        bytes_read += static_cast<size_t>(count);
    }

    close(random_fd);
#endif

    return value;
}

std::string RNG::random_word() {
    try {
        static const std::vector<std::string> fallback_words = {
            "cat", "sun", "blue", "tree", "star", "moon",
            "happy", "cool", "water", "light", "shadow",
            "password", "car", "secret", "word", "cheese"
        };

        std::vector<std::string> words;
        words.reserve(fallback_words.size());

        auto trim = [&](std::string value) {
            auto not_space = [](unsigned char ch) { return !std::isspace(ch); };
            value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
            value.erase(std::find_if(value.rbegin(), value.rend(), not_space).base(), value.end());
            return value;
        };

        if (!settings_.get()->wordlist_file.empty()) {
            std::vector<std::string> lines = file_manager_.get()->load_lines(settings_.get()->wordlist_file);
            for (auto& line : lines) {
                std::string entry = trim(line);
                if (!entry.empty()) {
                    words.push_back(entry);
                }
            }
        }

        if (words.empty()) {
            words = fallback_words;
        }

        return words[random_index(words.size())];
    } catch (const std::exception& e) {
        throw std::runtime_error("Error generating random word: " + std::string(e.what()));
    }
}

std::string RNG::build_default_pool() {
    return settings_.get()->uppercase_string + settings_.get()->lowercase_string + settings_.get()->digits_string + settings_.get()->special_string;
}

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
        deterministic_engine_.seed(seed_value.value());
        deterministic_mode_ = true;
    } else {
        deterministic_mode_ = false;
    }
}

size_t RNG::random_index(size_t upper_bound) {
    if (upper_bound == 0) {
        throw std::invalid_argument("Upper bound must be greater than zero.");
    }

    std::lock_guard<std::mutex> lock(engine_mutex_);
    if (deterministic_mode_) {
        std::uniform_int_distribution<size_t> distribution(0, upper_bound - 1);
        return distribution(deterministic_engine_);
    }

    const uint64_t bound = static_cast<uint64_t>(upper_bound);
    const uint64_t limit = std::numeric_limits<uint64_t>::max() -
                           (std::numeric_limits<uint64_t>::max() % bound);
    uint64_t value = 0;
    do {
        value = secure_random_uint64();
    } while (value >= limit);

    return static_cast<size_t>(value % bound);
}

char RNG::select_char(const std::string& charset){
    try{
        if (charset.empty()) {
            throw std::invalid_argument("Character set is empty - cannot select character!");
        }

        const size_t index = random_index(charset.size());

        return charset[index];
    }catch(const std::exception& e) {
        throw std::runtime_error("Error selecting character: " + std::string(e.what()));
    }
}

void RNG::shuffle_chars(std::vector<char>& values) {
    for (size_t i = values.size(); i > 1; --i) {
        std::swap(values[i - 1], values[random_index(i)]);
    }
}
