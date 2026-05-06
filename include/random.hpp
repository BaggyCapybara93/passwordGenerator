#pragma once
#include <string>
#include <vector>
#include <random>

class RNG{
    private:
        static std::mt19937_64 engine_;
        static std::random_device device_;
    
    public:
        static void seed(std::optional<uint64_t> seedValue);

        static std::string generate(
            size_t length,
            bool requiresUppercase = true,
            bool requiresLowercase = true,
            bool requiresDigits = true,
            bool requiresSpecial = true
        );

        static char selectChar(const std::vector<char>& charset);

};