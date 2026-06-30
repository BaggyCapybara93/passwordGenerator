#include "password_blacklist.hpp"

std::unordered_set<std::string> parse_blacklist(const std::string& blacklist_str) {
    try{
        std::unordered_set<std::string> blacklist;
        
        if (blacklist_str.empty() || blacklist_str.front() != '{' || blacklist_str.back() != '}') {
            return blacklist;
        }
                
        // Remove braces and split by comma
        std::string content = blacklist_str.substr(1, blacklist_str.length() - 2);
        std::stringstream ss(content);
        std::string entry;
        
        while (std::getline(ss, entry, ',')) {
            if (!entry.empty()) {
                blacklist.insert(entry);
            }
        }
        
        return blacklist;
    }catch(const std::exception& e) {
        throw std::runtime_error("Error parsing blacklist: " + std::string(e.what()));
    }
}