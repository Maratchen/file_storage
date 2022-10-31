#pragma once

#include <file_storage/client/control_stream.hpp>
#include <file_storage/type_defs.hpp>

namespace file_storage::client {

    class GrpcControlStreamFactory : public ControlStreamFactory
    {
    public:
        GrpcControlStreamFactory(const ServerAddress& address, TcpPortNumber tcp_port);
        auto MakeControlStream() -> boost::asio::awaitable<std::unique_ptr<ControlStream>> override;
    private:
        std::string address_;
        uint16_t tcp_port_;
    };

}
