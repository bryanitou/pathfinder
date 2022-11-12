/**
 * Pre-processor, functions that will be in charge of reading the file
 */

// System libraries
#include <vector>
#include <map>
#include <string>
#include <filesystem>

namespace duavc_input_file
{
    std::map<std::string, std::vector<double>> read_input(std::filesystem::path input_path);
};
