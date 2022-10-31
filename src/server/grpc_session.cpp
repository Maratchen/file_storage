#include <file_storage/server/grpc_session.hpp>

#include <agrpc/asio_grpc.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <file_storage/server/grpc_control_stream.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace asio = boost::asio;

namespace file_storage::server {

    GrpcSession::GrpcSession(std::unique_ptr<ControlStream>&& stream)
        : stream_(std::move(stream))
    {
    }

    asio::awaitable<UdpPortNumber> GrpcSession::Start(ChunkSize chunk_size) {
        auto request = FileSendRequest();
        auto response = FileSendResponse();
        auto is_read_ok = co_await stream_->Read(request);
        if (!is_read_ok || !request.has_start()) {
            throw std::runtime_error("grpc: failed to receive the start request");
        }
        spdlog::info("grpc: the start command received");
        const auto udp_port = boost::numeric_cast<uint16_t>(request.start().udp_port());
        const auto max_chunk_size = static_cast<uint16_t>(chunk_size);
        response.mutable_ready()->set_max_chunk_size(max_chunk_size);
        auto is_write_ok = co_await stream_->Write(response);
        if (!is_write_ok) {
            throw std::runtime_error("grpc: failed to send the ready response");
        }
        co_return UdpPortNumber{udp_port};
    }

    asio::awaitable<void> GrpcSession::Ack(FileOffset offset) {
        auto response = FileSendResponse();
        response.mutable_ack()->set_offset(offset);
        auto is_write_ok = co_await stream_->Write(response);
        if (!is_write_ok) {
            throw std::runtime_error("grpc: failed to send the ack response");
        }
    }

    asio::awaitable<FileName> GrpcSession::End() {
        auto request = FileSendRequest();
        auto is_read_ok = co_await stream_->Read(request);
        if (!is_read_ok || !request.has_end()) {
            throw std::runtime_error("grpc: failed to receive the end request");
        }
        spdlog::info("grpc: the end command received");
        co_await stream_->Finish();
        co_return FileName{request.end().filename()};
    }

    GrpcSessionFactory::GrpcSessionFactory(
        std::unique_ptr<ControlStreamFactory>&& stream_factory
    )
        : stream_factory_(std::move(stream_factory))
    {
    }

    asio::awaitable<std::unique_ptr<Session>> GrpcSessionFactory::MakeSession() {
        co_return std::make_unique<GrpcSession>(
            co_await stream_factory_->MakeControlStream()
        );
    }

    std::unique_ptr<SessionFactory> MakeGrpcSessionFactory(
        FileStorage::AsyncService& service
    ) {
        return std::make_unique<GrpcSessionFactory>(
            std::make_unique<GrpcControlStreamFactory>(service)
        );
    }

}