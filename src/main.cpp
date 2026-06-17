#include "random.hpp"
#include "ui/ui.hpp"
#include "parse_arguments.hpp"
#include "settings.hpp"
#include "password/password_generator.hpp"
#include <iostream>
#include <memory>


int main(int argc, char* arg[]){
    // Set program name for help output
    std::string program_name = arg[0];
    
    Settings settings;

    if (!parse_arguments::parse_args(argc, arg, settings)) {
        return 0; // Exit if argument parsing failed or help was requested 
    }

    // Create and initialize the password generator
    auto settings_ptr = std::make_shared<Settings>(settings);
    auto rng_ptr = std::make_shared<RNG>(settings_ptr);
    Password_Generator password_generator(settings_ptr, rng_ptr);
    password_generator.initialize();

    return 0;
}