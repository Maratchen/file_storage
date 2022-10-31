#include <boost/program_options.hpp>
#include <file_storage/client.hpp>
#include <fstream>
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
            ("udp", po::value<uint16_t>()->default_value(6000), "udp port number to send data")
            ("path", po::value<std::string>(), "path to tranfering file")
            ("timeout", po::value<uint32_t>()->default_value(5000), "timeout for receving ACK");

        positions.add("address", 1).add("port", 1).add("udp", 1).add("path", 1).add("timeout", 1);

        po::store(
            po::command_line_parser(argc, argv)
                .options(description)
                .positional(positions)
                .run(),
            options
        );

        po::notify(options);

        if (!options.count("path") || options.count("help")) {
            std::cout << description << "\n";
            return 0;
        }

        auto client = fs::Client(
            fs::ServerAddress{options["address"].as<std::string>()},
            fs::TcpPortNumber{options["port"].as<uint16_t>()},
            fs::UdpPortNumber{options["udp"].as<uint16_t>()},
            fs::ReceiveTimeout{std::chrono::milliseconds(options["timeout"].as<uint32_t>())}
        );

        client.SendFile(options["path"].as<std::string>());
    }
    catch (const std::exception& error) {
        std::cerr << "Exception: " << error.what();
        return -1;
    }
    return 0;
}
