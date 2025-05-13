#pragma once

#include <fmt/format.h>

/**
 * @brief Wrapper type for fixed formatting of floating point numbers.
 */
struct thdouble
{
    constexpr thdouble() = default;
    constexpr thdouble(double value, uint32_t precision) : value(value), precision(precision) {}

    double value = 0.0;
    uint32_t precision = 0;
};

/**
 * @brief User-defined literal fot thdouble.
 * @param value value intended for printing
 * @return thdouble with precision set to 2
 */
inline constexpr thdouble operator""_thd(long double value)
{
    return {static_cast<double>(value), 2};
}

namespace fmt
{
/**
 * @brief Extension for {fmt} for printing thdouble.
 * Will print floating point numbers in fixed format
 * and without trailing zeros.
 */
template <>
struct formatter<thdouble>: formatter<std::string> {
    template <typename FormatContext>
    auto format(const thdouble& p, FormatContext& ctx) const {
        // fixed formatting with given precision
        // and (perhaps more) stable cross-platform rounding in the border cases using two-passes
        auto num = fmt::format("{:.{}f}", std::stod(fmt::format("{:.{}f}", p.value, p.precision+1)), p.precision);
        // remove redundant zeros
        const auto dot = num.find('.');
        auto result_len = num.length();
        if (dot != std::string::npos)
        {
            const auto last_not_zero = num.find_last_not_of('0');
            if (last_not_zero != std::string::npos && last_not_zero >= dot)
            {
                result_len = last_not_zero + (last_not_zero != dot);
            }
        }
        num.resize(result_len);
        // Remove const for compatibility with fmtlib < 9.0.0
        return const_cast<formatter<thdouble> *>(this)
            ->formatter<std::string>::format(num, ctx);
    }
};
}
