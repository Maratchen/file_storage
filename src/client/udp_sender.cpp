#include <file_storage/client/udp_sender.hpp>

#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <generated/file_storage.pb.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace asio = boost::asio;

namespace file_storage::client
{
    UdpSender::UdpSender(
        asio::ip::udp::socket socket,
        asio::ip::udp::endpoint endpoint
    )
        : socket_(std::move(socket))
        , remote_(std::move(endpoint))
    {
        socket_.open(asio::ip::udp::v4());
    }

    UdpPortNumber UdpSender::GetRemotePort() const noexcept {
        return UdpPortNumber{remote_.port()};
    }

    boost::asio::awaitable<void> UdpSender::Send(FileOffset offset, const std::string& chunk) {
        auto message = FileChunkMessage();
        message.set_offset(offset);
        message.set_data(chunk);
        spdlog::info("udp: sending {} bytes (offset is {})...", chunk.size(), offset);
        auto [error, bytes_transferred] = co_await socket_.async_send_to(
            asio::buffer(message.SerializeAsString()), remote_,
            asio::experimental::as_tuple(asio::use_awaitable)
        );
        if (error) {
            throw std::runtime_error("udp: filed to send a file chunk");
        }
    }

    UdpSenderFactory::UdpSenderFactory(
        boost::asio::io_context& io_context,
        ServerAddress address,
        UdpPortNumber udp_port
    )
        : io_context_(io_context)
        , address_(std::move(address))
        , udp_port_(udp_port)
    {
    }

    std::unique_ptr<Sender> UdpSenderFactory::MakeSender() {
        return std::make_unique<UdpSender>(
            asio::ip::udp::socket(io_context_),
            asio::ip::udp::endpoint(asio::ip::address::from_string(address_), udp_port_)
        );
    }
}
