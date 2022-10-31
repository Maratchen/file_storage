#pragma once

#include <generated/file_storage.grpc.pb.h>
#include <file_storage/server/control_stream.hpp>
#include <file_storage/server/session.hpp>

namespace file_storage::server {

class GrpcSession : public Session {
    public:
        explicit GrpcSession(std::unique_ptr<ControlStream>&& stream);
        auto Start(ChunkSize chunk_size) -> boost::asio::awaitable<UdpPortNumber> override;
        auto Ack(FileOffset offset) -> boost::asio::awaitable<void> override;
        auto End() -> boost::asio::awaitable<FileName> override;
    private:
        std::unique_ptr<ControlStream> stream_;
    };

    class GrpcSessionFactory : public SessionFactory
    {
    public:
        explicit GrpcSessionFactory(std::unique_ptr<ControlStreamFactory>&& stream_factory);
        auto MakeSession() -> boost::asio::awaitable<std::unique_ptr<Session>> override;
    private:
        std::unique_ptr<ControlStreamFactory> stream_factory_;
    };

    std::unique_ptr<SessionFactory> MakeGrpcSessionFactory(
        FileStorage::AsyncService& service
    );

}
