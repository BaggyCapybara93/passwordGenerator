#pragma once
#include <string>
#include <unordered_set>
#include "settings.hpp"

/**
 * @brief Build the distinct characters available to the password generator.
 */
std::string build_effective_character_pool(const Settings& settings);

/** Calculate the entropy of the uniformly sampled valid output set. */
double calculate_generation_entropy(const Settings& settings,
                                    const std::unordered_set<std::string>& blacklist = {});

/** Check whether a candidate contains every enabled character group. */
bool password_meets_character_requirements(const std::string& password, const Settings& settings);

/** Return whether settings allow at least one valid password. */
bool has_valid_output_space(const Settings& settings,
                            const std::unordered_set<std::string>& blacklist = {});

/**
         * @brief Calculate the security score of a password
         * @param entropy The entropy value to calculate security score for
         * @return Security score as a string(Weak, Moderate, Strong)
*/
std::string calculate_security_score(double entropy, const Settings& settings);
