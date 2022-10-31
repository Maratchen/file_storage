#include <file_storage/server.hpp>

#include <generated/file_storage.grpc.pb.h>
#include <file_storage/server/grpc_session.hpp>
#include <file_storage/server/receive_file.hpp>
#include <file_storage/server/sync_file_writer.hpp>
#include <file_storage/server/udp_receiver.hpp>
#include <file_storage/context.hpp>
#include <grpcpp/server_builder.h>
#include <spdlog/spdlog.h>

namespace asio = boost::asio;
constexpr auto max_chunk_size = size_t(2048);

namespace file_storage
{
    Server::Server(
        ServerAddress address,
        TcpPortNumber port,
        DirectoryPath directory
    )
        : directory_(std::move(directory))
        , address_(std::move(address))
        , port_(port)
    {
    }

    void Server::Run()
    {
        auto builder = grpc::ServerBuilder();
        auto service = FileStorage::AsyncService();
        auto server = std::unique_ptr<grpc::Server>();
        auto context = Context(builder.AddCompletionQueue());

        builder.AddListeningPort(
            address_ + ":" + std::to_string(port_),
            grpc::InsecureServerCredentials()
        );
        builder.RegisterService(&service);
        server = builder.BuildAndStart();

        auto receiver_factory = server::UdpReceiverFactory(context.GetIOContext(), ChunkSize{max_chunk_size});
        auto writer_factory = server::SyncFileWriterFactory(DirectoryPath{directory_});
        auto session_factory = server::MakeGrpcSessionFactory(service);

        context.Run(std::bind(
            server::ReceiveFiles,
            session_factory.get(),
            &receiver_factory,
            &writer_factory
        ));

        server->Shutdown();
        spdlog::info("Shutdown completed");
    }

}
