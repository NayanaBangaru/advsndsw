#include <iostream>

#include <filesystem>

#include <boost/program_options.hpp>


namespace po = boost::program_options;
int main(int argc, char ** argv) {

    po::options_description desc("SNDLHC event filter");
    desc.add_options()
    ("help,h", "Show help message")
    ("input,i", po::value<std::string>()->required(), "Input file name (or reg exp)")
    ("output,o", po::value<std::string>()->required(), "Output file name")
    ("geofile,g", po::value<std::string>()->required(), "Geometry file name")
    ("pipeline,p", po::value<std::string>()->required(), "Path to process pipeline definition. For example, in ${SNDSW_ROOT}/analysis/pipelines/")
    ("recofile,r", po::value<std::string>()->default_value(""), "Path to reconstructed muon tracks file.");
  
    po::variables_map vm;

    try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    std::string input = vm["input"].as<std::string>();
    std::string output = vm["output"].as<std::string>();
    std::string geofile = vm["geofile"].as<std::string>();
    std::string pipeline_filename = vm["pipeline"].as<std::string>();
    std::string recofile = vm["recofile"].as<std::string>();
    
    // Check if files exist
    if (! std::filesystem::exists(geofile)) {
        std::cout << "Geometry file does not exist: "+geofile; 
    }
    if (! std::filesystem::exists(pipeline_filename)) {
        std::cout << "Cut set definition file does not exist: "+pipeline_filename;
    }

    std::cout << "sndEventFilter works!" << std::endl;
    return 0;
}
