#pragma once

#include <cstdio>
#include <memory>
#include <string>

/**
 * @brief Deleter type for std::unique_ptr.
 */
struct close_file
{
    void operator()(FILE* f) const
    {
        std::fclose(f);
    }
};

/**
 * @brief RAII handle for C-style files.
 */
using file_handle = std::unique_ptr<FILE, close_file>;

/**
 * @brief Open a file using C library function, but return it in a wrapper which
 * closes the file in its destructor.
 * @param name file name
 * @param mode file mode
 * @return FILE* wrapped in a unique pointer
 */
inline auto thopen_file(const std::string& name, const std::string& mode)
{
    return file_handle(std::fopen(name.c_str(), mode.c_str()));
}
