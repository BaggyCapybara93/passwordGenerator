#pragma once
#include <string>
#include "settings.hpp"

/**
 * @brief Build the distinct characters available to the password generator.
 */
std::string build_effective_character_pool(const Settings& settings);

/**
         * @brief Calculate the entropy of a password in bits
         * @param password The password to calculate entropy for
         * @return Entropy in bits (higher = more secure)
*/
double calculate_entropy(const std::string& password, const Settings& settings);

/**
         * @brief Calculate the security score of a password
         * @param entropy The entropy value to calculate security score for
         * @return Security score as a string(Weak, Moderate, Strong)
*/
std::string calculate_security_score(double entropy, const Settings& settings);
