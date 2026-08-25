#pragma once
#include <mutex>
#include <memory>
#include <cstddef>
#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include <random>

#include "settings.hpp"
#include "file_manager/file_manager.hpp"

class RNG{
    private:
        std::shared_ptr<Settings> settings_;

        std::shared_ptr<file_manager> file_manager_;
        std::mt19937_64 deterministic_engine_;
        std::mutex engine_mutex_;
        bool deterministic_mode_ = false;

        uint64_t secure_random_uint64();
    
    public:

        RNG(std::shared_ptr<Settings> settings, std::shared_ptr<file_manager> fm) : settings_(std::move(settings)), file_manager_(std::move(fm)) {}
        void seed(std::optional<uint64_t> seedValue);

        std::string random_word();

        /**
         * @brief Return a uniformly distributed index in [0, upper_bound).
         */
        size_t random_index(size_t upper_bound);

        char select_char(const std::string& charset);

        /**
         * @brief Shuffle characters using the configured random backend.
         */
        void shuffle_chars(std::vector<char>& values);

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
