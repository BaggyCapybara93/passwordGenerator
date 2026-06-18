#pragma once
#include <mutex>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <random>
#include <set>

#include "settings.hpp"

class RNG{
    private:
        std::shared_ptr<Settings> settings_;
    
    public:

        //TEMP: change this later
        static std::mt19937_64 engine_;
        static std::random_device device_;
        static std::mutex engine_mutex_;

        RNG(std::shared_ptr<Settings> settings) : settings_(std::move(settings)) {}
        void seed(std::optional<uint64_t> seedValue);

        std::string random_word();

        char select_char(const std::string& charset);

        /**
         * @brief Build the default character pool (uppercase + lowercase + digits + special)
         * @return The default 92-character pool
         */
        std::string build_default_pool();

        /**
         * @brief Build a custom character pool from provided characters
         * @param chars The custom character string
         * @return The custom character pool
         */
        std::string build_custom_pool(const std::string& chars);

        /**
         * @brief Exclude specified characters from a pool
         * @param pool The original character pool
         * @param exclude Characters to remove
         * @return The pool with excluded characters removed
         */
        std::string exclude_chars_from_pool(const std::string& pool, const std::string& exclude);

        /**
         * @brief Exclude ambiguous characters from a pool
         * @param pool The original character pool
         * @param exclude_ambiguous Whether to exclude ambiguous characters
         * @return The pool with ambiguous characters removed if requested
         */
        std::string exclude_ambiguous_from_pool(const std::string& pool, bool exclude_ambiguous);
};