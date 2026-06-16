#include "password_honey.hpp"

std::string generate_honey_password(std::shared_ptr<RNG> rng, std::shared_ptr<Settings> settings) {
    std::string word = rng->random_word();
    std::string pwd = word;

    // If the word is already too long, truncate it
    if (pwd.size() > settings->length) {
        pwd = pwd.substr(0, settings->length);
        return pwd;
    }

    // Choose a weak pattern - still random but with more predictable suffixes
    std::uniform_int_distribution<int> pattern_dist(0, 4);
    int pattern = pattern_dist(rng->engine_);

    switch (pattern) {
        case 0: {
            // word + random 3-digit number (weak: 000-999)
            std::uniform_int_distribution<int> num_dist(0, 999);
            pwd += std::to_string(num_dist(rng->engine_));
            break;
        }
        case 1: {
            // word + random 4-digit number (weak: 0000-9999)
            std::uniform_int_distribution<int> num_dist2(0, 9999);
            pwd += std::to_string(num_dist2(rng->engine_));
            break;
        }
        case 2: {
            // word + random 2-digit number (weak: 00-99)
            std::uniform_int_distribution<int> num_dist3(0, 99);
            pwd += std::to_string(num_dist3(rng->engine_));
            break;
        }
        case 3: {
            // word + random repeated digit (weak: 00, 11, 22, ..., 99)
            std::uniform_int_distribution<int> d(0, 9);
            char digit = '0' + static_cast<char>(d(rng->engine_));
            pwd += std::string(2, digit);
            break;
        }
        case 4: {
            // word + random lowercase letter repeated (weak: aa, bb, cc, ..., zz)
            std::uniform_int_distribution<int> l(0, 25);
            char letter = 'a' + static_cast<char>(l(rng->engine_));
            pwd += std::string(2, letter);
            break;
        }
    }

    // Trim or pad to requested length
    if (pwd.size() > settings->length) {
        pwd = pwd.substr(0, settings->length);
    } else {
        // pad with lowercase letters (weak)
        while (pwd.size() < settings->length) {
            pwd += rng->select_char(settings->lowercase_string);
        }
    }

    return pwd;
}