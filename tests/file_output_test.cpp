#include "file_manager/file_manager.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>

#ifndef _WIN32
#include <sys/stat.h>
#endif

namespace {
namespace fs = std::filesystem;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

struct TemporaryDirectory {
    fs::path path;

    TemporaryDirectory() {
        std::random_device random;
        for (int attempt = 0; attempt < 20; ++attempt) {
            path = fs::temp_directory_path() /
                   ("password-generator-test-" + std::to_string(random()));
            if (fs::create_directory(path)) return;
        }
        throw std::runtime_error("Could not create a temporary test directory.");
    }

    ~TemporaryDirectory() {
        std::error_code error;
        fs::remove_all(path, error);
    }
};

std::string read_all(const fs::path& path) {
    std::ifstream input(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}
} // namespace

int main() {
    TemporaryDirectory temporary;
    file_manager manager;
    const fs::path output = temporary.path / "passwords.txt";

#ifndef _WIN32
    const mode_t previous_mask = umask(0);
#endif
    const bool saved = manager.save_passwords(output.string(), {"first", "second"});
#ifndef _WIN32
    umask(previous_mask);
#endif
    require(saved, "Initial save failed.");
    require(read_all(output) == "first\nsecond\n", "Saved contents differ.");

#ifndef _WIN32
    struct stat details {};
    require(stat(output.c_str(), &details) == 0, "Could not inspect file mode.");
    require((details.st_mode & 0777) == 0600, "Password file is not owner-only.");
#endif

    bool refused_existing = false;
    try {
        manager.save_passwords(output.string(), {"replacement"});
    } catch (const std::runtime_error&) {
        refused_existing = true;
    }
    require(refused_existing, "Existing file was accepted.");
    require(read_all(output) == "first\nsecond\n", "Existing file was changed.");

#ifndef _WIN32
    const fs::path link = temporary.path / "link.txt";
    fs::create_symlink(output, link);
    bool refused_link = false;
    try {
        manager.save_passwords(link.string(), {"replacement"});
    } catch (const std::runtime_error&) {
        refused_link = true;
    }
    require(refused_link, "Symbolic link was accepted.");
    require(read_all(output) == "first\nsecond\n", "Symbolic link target was changed.");
#endif
}
