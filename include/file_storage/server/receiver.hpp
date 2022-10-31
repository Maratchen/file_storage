#pragma once

#include <boost/asio/awaitable.hpp>
#include <file_storage/type_defs.hpp>
#include <memory>
#include <tuple>

namespace file_storage::server {

    class Receiver {
    public:
        using ReceiveResultType = std::tuple<FileOffset, std::string>;

        virtual ~Receiver() = default;
        virtual auto Receive() -> boost::asio::awaitable<ReceiveResultType> = 0;
        virtual void Close() = 0;
    };

    class ReceiverFactory {
    public:
        virtual ~ReceiverFactory() = default;
        virtual auto GetChunkSize() const noexcept -> ChunkSize = 0;
        virtual auto MakeReceiver(UdpPortNumber) -> std::unique_ptr<Receiver> = 0;
    };

}
