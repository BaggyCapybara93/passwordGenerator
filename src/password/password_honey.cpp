#include "password_honey.hpp"

//This isnt great and probably should be removed, in favor of a more robust honey password generation method
//Or changed to be weaker
std::string generate_honey_password(std::shared_ptr<RNG> rng, std::shared_ptr<Settings> settings) {
    try{
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
                // word + 1 random digit (0–9)
                std::uniform_int_distribution<int> d(0, 9);
                pwd += std::to_string(d(rng->engine_));
                break;
            }
            case 1: {
                // word + predictable suffix from a tiny list
                static const std::vector<std::string> suffixes = {
                    "123", "111", "abc", "aaa", "000"
                };
                std::uniform_int_distribution<int> s(0, static_cast<int>(suffixes.size() - 1));
                pwd += suffixes[static_cast<size_t>(s(rng->engine_))];
                break;
            }
            case 2: {
                // word + repeated letter (aaa, bbb, ccc...)
                std::uniform_int_distribution<int> l(0, 25);
                char letter = 'a' + static_cast<char>(l(rng->engine_));
                pwd += std::string(3, letter);
                break;
            }
            case 3: {
                // word + low‑entropy number (0–19)
                std::uniform_int_distribution<int> n(0, 19);
                pwd += std::to_string(n(rng->engine_));
                break;
            }
            case 4: {
                // word + fixed pattern: letter + digit
                std::uniform_int_distribution<int> l(0, 25);
                std::uniform_int_distribution<int> d(0, 9);
                pwd += static_cast<char>('a' + l(rng->engine_));
                pwd += static_cast<char>('0' + d(rng->engine_));
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
    } catch (const std::exception& e) {
        throw std::runtime_error("Error generating honey password: " + std::string(e.what()));
    }
}