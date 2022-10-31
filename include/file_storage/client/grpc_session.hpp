#pragma once

#include <file_storage/client/control_stream.hpp>
#include <file_storage/client/session.hpp>

namespace file_storage::client {

    class GrpcSession : public Session {
    public:
        GrpcSession(
            std::unique_ptr<ControlStream>&& stream,
            ReceiveTimeout timeout
        );
        auto Start(UdpPortNumber udp_port) -> boost::asio::awaitable<ChunkSize> override;
        auto End(FileName file_name) -> boost::asio::awaitable<void> override;
        auto Ack() -> boost::asio::awaitable<std::optional<FileOffset>> override;
    private:
        std::unique_ptr<client::ControlStream> stream_;
        std::chrono::milliseconds timeout_;
    };

    class GrpcSessionFactory : public SessionFactory {
    public:
        GrpcSessionFactory(
            std::unique_ptr<ControlStreamFactory>&& stream_factory,
            ReceiveTimeout receive_timeout
        );
        auto MakeSession() -> boost::asio::awaitable<std::unique_ptr<Session>> override;
    private:
        std::unique_ptr<ControlStreamFactory> stream_factory_;
        ReceiveTimeout receive_timeout_;
    };

    std::unique_ptr<SessionFactory> MakeGrpcSessionFactory(
        const ServerAddress& address,
        TcpPortNumber tcp_port,
        ReceiveTimeout receive_timeout
    );

}
