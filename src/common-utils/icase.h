#pragma once

#include <string_view>

/**
 * @brief Case insensitive comparison of two strings.
 */ 
bool icase_equals(std::string_view a, std::string_view b);

/**
 * @brief Case insensitive ordering of two strings.
 */ 
bool icase_less_than(std::string_view a, std::string_view b);
