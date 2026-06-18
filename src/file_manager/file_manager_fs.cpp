#include "file_manager.hpp"
#include <filesystem>

bool file_manager::delete_file(const std::string& path) {
    return std::filesystem::remove(path);
}

bool file_manager::file_exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool file_manager::file_validation(const std::string& path) {
    if (!file_exists(path)) return false;

    if (!std::filesystem::is_regular_file(path)) return false;

    return true;
}