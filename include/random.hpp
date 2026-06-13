#pragma once
#include <mutex>
#include <optional>
#include <string>
#include <vector>
#include <random>
#include <set>

class RNG{
    private:
        static std::mt19937_64 engine_;
        static std::random_device device_;
        static std::mutex engine_mutex_;

        static std::string random_word();
    
    public:
        static std::set<std::string> parse_blacklist(const std::string& blacklist_str);
        static void seed(std::optional<uint64_t> seedValue);

        static std::string generate(
            size_t length,
            bool requires_uppercase = true,
            bool requires_lowercase = true,
            bool requires_digits = true,
            bool requires_special = true,
            const std::string& custom_chars = "",
            const std::string& exclude_chars = "",
            const std::set<std::string>& blacklist = std::set<std::string>(),
            bool exclude_ambiguous = false
        );

        static char select_char(const std::string& charset);

        /**
         * @brief Build the default character pool (uppercase + lowercase + digits + special)
         * @return The default 92-character pool
         */
        static std::string build_default_pool();

        /**
         * @brief Build a custom character pool from provided characters
         * @param chars The custom character string
         * @return The custom character pool
         */
        static std::string build_custom_pool(const std::string& chars);

        /**
         * @brief Exclude specified characters from a pool
         * @param pool The original character pool
         * @param exclude Characters to remove
         * @return The pool with excluded characters removed
         */
        static std::string exclude_chars_from_pool(const std::string& pool, const std::string& exclude);

        /**
         * @brief Exclude ambiguous characters from a pool
         * @param pool The original character pool
         * @param exclude_ambiguous Whether to exclude ambiguous characters
         * @return The pool with ambiguous characters removed if requested
         */
        static std::string exclude_ambiguous_from_pool(const std::string& pool, bool exclude_ambiguous);

        /**
         * @brief Calculate the entropy of a password in bits
         * @param password The password to calculate entropy for
         * @return Entropy in bits (higher = more secure)
         */
        static double calculate_entropy(const std::string& password);

        /**
         * @brief Generate a honey password (intentionally weak, designed to be compromised)
         * @param length The desired password length
         * @return A weak password that is commonly used and easily guessable
         */
        static std::string generate_honey_password(size_t length);

 };