template<typename T>
std::vector<T> tools_str::str2vector(const std::string& str2vector, const std::string& delimiter)
{
    // Auxiliary variables
    size_t pos = 0;
    std::string token;
    T token_type;

    // Output to be returned
    std::vector<T> vector2return{};

    // Clean brackets
    auto s = tools_str::clean_str(str2vector, tools_str::keys::bracket_left + tools_str::keys::bracket_right);

    // Just for the loop to work, add a delimiter (i.e., ",") at the end
    s += delimiter;

    // Iterate until no more positions
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        // Get the token
        token = s.substr(0, pos);

        // Try to convert to the desired type
        if constexpr (std::is_integral_v<T> or std::is_floating_point_v<T>)
        {
            token_type = std::stod(token);

        }
        else
        {
            token_type = (T) token;
        }

        // Erase from the string
        s.erase(0, pos + delimiter.length());

        // Push back the element
        vector2return.push_back(token_type);
    }

    // Return the string
    return vector2return;
}