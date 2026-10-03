#include "random.hpp"
#include "ui/ui.hpp"
#include "parser/parse_arguments.hpp"
#include "settings.hpp"
#include "password/password.hpp"
#include <iostream>
#include <memory>
#include <exception>


int main(int argc, char* arg[]){
    
    Settings settings;
    ParseArguments parser;

    const ParseResult parse_result = parser.parse_args(argc, arg, settings);
    if (parse_result == ParseResult::help) return 0;
    if (parse_result == ParseResult::error) return 2;

    // Create and initialize the password generator
    auto settings_ptr = std::make_shared<Settings>(settings);
    auto file_manager_ptr = std::make_shared<file_manager>();
    auto rng_ptr = std::make_shared<RNG>(settings_ptr, file_manager_ptr);
    Password_Generator password_generator(settings_ptr, rng_ptr);
    try {
        password_generator.initialize();
    } catch (const std::exception&) {
        return 1;
    }

    return 0;
}
