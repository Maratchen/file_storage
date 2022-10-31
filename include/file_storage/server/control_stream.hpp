#pragma once

#include <boost/asio/awaitable.hpp>
#include <generated/file_storage.pb.h>
#include <memory>

namespace file_storage::server {

    class ControlStream {
    public:
        virtual ~ControlStream() = default;
        virtual auto Read(FileSendRequest& request) -> boost::asio::awaitable<bool> = 0;
        virtual auto Write(FileSendResponse& response) -> boost::asio::awaitable<bool> = 0;
        virtual auto Finish() -> boost::asio::awaitable<bool> = 0;
    };

    class ControlStreamFactory {
    public:
        virtual ~ControlStreamFactory() = default;
        virtual auto MakeControlStream() -> boost::asio::awaitable<std::unique_ptr<ControlStream>> = 0;
    };
}
