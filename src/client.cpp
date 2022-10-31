#include <file_storage/client.hpp>

#include <file_storage/client/file_reader.hpp>
#include <file_storage/client/grpc_session.hpp>
#include <file_storage/client/send_file.hpp>
#include <file_storage/client/udp_sender.hpp>
#include <file_storage/context.hpp>
#include <generated/file_storage.grpc.pb.h>

namespace asio = boost::asio;

namespace file_storage
{
    Client::Client(ServerAddress address,
                   TcpPortNumber tcp_port,
                   UdpPortNumber udp_port,
                   ReceiveTimeout timeout)
        : address_(std::move(address))
        , tcp_port_(tcp_port)
        , udp_port_(udp_port)
        , timeout_(timeout)
    {
    }

    void Client::SendFile(const std::filesystem::path& path)
    {
        auto context = Context(std::make_unique<grpc::CompletionQueue>());
        auto session_factory = client::MakeGrpcSessionFactory(ServerAddress{address_}, TcpPortNumber{tcp_port_}, ReceiveTimeout{timeout_});
        auto sender_factory = client::UdpSenderFactory(context.GetIOContext(), ServerAddress{address_}, UdpPortNumber{udp_port_});
        auto reader_factory = client::FileReaderFactory(path);

        context.Run(std::bind(
            client::SendFile,
            session_factory.get(),
            &reader_factory,
            &sender_factory
        ));
    }

}
