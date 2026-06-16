#pragma once

#include <unordered_set>
#include <iostream>
#include <sstream>

std::unordered_set<std::string> parse_blacklist(const std::string& blacklist_str);
