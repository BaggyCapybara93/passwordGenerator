#include "file_manager.hpp"
#include <fstream>

bool file_manager::write_file(const std::string& path, const std::string& data) {
    try {
        std::ofstream out(path);
        if (!file_validation(path)) return false;
        if (!out) return false;
        out << data;
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error writing file: " + std::string(e.what()));
    }
}

std::string file_manager::read_file(const std::string& path) {
    try {
        std::ifstream in(path);
        if (!file_validation(path)) return "";
        if (!in) return "";
        return std::string((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
    } catch (const std::exception& e) {
        throw std::runtime_error("Error reading file: " + std::string(e.what()));
    }
}

bool file_manager::save_passwords(const std::string& path, const std::vector<std::string>& passwords) {
    try {
        if (passwords.empty()) return false;
        
        std::ofstream out(path);
        if (!file_validation(path)) return false;
        if (!out) return false;
        
        // Write each password on a new line
        for (const auto& password : passwords) {
            out << password << "\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error saving passwords: " + std::string(e.what()));
    }
}

std::string file_manager::load_blacklist(const std::string& path) {
    try {
        std::ifstream in(path);
        if (!file_validation(path)) return "";
        if (!in) return "";
        
        return std::string((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());

        if (!in) return "";
        
        std::string content((std::istreambuf_iterator<char>(in)),
                            std::istreambuf_iterator<char>());
        return content;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading blacklist: " + std::string(e.what()));
    }
}