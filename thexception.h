/**
 * @file thexception.h
 * String exception class.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */

#pragma once

#include <stdexcept>
#include <fmt/core.h>

/**
 * Therion exception class.
 */
class thexception : public std::runtime_error {
public:
    explicit thexception(const std::string& msg)
        : std::runtime_error(msg)
    {}

    thexception(const std::string& msg, const std::exception& e)
        : thexception(fmt::format("{} -- {}", msg, e.what()))
    {}
};

// These definitions are here only so the build does not fail and will be removed in the final version.
template <typename... Args>
[[noreturn]] void ththrow(const Args&...)
{
    throw std::runtime_error("ththrow");
}
