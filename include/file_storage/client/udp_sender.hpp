#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>
#include <file_storage/client/sender.hpp>
#include <file_storage/type_defs.hpp>

namespace file_storage::client
{
    class UdpSender final : public Sender
    {
    public:
        UdpSender(
            boost::asio::ip::udp::socket socket,
            boost::asio::ip::udp::endpoint endpoint
        );
        auto GetRemotePort() const noexcept -> UdpPortNumber override;
        auto Send(FileOffset offset, const std::string& chunk) -> boost::asio::awaitable<void> override;
    private:
        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint remote_;
    };

    class UdpSenderFactory : public SenderFactory {
    public:
        UdpSenderFactory(
            boost::asio::io_context& io_context,
            ServerAddress address,
            UdpPortNumber udp_port
        );
        auto MakeSender() -> std::unique_ptr<Sender> override;
    private:
        boost::asio::io_context& io_context_;
        std::string address_;
        uint16_t udp_port_;
    };
}

