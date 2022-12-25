/**
 * Main used for the DUAVC project.
 */

// Boost libraries
#include <boost/program_options.hpp>

// System libraries
#include <iostream>

// Project libraries
#include "Ampli_DubinsStateSpace.h"

// JSON Parser interface
#include "json_parser.h"

// Namespaces
namespace po = boost::program_options;
namespace ob = ompl::base;

/**
 * Main entry point
 */
int main(int argc, char* argv[])
{
    // Auxiliary constant variables
    std::string input_file;

    try {
        po::options_description desc("Options");
        desc.add_options()
                ("help", "show help message")
                ("start",po::value<std::vector<double > >()->multitoken(),
                        "use Dubins state space")
                ("goal",po::value<std::vector<double > >()->multitoken(),
                 "use Dubins state space")
                ("file", po::value<std::string>(&input_file)->default_value("./cfg/example.json"),
                        "in the house formatted .dat file.")
                ("output", "where to dump the output files (folder). File names are hard set.")
                ;

        // Create a variable map from the command entry lines
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc,
                                         po::command_line_style::unix_style ^ po::command_line_style::allow_short), vm);
        po::notify(vm);

        // Check inputs
        if ((vm.count("help") != 0u) || argc==1)
        {
            std::cout << desc << "\n";
            return 1;
        }

        // Hard set the output files
        std::string trajectory_txt_path = "./out/trajectory.txt";
        std::string plan_txt_path = "./out/plan.txt";
        std::string vertexes_vrt_path = "./out/vertexes.vrt";
        std::string graph_viz_path = "./out/graph.viz";

        // Set the space
        ob::StateSpacePtr space = std::make_shared<Ampli_DubinsStateStateSpace>();

        // Check entry commands
        if (vm.count("start") != 0u || vm.count("goal") != 0u)
        {
            // Check if we can use start/goal to perform the planning
            if (vm.count("start") != 0u && vm.count("goal"))
            {
                // At this point, use the start and goal information entered from the command line
                std::cerr << "Error: start and goal still not implemented from terminal." << "\n";
                return 1;

            }
            else
            {
                // At this point, we don't have enough information, throw error and terminate the program
                std::cerr << "Not enough arguments passed. For help, use --help as an argument." << "\n";
                return 1;

            }
        }
        else if (vm.count("file") != 0u)
        {
            // At this point, means we have to use the input file
            // Get the name of the input file
            auto parsed_object = json_parser::parse_input_file(input_file);

            // Once we have the parsed object, we have to launch the simulation

        }
        else
        {
            // At this point, means nothing happened, throw help message

        }
    }
    catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }

}