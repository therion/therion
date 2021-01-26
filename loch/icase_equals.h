#pragma once

#include <string> // in C++17 replace with <string_view>
#include <algorithm>

/**
 * @brief Case insensitive comparison of two strings.
 */ 
inline bool icase_equals(const std::string& a, const std::string& b)
{
     // we need to convert chars to unsigned chars because of std::tolower's requirements
    using uchar_type = const unsigned char;
    return std::equal(a.begin(), a.end(), 
                      b.begin(), b.end(),
                      [](uchar_type ca, uchar_type cb){ return std::tolower(ca) == std::tolower(cb); });
}
