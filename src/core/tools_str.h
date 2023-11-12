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
        const static std::string bracket_left = "[";
        const static std::string bracket_right = "]";
    }

    std::string clean_str(const std::string& str2clean, const std::string & str2remove = "");

    std::string clean_bars(const std::string& str2clean, const std::string & str2preserve = "");

    template<typename T>
    std::vector<T> str2vector(const std::string &str2vector, const std::string &delimiter = ",");
};

#include "tools_str_temp.cpp"