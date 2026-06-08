#pragma once
#include <mutex>
#include <optional>
#include <string>
#include <vector>
#include <random>

class RNG{
    private:
        static std::mt19937_64 engine_;
        static std::random_device device_;
        static std::mutex engine_mutex_;
    
    public:
        static void seed(std::optional<uint64_t> seedValue);

        static std::string generate(
            size_t length,
            bool requires_uppercase = true,
            bool requires_lowercase = true,
            bool requires_digits = true,
            bool requires_special = true
        );

        static char select_char(const std::string& charset);

};