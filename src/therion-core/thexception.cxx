#include "thexception.h"

#include <format>

thexception::thexception(const std::string& msg)
    : std::runtime_error(msg)
{}

thexception::thexception(const std::string& msg, const std::exception& e)
    : thexception(std::format("{} -- {}", msg, e.what()))
{}
