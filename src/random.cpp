#include "random.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

static const std::string uppercaseString = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const std::string lowercaseString = "abcdefghijklmnopqrstuvwxyz";
static const std::string digitsString = "0123456789";
static const std::string specialString = "!@#$%^&*()_+~`|}{[]:;?><,./-=";

std::mt19937_64 RNG::engine_;
std::random_device RNG::device_;

void RNG::seed(){
    std::cout << "Generating seed for number generator...";
    engine_.seed(device_());
    std::cout << "Completed." << std::endl;
}

char RNG::selectChar(const std::vector<char>& charset){
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

std::string RNG::generate(size_t length, bool requiresUppercase, bool requiresLowercase, bool requiresDigits, bool requiresSpecial){
    std::vector<char> result;
    
    // Define pools
    const std::string& up = uppercaseString;
    const std::string& low = lowercaseString;
    const std::string& dig = digitsString;
    const std::string& sp = specialString;

    auto ensureChar = [&](const std::string& s, bool req) {
        if (req && !s.empty()) {
            result.push_back(selectChar(std::vector<char>(s.begin(), s.end())));
        }
    };

    ensureChar(up, requiresUppercase);
    ensureChar(low, requiresLowercase);
    ensureChar(dig, requiresDigits);
    ensureChar(sp, requiresSpecial);

    size_t current_size = result.size();
    size_t remaining = length - current_size;
    
    if(remaining < 0){
        std::string msg = "Requested password length (" + std::to_string(length) + 
                          ") is too short for requirements (" + 
                          std::to_string(current_size) + " minimum).";
        throw std::invalid_argument(msg);
    }

    std::string allPool = "";
    if(requiresUppercase) allPool += up;
    if(requiresLowercase) allPool += low;
    if(requiresDigits) allPool += dig;
    if(requiresSpecial) allPool += sp;

    if(allPool.empty()) {
        throw std::invalid_argument("No character set defined to generate remaining characters!");
    }

    while(static_cast<size_t>(result.size()) < length && remaining > 0){
        result.push_back(selectChar(std::vector<char>(allPool.begin(), allPool.end())));
        --remaining;
    }

   
    std::shuffle(result.begin(), result.end(), engine_);

    return std::string(result.begin(), result.end());
}