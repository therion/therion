#include "icase.h"

#include <algorithm>
#include <cctype>
#include <ranges>

static auto lower_range(std::string_view s)
{
     // we need to convert chars to unsigned chars because of std::tolower's requirements
     return std::views::transform(s, [](const unsigned char c){ return std::tolower(c); });
}

bool icase_equal::operator()(std::string_view a, std::string_view b) const
{
     return std::ranges::equal(lower_range(a), lower_range(b));
}

bool icase_less::operator()(std::string_view a, std::string_view b) const
{
     return std::ranges::lexicographical_compare(lower_range(a), lower_range(b));
}
