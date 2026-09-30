#include "password_entropy.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

#include <boost/multiprecision/cpp_int.hpp>

namespace {
using BigInt = boost::multiprecision::cpp_int;

constexpr unsigned uppercase_bit = 1U << 0U;
constexpr unsigned lowercase_bit = 1U << 1U;
constexpr unsigned digit_bit = 1U << 2U;
constexpr unsigned special_bit = 1U << 3U;

bool is_ascii_uppercase(char c) { return c >= 'A' && c <= 'Z'; }
bool is_ascii_lowercase(char c) { return c >= 'a' && c <= 'z'; }
bool is_ascii_digit(char c) { return c >= '0' && c <= '9'; }
bool is_ascii_punctuation(char c) {
    return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') ||
           (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

unsigned char_category(char c) {
    if (is_ascii_uppercase(c)) return uppercase_bit;
    if (is_ascii_lowercase(c)) return lowercase_bit;
    if (is_ascii_digit(c)) return digit_bit;
    if (is_ascii_punctuation(c)) return special_bit;
    return 0U;
}

BigInt integer_power(BigInt base, size_t exponent) {
    BigInt result = 1;
    while (exponent > 0) {
        if ((exponent & 1U) != 0U) result *= base;
        exponent >>= 1U;
        if (exponent > 0) base *= base;
    }
    return result;
}

double log2_big_integer(const BigInt& value) {
    if (value <= 0) return 0.0;
    const size_t highest_bit = boost::multiprecision::msb(value);
    constexpr size_t retained_bits = 63;
    if (highest_bit <= retained_bits) {
        return std::log2(value.convert_to<double>());
    }
    const size_t shift = highest_bit - retained_bits;
    const BigInt leading_bits = value >> shift;
    return std::log2(leading_bits.convert_to<double>()) + static_cast<double>(shift);
}

std::string unique_filtered_pool(const Settings& settings) {
    const std::string source = settings.custom_chars.empty()
        ? settings.uppercase_string + settings.lowercase_string + settings.digits_string + settings.special_string
        : settings.custom_chars;
    std::string pool;
    pool.reserve(source.size());
    for (char c : source) {
        if (c < ' ' || c > '~' || settings.exclude_chars.find(c) != std::string::npos ||
            (settings.exclude_ambiguous && settings.ambiguous_chars.find(c) != std::string::npos)) {
            continue;
        }
        if (settings.custom_chars.empty()) {
            if ((is_ascii_uppercase(c) && !settings.req_uppercase) ||
                (is_ascii_lowercase(c) && !settings.req_lowercase) ||
                (is_ascii_digit(c) && !settings.req_digits) ||
                (settings.special_string.find(c) != std::string::npos && !settings.req_special)) {
                continue;
            }
        }
        if (pool.find(c) == std::string::npos) pool.push_back(c);
    }
    return pool;
}

unsigned required_mask(const Settings& settings) {
    unsigned mask = 0U;
    if (settings.req_uppercase) mask |= uppercase_bit;
    if (settings.req_lowercase) mask |= lowercase_bit;
    if (settings.req_digits) mask |= digit_bit;
    if (settings.req_special) mask |= special_bit;
    return mask;
}

BigInt count_valid_outputs(const Settings& settings,
                           const std::unordered_set<std::string>* blacklist = nullptr) {
    const std::string pool = unique_filtered_pool(settings);
    const unsigned required = required_mask(settings);
    const unsigned pool_size = static_cast<unsigned>(pool.size());
    BigInt valid = 0;

    // Inclusion-exclusion counts the strings that contain every enabled group.
    for (unsigned subset = required; ; subset = (subset - 1U) & required) {
        unsigned excluded_characters = 0U;
        unsigned parity = 0U;
        for (char c : pool) {
            if ((char_category(c) & subset) != 0U) ++excluded_characters;
        }
        for (unsigned bits = subset; bits != 0U; bits >>= 1U) parity += bits & 1U;

        const BigInt term = integer_power(BigInt(pool_size - excluded_characters), settings.length);
        if ((parity & 1U) == 0U) valid += term;
        else valid -= term;

        if (subset == 0U) break;
    }
    if (blacklist != nullptr && valid > 0) {
        for (const auto& password : *blacklist) {
            if (password.size() == settings.length &&
                password_meets_character_requirements(password, settings) &&
                password.find_first_not_of(pool) == std::string::npos) {
                --valid;
            }
        }
    }
    return valid;
}
} // namespace

std::string build_effective_character_pool(const Settings& settings) {
    return unique_filtered_pool(settings);
}

bool password_meets_character_requirements(const std::string& password, const Settings& settings) {
    unsigned found = 0U;
    for (char c : password) found |= char_category(c);
    return (found & required_mask(settings)) == required_mask(settings);
}

bool has_valid_output_space(const Settings& settings,
                            const std::unordered_set<std::string>& blacklist) {
    return count_valid_outputs(settings, &blacklist) > 0;
}

double calculate_generation_entropy(const Settings& settings,
                                    const std::unordered_set<std::string>& blacklist) {
    const BigInt valid = count_valid_outputs(settings, &blacklist);
    if (valid <= 0) return 0.0;
    return log2_big_integer(valid);
}

std::string calculate_security_score(double entropy, const Settings& settings) {
    // This is a rough offline-guessing estimate based on the configured rate.
    const long double log2_expected_guesses = static_cast<long double>(entropy) - 1.0L;
    const long double log2_seconds = log2_expected_guesses - std::log2(settings.guesses_per_second);
    if (log2_seconds < std::log2(60.0L)) return "Very Weak";
    if (log2_seconds < std::log2(3600.0L)) return "Weak";
    if (log2_seconds < std::log2(2629800.0L)) return "Moderate";
    if (log2_seconds < std::log2(3.15576e9L)) return "Strong";
    return "Very Strong";
}
