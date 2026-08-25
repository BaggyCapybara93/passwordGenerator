#include "parse_arguments.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

bool ParseArguments::parse_args(int argc, char* argv[], Settings& settings) {
    try {
        // Define command line options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "Show help message")
            ("length", po::value<size_t>(&settings.length), "Set desired password length (default: 12)")
            ("no-uppercase", "Disable uppercase requirement")
            ("no-lowercase", "Disable lowercase requirement")
            ("no-digits", "Disable digit requirement")
            ("no-special", "Disable special character requirement")
            ("no-color", "Disable colored output")
            ("num-passwords", po::value<int>(&settings.num_passwords),"Number of passwords to generate (default: 1)")
            ("seed", po::value<uint64_t>(), "Use deterministic seed for random number generation")
            ("custom-chars", po::value<std::string>(&settings.custom_chars), "Custom character pool (e.g., \"abcXYZ123!@#\")")
            ("exclude-chars", po::value<std::string>(&settings.exclude_chars), "Characters to exclude from default pools (e.g., \"!@#$\")")
            ("blacklist", po::value<std::string>(&settings.blacklist), "Comma-separated list of passwords to blacklist (e.g., \"{pass1,pass2,pass3}\")")
            ("blacklist-file", po::value<std::string>(&settings.blacklist_file), "Path to blacklist file (default: blacklist.txt)")
            ("wordlist-file", po::value<std::string>(&settings.wordlist_file), "Path to word list file for honey passwords")
            ("min-entropy", po::value<double>(&settings.min_entropy), "Set minimum entropy threshold in bits (default: 0 means no minimum)")
            ("no-ambiguous", "Exclude ambiguous characters (0/O, 1/l/I)")
            ("honey-password", "Generate a weak password designed to be compromised")
            ("guesses-per-second", po::value<double>(&settings.guesses_per_second), "Set brute-force guesses per second (default: 1e9)")
            ("save-file", po::value<std::string>(&settings.save_file), "Path to save generated passwords (default: saved_passwords.txt)");

        // Parse command line arguments
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);

        // Check if help was requested
        if (vm.count("help")) {
            print_help();
            return false;
        }

        // Handle seed option
        if (vm.count("seed")) {
            settings.seed = vm["seed"].as<uint64_t>();
        }

        // Handle no-color option
        if (vm.count("no-color")) {
            settings.no_color = true;
        }

        if (vm.count("no-ambiguous")) {
            settings.exclude_ambiguous = true;
        }

        if(vm.count(("honey-password"))) {
            settings.is_honeypassword = true;
        }

        // Handle character-type disable flags
        if (vm.count("no-uppercase")) {
            settings.req_uppercase = false;
        }
        if (vm.count("no-lowercase")) {
            settings.req_lowercase = false;
        }
        if (vm.count("no-digits")) {
            settings.req_digits = false;
        }
        if (vm.count("no-special")) {
            settings.req_special = false;
        }

        // Validate settings
        if (!validate_settings(settings)) {
            return false;
        }

        return true;

    } catch (const po::error& e) {
        std::cerr << "Error parsing arguments: " << e.what() << "\n";
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return false;
    }
}

