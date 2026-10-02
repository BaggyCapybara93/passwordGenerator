#include "file_manager.hpp"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#include <memory>
#else
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace {
#ifdef _WIN32
void write_private_file(const std::string& path, const std::string& data) {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        throw std::runtime_error("Could not get the current user's security token.");
    }

    DWORD token_size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &token_size);
    auto token_data = std::make_unique<BYTE[]>(token_size);
    const BOOL got_user = GetTokenInformation(token, TokenUser, token_data.get(), token_size, &token_size);
    CloseHandle(token);
    if (!got_user) {
        throw std::runtime_error("Could not get the current user's security identifier.");
    }

    const auto* user = reinterpret_cast<const TOKEN_USER*>(token_data.get());
    const DWORD acl_size = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) +
                           GetLengthSid(user->User.Sid);
    auto acl_data = std::make_unique<BYTE[]>(acl_size);
    auto* acl = reinterpret_cast<ACL*>(acl_data.get());
    SECURITY_DESCRIPTOR descriptor;
    if (!InitializeAcl(acl, acl_size, ACL_REVISION) ||
        !AddAccessAllowedAce(acl, ACL_REVISION, FILE_ALL_ACCESS, user->User.Sid) ||
        !InitializeSecurityDescriptor(&descriptor, SECURITY_DESCRIPTOR_REVISION) ||
        !SetSecurityDescriptorDacl(&descriptor, TRUE, acl, FALSE) ||
        !SetSecurityDescriptorControl(&descriptor, SE_DACL_PROTECTED, SE_DACL_PROTECTED)) {
        throw std::runtime_error("Could not set private permissions for the password file.");
    }

    SECURITY_ATTRIBUTES attributes{sizeof(attributes), &descriptor, FALSE};
    HANDLE file = CreateFileA(path.c_str(), GENERIC_WRITE, 0, &attributes, CREATE_NEW,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        if (error == ERROR_FILE_EXISTS || error == ERROR_ALREADY_EXISTS) {
            throw std::runtime_error("Password file already exists: " + path);
        }
        throw std::runtime_error("Could not create private password file (Windows error " +
                                 std::to_string(error) + "): " + path);
    }

    size_t offset = 0;
    while (offset < data.size()) {
        const DWORD requested = static_cast<DWORD>(std::min<size_t>(data.size() - offset, 1U << 20U));
        DWORD written = 0;
        if (!WriteFile(file, data.data() + offset, requested, &written, nullptr) || written == 0) {
            CloseHandle(file);
            throw std::runtime_error("Could not write the password file: " + path);
        }
        offset += written;
    }
    if (!CloseHandle(file)) {
        throw std::runtime_error("Could not close the password file: " + path);
    }
}
#else
void write_private_file(const std::string& path, const std::string& data) {
    int flags = O_WRONLY | O_CREAT | O_EXCL;
#ifdef O_NOFOLLOW
    flags |= O_NOFOLLOW;
#endif
#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
    const int file = open(path.c_str(), flags, S_IRUSR | S_IWUSR);
    if (file < 0) {
        if (errno == EEXIST) {
            throw std::runtime_error("Password file already exists: " + path);
        }
        throw std::runtime_error("Could not create private password file: " + path +
                                 " (" + std::strerror(errno) + ")");
    }

    size_t offset = 0;
    while (offset < data.size()) {
        const size_t requested = std::min<size_t>(data.size() - offset, 1U << 20U);
        const ssize_t written = write(file, data.data() + offset, requested);
        if (written < 0 && errno == EINTR) continue;
        if (written <= 0) {
            const int error = errno;
            close(file);
            throw std::runtime_error("Could not write the password file: " + path +
                                     " (" + std::strerror(error) + ")");
        }
        offset += static_cast<size_t>(written);
    }
    if (close(file) != 0) {
        throw std::runtime_error("Could not close the password file: " + path +
                                 " (" + std::strerror(errno) + ")");
    }
}
#endif
} // namespace

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

        std::string content;
        for (const auto& password : passwords) {
            content += password + "\n";
        }
        write_private_file(path, content);
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error saving passwords: " + std::string(e.what()));
    }
}

std::vector<std::string> file_manager::load_lines(const std::string& path) {
    try {
        std::vector<std::string> lines;
        std::ifstream in(path);
        if (!file_validation(path)) return lines;
        if (!in) return lines;

        std::string line;
        while (std::getline(in, line)) {
            lines.push_back(line);
        }

        return lines;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading file lines: " + std::string(e.what()));
    }
}

std::string file_manager::load_blacklist(const std::string& path) {
    try {
        std::vector<std::string> lines = load_lines(path);
        std::string result;
        for (const auto& line : lines) {
            if (!result.empty()) {
                result += '\n';
            }
            result += line;
        }
        return result;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading blacklist: " + std::string(e.what()));
    }
}
