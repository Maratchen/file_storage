#include <file_storage/server/udp_receiver.hpp>

#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <generated/file_storage.pb.h>
#include <spdlog/spdlog.h>

namespace asio = boost::asio;
constexpr auto protocol_overhead = size_t(6);

namespace file_storage::server {

    UdpReceiver::UdpReceiver(boost::asio::ip::udp::socket&& socket, ChunkSize max_chunk_size)
        : socket_(std::move(socket))
        , buffer_(max_chunk_size + protocol_overhead)
    {
        spdlog::info("udp: listening {}:{}...",
            socket_.local_endpoint().address().to_string(),
            socket_.local_endpoint().port()
        );
    }

    asio::awaitable<Receiver::ReceiveResultType> UdpReceiver::Receive() {
        auto chunk = FileChunkMessage();
        auto [error, received_bytes] = co_await socket_.async_receive(
            asio::buffer(buffer_.data(), buffer_.size()),
            asio::experimental::as_tuple(asio::use_awaitable)
        );
        if (asio::error::operation_aborted == error) {
            co_return ReceiveResultType{};
        }
        if (error) {
            spdlog::error("udp: failed to receive data");
            co_return ReceiveResultType{};
        }
        spdlog::info("udp: {} bytes transferred", received_bytes);
        if (!chunk.ParseFromArray(buffer_.data(), received_bytes)) {
            spdlog::error("udp: failed to parse a file chunk");
            co_return ReceiveResultType{};
        }
        spdlog::info("udp: {} bytes received (offset {})",
            chunk.data().size(), chunk.offset()
        );
        co_return ReceiveResultType{
            chunk.offset(),
            std::move(*chunk.mutable_data())
        };
    }

    void UdpReceiver::Close() {
        socket_.close();
    }

    UdpReceiverFactory::UdpReceiverFactory(asio::io_context& io_context, ChunkSize chunk_size)
        : io_context_(io_context)
        , chunk_size_(chunk_size)
    {
    }

    ChunkSize UdpReceiverFactory::GetChunkSize() const noexcept {
        return chunk_size_;
    }

    std::unique_ptr<Receiver> UdpReceiverFactory::MakeReceiver(UdpPortNumber port_number) {
        auto endpoint = asio::ip::udp::endpoint(asio::ip::udp::v4(), port_number);
        auto socket = asio::ip::udp::socket(io_context_, endpoint);
        return std::make_unique<UdpReceiver>(std::move(socket), chunk_size_);
    }
}
