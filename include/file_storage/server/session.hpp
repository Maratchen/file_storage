#pragma once

#include <boost/asio/awaitable.hpp>
#include <file_storage/type_defs.hpp>
#include <memory>

namespace file_storage::server {

    class Session {
    public:
        virtual ~Session() = default;
        virtual auto Start(ChunkSize) -> boost::asio::awaitable<UdpPortNumber> = 0;
        virtual auto Ack(FileOffset) -> boost::asio::awaitable<void> = 0;
        virtual auto End() -> boost::asio::awaitable<FileName> = 0;
    };

    class SessionFactory {
    public:
        virtual ~SessionFactory() = default;
        virtual auto MakeSession() -> boost::asio::awaitable<std::unique_ptr<Session>> = 0;
    };

}
