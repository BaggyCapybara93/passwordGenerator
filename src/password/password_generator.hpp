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

        std::shared_ptr<std::unordered_set<std::string>> blacklist_;

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