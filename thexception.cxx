#include "thexception.h"

#include <fmt/core.h>

thexception::thexception(const std::string& msg)
    : std::runtime_error(msg)
{}

thexception::thexception(const std::string& msg, const std::exception& e)
    : thexception(fmt::format("{} -- {}", msg, e.what()))
{}
