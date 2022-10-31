#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>
#include <file_storage/server/receiver.hpp>
#include <vector>

namespace file_storage::server {

    class UdpReceiver : public Receiver {
    public:
        UdpReceiver(boost::asio::ip::udp::socket&&, ChunkSize);
        auto Receive() -> boost::asio::awaitable<ReceiveResultType> override;
        void Close() override;
    private:
        boost::asio::ip::udp::socket socket_;
        std::vector<std::byte> buffer_;
    };

    class UdpReceiverFactory : public ReceiverFactory {
    public:
        UdpReceiverFactory(boost::asio::io_context&, ChunkSize);
        auto GetChunkSize() const noexcept -> ChunkSize override;
        auto MakeReceiver(UdpPortNumber) -> std::unique_ptr<Receiver> override;
    private:
        boost::asio::io_context& io_context_;
        ChunkSize chunk_size_;
    };

}
