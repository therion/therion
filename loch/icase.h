#pragma once

// Standard libraries
#ifndef LXDEPCHECK
#include <string_view>
#endif  
//LXDEPCHECK - standard libraries

/**
 * @brief Case insensitive comparison of two strings.
 */ 
bool icase_equals(std::string_view a, std::string_view b);

/**
 * @brief Case insensitive ordering of two strings.
 */ 
bool icase_less_than(std::string_view a, std::string_view b);
