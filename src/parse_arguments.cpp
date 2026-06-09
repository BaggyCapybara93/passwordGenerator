#include "parse_arguments.hpp"
#include <stdexcept>
#include <string>

namespace parse_arguments {

    std::string program_name;

    void set_program_name(const std::string& name) {
        program_name = name;
    }

    bool parse_args(int argc, char* argv[], Settings& settings) {
        try {
            // Define command line options
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help", "Show help message")
                ("length", po::value<size_t>(&settings.desired_length),
                 "Set desired password length (default: 12)")
                ("no-uppercase", "Disable uppercase requirement")
                ("no-lowercase", "Disable lowercase requirement")
                ("no-digits", "Disable digit requirement")
                ("no-special", "Disable special character requirement")
                ("no-color", "Disable colored output")
                ("num-passwords", po::value<int>(&settings.num_passwords),
                 "Number of passwords to generate (default: 1)")
                ("seed", po::value<uint64_t>(),
                 "Use deterministic seed for random number generation")
                ("custom-chars", po::value<std::string>(&settings.custom_chars),
                 "Custom character pool (e.g., \"abcXYZ123!@#\")")
                ("exclude-chars", po::value<std::string>(&settings.exclude_chars),
                 "Characters to exclude from default pools (e.g., \"!@#$\")")
                ;

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

            // Validate custom characters
            if (!settings.custom_chars.empty()) {
                if (settings.custom_chars.length() < 4) {
                    std::cerr << "Error: Custom character pool must contain at least 4 characters.\n";
                    return false;
                }
            }

            // Validate exclude characters
            if (!settings.exclude_chars.empty()) {
                if (settings.exclude_chars.length() < 1) {
                    std::cerr << "Error: Exclude characters must contain at least 1 character.\n";
                    return false;
                }
            }

            // Validate settings
            if (settings.desired_length < 1) {
                std::cerr << "Error: Password length must be at least 1.\n";
                return false;
            }

            if (settings.num_passwords < 1) {
                std::cerr << "Error: Number of passwords must be at least 1.\n";
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

    void print_help() {
        std::cout << "Usage: " << std::string(program_name) << " [options]\n\n";
        std::cout << "Generate random passwords with customizable options.\n\n";
        std::cout << "Options:\n";
        std::cout << "  --length N              Set desired password length (default: 12)\n";
        std::cout << "  --no-uppercase          Disable uppercase requirement\n";
        std::cout << "  --no-lowercase          Disable lowercase requirement\n";
        std::cout << "  --no-digits             Disable digit requirement\n";
        std::cout << "  --no-color              Disable colored output\n";
        std::cout << "  --no-special            Disable special character requirement\n";
        std::cout << "  --num-passwords N       Number of passwords to generate (default: 1)\n";
        std::cout << "  --seed N                Use deterministic seed for random generation\n";
        std::cout << "  --help, -h              Show this help message and exit\n\n";
        std::cout << "Example:\n";
        std::cout << "  " << program_name << " --length 32 --no-special --num-passwords 5\n";
        std::cout << "  " << program_name << " --length 16 --custom-chars \"abcXYZ123!@#\"\n";
        std::cout << "  " << program_name << " --length 16 --exclude-chars \"!@#$\"\n";
    }

} // namespace parse_arguments
