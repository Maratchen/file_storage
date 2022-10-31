#pragma once

#include <file_storage/type_defs.hpp>
#include <filesystem>
#include <memory>

namespace file_storage
{
    class Client
    {
    public:
        Client(
            ServerAddress address,
            TcpPortNumber tcp_port,
            UdpPortNumber udp_port,
            ReceiveTimeout timeout);

        void SendFile(const std::filesystem::path& path);

    private:
        std::string address_;
        uint16_t tcp_port_;
        uint16_t udp_port_;
        std::chrono::milliseconds timeout_;
    };
}
