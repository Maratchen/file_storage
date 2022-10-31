#pragma once

#include <file_storage/type_defs.hpp>

namespace file_storage
{
    class Server
    {
    public:
        Server(
            ServerAddress address,
            TcpPortNumber port,
            DirectoryPath directory
        );

        void Run();

    private:
        std::string directory_;
        std::string address_;
        uint16_t port_;
    };
}