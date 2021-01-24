#pragma once

#include <fmt/core.h> // in C++17 replace with <string_view>
#include <algorithm>

/**
 * @brief Case insensitive comparison of two strings.
 */ 
inline bool icase_equals(fmt::string_view a, fmt::string_view b)
{
     // we need to convert chars to unsigned chars because of std::tolower's requirements
    using uchar_type = const unsigned char;
    return std::equal(a.begin(), a.end(), 
                      b.begin(), b.end(),
                      [](uchar_type ca, uchar_type cb){ return std::tolower(ca) == std::tolower(cb); });
}
