#include "file_manager.hpp"
#include <fstream>

bool file_manager::write_file(const std::string& path, const std::string& data) {
    std::ofstream out(path);
    if (!file_validation(path)) return false;
    if (!out) return false;
    out << data;
    return true;
}

std::string file_manager::read_file(const std::string& path) {
    std::ifstream in(path);
    if (!file_validation(path)) return "";
    if (!in) return "";
    return std::string((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
}

bool file_manager::save_passwords(const std::string& path, const std::vector<std::string>& passwords) {
    if (passwords.empty()) return false;
    
    std::ofstream out(path);
    if (!file_validation(path)) return false;
    if (!out) return false;
    
    // Write each password on a new line
    for (const auto& password : passwords) {
        out << password << "\n";
    }
    
    return true;
}

std::string file_manager::load_blacklist(const std::string& path) {
    std::ifstream in(path);
    if (!file_validation(path)) return "";
    if (!in) return "";
    
    std::string content((std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
    return content;
}