bool ParseArguments::validate_settings(Settings& settings) {
    // Validate custom characters
    if (!settings.custom_chars.empty() && settings.custom_chars.length() < 4) {
        std::cerr << "Error: Custom character pool must contain at least 4 characters.\n";
        return false;
    }

    // Validate basic settings
    if (settings.length < 1) {
        std::cerr << "Error: Password length must be at least 1.\n";
        return false;
    }

    if (settings.num_passwords < 1) {
        std::cerr << "Error: Number of passwords must be at least 1.\n";
        return false;
    }

    if (settings.min_entropy < 0) {
        std::cerr << "Error: Minimum entropy must be non-negative.\n";
        return false;
    }

    if (settings.guesses_per_second <= 0) {
        std::cerr << "Error: Guesses per second must be positive.\n";
        return false;
    }

    // Ensure that users cannot disable all character types
    if(!settings.req_uppercase && !settings.req_lowercase && !settings.req_digits && !settings.req_special && settings.custom_chars.empty()) {
        std::cerr << "Error: At least one character type must be enabled or a custom character pool must be provided.\n";
        return false;
    }

    const auto filter_pool = [&](std::string pool) {
        pool.erase(
            std::remove_if(pool.begin(), pool.end(), [&](char c) {
                return settings.exclude_chars.find(c) != std::string::npos ||
                       (settings.exclude_ambiguous && settings.ambiguous_chars.find(c) != std::string::npos);
            }),
            pool.end());
        return pool;
    };

    // Build and validate each required character pool independently.
    std::string final_pool;
    if (!settings.custom_chars.empty()) {
        final_pool = filter_pool(settings.custom_chars);
    } else {
        const std::string uppercase_pool = filter_pool(settings.uppercase_string);
        const std::string lowercase_pool = filter_pool(settings.lowercase_string);
        const std::string digits_pool = filter_pool(settings.digits_string);
        const std::string special_pool = filter_pool(settings.special_string);

        const auto add_required_pool = [&](bool required, const std::string& pool, const char* name) {
            if (!required) {
                return true;
            }
            if (pool.empty()) {
                std::cerr << "Error: Character exclusions removed every " << name << " character.\n";
                return false;
            }
            final_pool += pool;
            return true;
        };

        if (!add_required_pool(settings.req_uppercase, uppercase_pool, "uppercase") ||
            !add_required_pool(settings.req_lowercase, lowercase_pool, "lowercase") ||
            !add_required_pool(settings.req_digits, digits_pool, "digit") ||
            !add_required_pool(settings.req_special, special_pool, "special")) {
            return false;
        }
    }

    if (final_pool.empty()) {
        std::cerr << "Error: No characters available for password generation.\n";
        return false;
    }

    // Validate entropy constraints
    double max_entropy = static_cast<double>(settings.length) * std::log2(static_cast<double>(final_pool.size()));

    if (settings.min_entropy > 0 && settings.min_entropy > 1024) {
        std::cerr << "Error: Minimum entropy must be between 0 and 1024 bits.\n";
        return false;
    }
    
    if (settings.min_entropy > max_entropy) {
        std::cerr << "Error: Requested entropy (" << settings.min_entropy
                << " bits) exceeds maximum possible entropy (" << max_entropy
                << " bits) for the selected character set.\n";
        return false;
    }

    return true;
}

void ParseArguments::print_help() {
    std::cout << "Usage: " << std::string(program_name) << " [options]\n\n"
    << "Generate random passwords with customizable options.\n\n"
    << "Options:\n"
    << "  --length N              Set desired password length (default: 12)\n"
    << "  --no-uppercase          Disable uppercase requirement\n"
    << "  --no-lowercase          Disable lowercase requirement\n"
    << "  --no-digits             Disable digit requirement\n"
    << "  --no-color              Disable colored output\n"
    << "  --no-special            Disable special character requirement\n"
    << "  --num-passwords N       Number of passwords to generate (default: 1)\n"
    << "  --seed N                Use deterministic seed for random generation\n"
    << "  --blacklist S           Comma-separated list of passwords to blacklist (e.g., \"{pass1,pass2,pass3}\")\n"
    << "  --blacklist-file F      Path to blacklist file (default: blacklist.txt)\n"
    << "  --wordlist-file F       Path to word list file for honey passwords\n"
    << "  --min-entropy N         Set minimum entropy threshold in bits (default: 0 means no minimum)\n"
    << "  --no-ambiguous          Exclude ambiguous characters (0/O, 1/l/I)\n"
    << "  --honey-password        Generate a weak password designed to be compromised\n"
    << "  --guesses-per-second N  Set brute-force guesses per second (default: 1e9)\n"
    << "  --save-file F           Path to save generated passwords (default: saved_passwords.txt)\n"
    << "  --help, -h              Show this help message and exit\n\n"
    << "Example:\n"
    << "  " << program_name << " --length 32 --no-special --num-passwords 5\n"
    << "  " << program_name << " --length 16 --custom-chars \"abcXYZ123!@#\"\n"
    << "  " << program_name << " --length 16 --exclude-chars \"!@#$\"\n"
    << "  " << program_name << " --length 12 --blacklist \"{weak123,default,password}\"\n"
    << "  " << program_name << " --length 16 --no-ambiguous\n"
    << "  " << program_name << " --guesses-per-second 1e8\n";
}
