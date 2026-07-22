#pragma once

#include <string_view>

/**
 * @brief Case insensitive equality comparator for strings.
 */
struct icase_equal
{
    bool operator()(std::string_view a, std::string_view b) const;
};

/**
 * @brief Case insensitive ordering comparator for strings.
 */
struct icase_less
{
    bool operator()(std::string_view a, std::string_view b) const;
};
