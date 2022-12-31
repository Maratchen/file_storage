#include <file_storage/server/grpc_control_stream.hpp>

#include <agrpc/asio_grpc.hpp>
#include <stdexcept>

namespace asio = boost::asio;

namespace file_storage::server {

    using AsyncStream = grpc::ServerAsyncReaderWriter<FileSendResponse, FileSendRequest>;

    struct GrpcContext : public std::enable_shared_from_this<GrpcContext> {
        auto MakeControlStream(FileStorage::AsyncService& service) ->
            asio::awaitable<std::unique_ptr<ControlStream>>;
        grpc::ServerContext context_;
    };

    class GrpcControlStream : public ControlStream {
    public:
        GrpcControlStream(
            std::shared_ptr<GrpcContext>&& context,
            std::unique_ptr<AsyncStream>&& stream
        );
        auto Read(FileSendRequest& request) -> asio::awaitable<bool> override;
        auto Write(FileSendResponse& response) -> asio::awaitable<bool> override;
        auto Finish() -> boost::asio::awaitable<bool> override;
    private:
        std::shared_ptr<GrpcContext> context_;
        std::unique_ptr<AsyncStream> stream_;
    };

    asio::awaitable<std::unique_ptr<ControlStream>> GrpcContext::MakeControlStream(
        FileStorage::AsyncService& service
    ) {
        auto stream = std::make_unique<AsyncStream>(&context_);
        auto is_request_ok = co_await agrpc::request(
            &FileStorage::AsyncService::RequestSendFile,
            service, context_, *stream
        );
        if (!is_request_ok) {
            throw std::runtime_error("grpc: failed to get request");
        }
        co_return std::make_unique<GrpcControlStream>(
            shared_from_this(), std::move(stream)
        );
    }

    GrpcControlStream::GrpcControlStream(
        std::shared_ptr<GrpcContext>&& context,
        std::unique_ptr<AsyncStream>&& stream
    )
        : context_(std::move(context))
        , stream_(std::move(stream))
    {
    }

    asio::awaitable<bool> GrpcControlStream::Read(FileSendRequest& request) {
        return agrpc::read(stream_, request);
    }

    asio::awaitable<bool> GrpcControlStream::Write(FileSendResponse& response) {
        return agrpc::write(stream_, response);
    }

    asio::awaitable<bool> GrpcControlStream::Finish() {
        return agrpc::finish(stream_, grpc::Status::OK);
    }

    GrpcControlStreamFactory::GrpcControlStreamFactory(
        FileStorage::AsyncService& service
    )
        : service_(service)
    {
    }

    auto GrpcControlStreamFactory::MakeControlStream() ->
        asio::awaitable<std::unique_ptr<ControlStream>>
    {
        auto context = std::make_shared<GrpcContext>();
        co_return co_await context->MakeControlStream(service_);
    }
}
