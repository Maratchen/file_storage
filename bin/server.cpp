#include <boost/program_options.hpp>
#include <file_storage/server.hpp>
#include <iostream>

namespace fs = file_storage;
namespace po = boost::program_options;

int main(int argc, const char* argv[]) {
    try {
        auto description = po::options_description("Allowed options");
        auto positions = po::positional_options_description();
        auto options = po::variables_map();

        description.add_options()
            ("help", "produce help message")
            ("address", po::value<std::string>()->default_value("0.0.0.0"), "server IP address")
            ("port", po::value<uint16_t>()->default_value(5555), "server port number")
            ("path", po::value<std::string>(), "path to directory");

        positions.add("address", 1).add("port", 1).add("path", 1);

        po::store(
            po::command_line_parser(argc, argv)
                .options(description)
                .positional(positions)
                .run(),
            options
        );

        po::notify(options);

        if (options.count("help")) {
            std::cout << description << "\n";
            return 0;
        }

        if (!options.count("path")) {
            std::cout << "path was not set.\n";
            return -1;
        }

        auto server = fs::Server(
            fs::ServerAddress{options["address"].as<std::string>()},
            fs::TcpPortNumber{options["port"].as<uint16_t>()},
            fs::DirectoryPath{options["path"].as<std::string>()}
        );

        std::cout << "listening "
            << options["address"].as<std::string>() << ":"
            << options["port"].as<uint16_t>() << "...\n";

        server.Run();
        //server.Test();
    }
    catch (const std::exception& error) {
        std::cerr << "Exception: " << error.what();
        return -1;
    }
    return 0;
}