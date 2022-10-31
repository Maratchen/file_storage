#pragma once

#include <boost/asio/awaitable.hpp>
#include <file_storage/type_defs.hpp>
#include <memory>
#include <optional>

namespace file_storage::client {

    class Session {
    public:
        virtual ~Session() = default;
        virtual auto Start(UdpPortNumber udp_port) -> boost::asio::awaitable<ChunkSize> = 0;
        virtual auto End(FileName file_name) -> boost::asio::awaitable<void> = 0;
        virtual auto Ack() -> boost::asio::awaitable<std::optional<FileOffset>> = 0;
    };

    class SessionFactory {
    public:
        virtual ~SessionFactory() = default;
        virtual auto MakeSession() -> boost::asio::awaitable<std::unique_ptr<Session>> = 0;
    };

}
