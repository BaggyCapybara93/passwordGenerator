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
        const size_t pattern = rng->random_index(5);

        switch (pattern) {
            case 0: {
                // word + 1 random digit (0–9)
                pwd += std::to_string(rng->random_index(10));
                break;
            }
            case 1: {
                // word + predictable suffix from a tiny list
                static const std::vector<std::string> suffixes = {
                    "123", "111", "abc", "aaa", "000"
                };
                pwd += suffixes[rng->random_index(suffixes.size())];
                break;
            }
            case 2: {
                // word + repeated letter (aaa, bbb, ccc...)
                const char letter = static_cast<char>('a' + rng->random_index(26));
                pwd += std::string(3, letter);
                break;
            }
            case 3: {
                // word + low‑entropy number (0–19)
                pwd += std::to_string(rng->random_index(20));
                break;
            }
            case 4: {
                // word + fixed pattern: letter + digit
                pwd += static_cast<char>('a' + rng->random_index(26));
                pwd += static_cast<char>('0' + rng->random_index(10));
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
