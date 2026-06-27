#pragma once

#include <unordered_set>
#include <iostream>
#include <sstream>

 /**
         * @brief Parse a blacklist string into a set of strings
         * @param blacklist_str The string containing the blacklist items
         * @return A set of strings representing the blacklisted items
*/
std::unordered_set<std::string> parse_blacklist(const std::string& blacklist_str);
