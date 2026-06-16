#pragma once 
#include <string>
#include "settings.hpp"
#include "random.hpp"

 /**
         * @brief Generate a honey password (intentionally weak, designed to be compromised)
         * @param length The desired length of the honey password
         * @return A weak password that is commonly used and easily guessable
*/
std::string generate_honey_password(std::shared_ptr<RNG> rng, std::shared_ptr<Settings> settings);
