#pragma once

#include <boost/asio/awaitable.hpp>
#include <file_storage/type_defs.hpp>
#include <memory>

namespace file_storage::client
{
    class Sender
    {
    public:
        virtual ~Sender() = default;
        virtual auto GetRemotePort() const noexcept -> UdpPortNumber = 0;
        virtual auto Send(FileOffset offset, const std::string& chunk) -> boost::asio::awaitable<void> = 0;
    };

    class SenderFactory {
    public:
        virtual ~SenderFactory() = default;
        virtual auto MakeSender() -> std::unique_ptr<Sender> = 0;
    };
}

