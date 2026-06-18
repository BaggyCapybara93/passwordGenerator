#pragma once
#include <string>
#include <vector>

class file_manager{
    private:
        /**
             * @brief Check if a file exists
             * @param path The path to the file
             * @return True if the file exists, false otherwise
        */
        bool file_exists(const std::string& path);

        /**
             * @brief Check if the file is valid and accessible
             * @param path The path to the file
             * @return True if the file is valid, false otherwise
        */
        bool file_validation(const std::string& path);

        /**
             * @brief Write to a file
             * @param path The path to the file
             * @return True if writing likely happened, false otherwise
        */
        bool write_file(const std::string& path, const std::string& data);

        /**
             * @brief Read a file
             * @param path The path to the file
             * @return The content of the file if successful, an empty string otherwise
        */
        std::string read_file(const std::string& path);

        /**
             * @brief Delete a file
             * @param path The path to the file
             * @return True if process was successful, false otherwise
        */
        bool delete_file(const std::string& path);

    public:
        file_manager() = default;
        
        /**
             * @brief Save passwords to a file
             * @param path The path to the file
             * @param passwords The vector of passwords to save
             * @return True if saving was successful, false otherwise
        */
        bool save_passwords(const std::string& path, const std::vector<std::string>& passwords);
        
        /**
             * @brief Load blacklist from file
             * @param path The path to the file
             * @return The content of the file if successful, an empty string otherwise
        */
        std::string load_blacklist(const std::string& path);
};