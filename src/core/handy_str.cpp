//
// Created by bryan on 11/12/22.
//

#include "handy_str.h"

std::string handy_str::clean_bars(const std::string& str2clean)
{
    // Copy new string
    std::string str2return = str2clean;

    // Chars we want to remove:
    std::string chars2remove = handy_str::keys::bars + handy_str::keys::comma;

    // Remove characters
    for(char char2remove : chars2remove)
    {
        // Position to erase
        auto it = std::remove(str2return.begin(), str2return.end(), char2remove);

        // Erase it from the group
        str2return.erase(it, str2return.end());
    }

    // Return the string
    return str2return;
}