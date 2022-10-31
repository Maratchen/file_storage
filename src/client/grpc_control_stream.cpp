#include <file_storage/client/grpc_control_stream.hpp>

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <generated/file_storage.grpc.pb.h>
#include <grpcpp/alarm.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

namespace asio = boost::asio;
using namespace boost::asio::experimental::awaitable_operators;

namespace file_storage::client {

    using AsyncStream = grpc::ClientAsyncReaderWriter<FileSendRequest, FileSendResponse>;

    struct GrpcContext : std::enable_shared_from_this<GrpcContext> {
        explicit GrpcContext(std::unique_ptr<FileStorage::Stub>&& stub);
        auto MakeControlStream() -> boost::asio::awaitable<std::unique_ptr<ControlStream>>;

        std::unique_ptr<FileStorage::Stub> stub_;
        grpc::ClientContext context_;
    };

    class GrpcControlStream : public ControlStream {
    public:
        GrpcControlStream(
            std::shared_ptr<GrpcContext> context,
            std::unique_ptr<AsyncStream>&& stream
        )
            : context_(std::move(context))
            , stream_(std::move(stream))
        {
        }

        auto Write(FileSendRequest& request) -> asio::awaitable<bool> override {
            return agrpc::write(stream_, request);
        }

        auto Read(FileSendResponse& response) -> asio::awaitable<bool> override {
            return agrpc::read(stream_, response);
        }

        auto Read(FileSendResponse& response, std::chrono::milliseconds timeout) ->
            asio::awaitable<std::optional<bool>> override
        {
            auto deadline = std::chrono::system_clock::now() + timeout;
            auto result = co_await (
                agrpc::read(stream_, response) ||
                agrpc::wait(alarm_, deadline)
            );
            if (result.index() != 0) {
                co_return  std::nullopt;
            }
            co_return std::get<0>(result);
        }

        auto Finish() -> asio::awaitable<void> override {
            auto status = grpc::Status();
            co_await agrpc::writes_done(stream_);
            co_await agrpc::finish(stream_, status);
        }

    private:
        std::shared_ptr<GrpcContext> context_;
        std::unique_ptr<AsyncStream> stream_;
        grpc::Alarm alarm_;
    };

    GrpcContext::GrpcContext(std::unique_ptr<FileStorage::Stub>&& stub)
        : stub_(std::move(stub))
    {
    }

    asio::awaitable<std::unique_ptr<ControlStream>> GrpcContext::MakeControlStream() {
        auto stream = std::unique_ptr<AsyncStream>();
        auto is_request_ok = co_await agrpc::request(
            &FileStorage::Stub::PrepareAsyncSendFile,
            stub_, context_, stream
        );
        if (!is_request_ok) {
            throw std::runtime_error("grpc: failed to create request");
        }
        co_return std::make_unique<GrpcControlStream>(
            shared_from_this(), std::move(stream)
        );
    }

    GrpcControlStreamFactory::GrpcControlStreamFactory(
        const ServerAddress& address,
        TcpPortNumber tcp_port
    )
        : address_(address)
        , tcp_port_(tcp_port)
    {
    }

    asio::awaitable<std::unique_ptr<ControlStream>> GrpcControlStreamFactory::MakeControlStream() {
        auto context = std::make_shared<GrpcContext>(
            FileStorage::NewStub(
                grpc::CreateChannel(
                    address_ + ":" + std::to_string(tcp_port_),
                    grpc::InsecureChannelCredentials()
                )
            )
        );
        co_return co_await context->MakeControlStream();
    }

}
