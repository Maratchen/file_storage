#pragma once

#include <boost/asio/awaitable.hpp>
#include <generated/file_storage.pb.h>
#include <memory>
#include <optional>

namespace file_storage::client {

    class ControlStream {
    public:
        virtual ~ControlStream() = default;
        virtual auto Write(FileSendRequest& request) -> boost::asio::awaitable<bool> = 0;
        virtual auto Read(FileSendResponse& request) -> boost::asio::awaitable<bool> = 0;
        virtual auto Read(FileSendResponse& response, std::chrono::milliseconds timeout) ->
            boost::asio::awaitable<std::optional<bool>> = 0;
        virtual auto Finish() -> boost::asio::awaitable<void> = 0;
    };

    class ControlStreamFactory {
    public:
        virtual ~ControlStreamFactory() = default;
        virtual auto MakeControlStream() -> boost::asio::awaitable<std::unique_ptr<ControlStream>> = 0;
    };

}
