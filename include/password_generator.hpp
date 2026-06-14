#pragma once
#include <mutex>
#include <optional>
#include <unordered_set>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#include <random.hpp>
#include <settings.hpp>

class Password_Generator{
    private:
        std::shared_ptr<Settings> settings_;
        std::shared_ptr<RNG> rng_; 

        /**
         * @brief Generate a password
         * @return A string of the password generated
         */
        std::string generate_password();

        /**
         * @brief Generate multiple passwords and display them
         * @param num_passwords Number of passwords to generate
         */
        void generate_passwords(int num_passwords);

        /**
         * @brief Parse the blacklist 
         * @param blacklist_str The string of the blacklist
         * @return Unordered set of blacklisted strings
         */
        std::unordered_set<std::string> parse_blacklist(const std::string& blacklist_str);

        /**
         * @brief Generate a honey password (intentionally weak, designed to be compromised)
         * @param length The desired length of the honey password
         * @return A weak password that is commonly used and easily guessable
         */
        std::string generate_honey_password();

        /**
         * @brief Calculate the entropy of a password in bits
         * @param password The password to calculate entropy for
         * @return Entropy in bits (higher = more secure)
         */
        double calculate_entropy(const std::string& password);

        /**
         * @brief Calculate the security score of a password
         * @param entropy The entropy value to calculate security score for
         * @return Security score as a string(Weak, Moderate, Strong)
         */
        std::string calculate_security_score(const double& entropy);

        /**
         * @brief Generate and display a single password
         * @param password The password to display
         */
        void display_password(const std::string& password);

    public:
        Password_Generator(std::shared_ptr<Settings> settings, std::shared_ptr<RNG> rng)
            : settings_(settings), rng_(rng) {}

        /**
         * @brief Starts the password_generator
         */
        void initialize();
};