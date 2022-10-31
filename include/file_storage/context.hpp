#pragma once

#include <agrpc/asio_grpc.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <functional>

namespace file_storage {

    class Context {
    public:
        explicit Context(std::unique_ptr<grpc::CompletionQueue> completion_queue);

        auto GetGrpcContext() noexcept -> agrpc::GrpcContext&;
        auto GetIOContext() noexcept -> boost::asio::io_context&;

        void Run(std::function<boost::asio::awaitable<void>()> routine);

    private:
        agrpc::GrpcContext grpc_context_;
        boost::asio::io_context io_context_;
    };
}
