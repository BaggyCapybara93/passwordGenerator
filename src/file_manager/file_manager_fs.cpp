#include "file_manager.hpp"
#include <filesystem>

bool file_manager::delete_file(const std::string& path) {
    return std::filesystem::remove(path);
}

bool file_manager::file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool file_manager::file_validation(const std::string& path) {
    std::filesystem::path file_path(path);
    return std::filesystem::exists(file_path) &&
           std::filesystem::is_regular_file(file_path);
}