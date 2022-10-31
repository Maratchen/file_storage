#include <file_storage/client/grpc_session.hpp>

#include <file_storage/client/grpc_control_stream.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace asio = boost::asio;

namespace file_storage::client {

    GrpcSession::GrpcSession(
        std::unique_ptr<ControlStream>&& stream,
        ReceiveTimeout timeout
    )
        : stream_(std::move(stream))
        , timeout_(timeout)
    {
    }

    asio::awaitable<ChunkSize> GrpcSession::Start(UdpPortNumber udp_port) {
        auto request = FileSendRequest();
        auto response = FileSendResponse();
        request.mutable_start()->set_udp_port(udp_port);
        auto is_write_ok = co_await stream_->Write(request);
        if (!is_write_ok) {
            throw std::runtime_error("grpc: failed to send the start command");
        }
        spdlog::info("grpc: the start command has been sent");
        auto is_read_ok = co_await stream_->Read(response);
        if (!is_read_ok || !response.has_ready()) {
            throw std::runtime_error("grpc: failed to receive the ready response");
        }
        spdlog::info(
            "grpc: the ready command received (chunk size is {} bytes)",
            response.ready().max_chunk_size()
        );
        co_return response.ready().max_chunk_size();
    }

    asio::awaitable<void> GrpcSession::End(FileName file_name) {

        auto request = FileSendRequest();
        request.mutable_end()->mutable_filename()->assign(std::move(file_name));
        auto is_write_ok = co_await stream_->Write(request);
        if (!is_write_ok) {
            throw std::runtime_error("grpc: failed to send the end command");
        }
        spdlog::info("grpc: the end command has been sent");
        co_await stream_->Finish();
    }

    asio::awaitable<std::optional<FileOffset>> GrpcSession::Ack() {
        spdlog::info("grpc: waiting for ack...");
        auto response = FileSendResponse();
        auto maybe_read_ok = co_await stream_->Read(response, timeout_);
        if (!maybe_read_ok.has_value()) {
            spdlog::warn("grpc: receiving ack timed out");
            co_return std::nullopt;
        }
        auto is_read_ok = maybe_read_ok.value();
        if (!is_read_ok || !response.has_ack()) {
            throw std::runtime_error("grpc: failed to receive the ack response");
        }

        spdlog::info("grpc: the ack command received (offset is {})", response.ack().offset());
        co_return response.ack().offset();
    }

    GrpcSessionFactory::GrpcSessionFactory(
        std::unique_ptr<ControlStreamFactory>&& stream_factory,
        ReceiveTimeout receive_timeout
    )
        : stream_factory_(std::move(stream_factory))
        , receive_timeout_(receive_timeout)
    {
    }

    asio::awaitable<std::unique_ptr<Session>> GrpcSessionFactory::MakeSession() {
        co_return std::make_unique<GrpcSession>(
            co_await stream_factory_->MakeControlStream(),
            receive_timeout_
        );
    }

    std::unique_ptr<SessionFactory> MakeGrpcSessionFactory(
        const ServerAddress& address,
        TcpPortNumber tcp_port,
        ReceiveTimeout receive_timeout
    ) {
        return std::make_unique<GrpcSessionFactory>(
            std::make_unique<GrpcControlStreamFactory>(address, tcp_port),
            receive_timeout
        );
    }
}
