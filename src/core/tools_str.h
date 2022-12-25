#pragma once

#include <string>
#include <algorithm>

namespace tools_str
{
    // Auxiliary general values
    namespace keys
    {
        const static std::string bars = "\\/";
        const static std::string comma = "\"";
    }

    std::string clean_bars(const std::string& str2clean);

};
