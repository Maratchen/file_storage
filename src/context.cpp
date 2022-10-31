#include <file_storage/context.hpp>

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>
#include <spdlog/spdlog.h>
#include <thread>

namespace asio = boost::asio;

namespace file_storage {

    namespace {
        constexpr auto IO_THREADS_COUNT = 1;

        void run_io_context(asio::io_context& context) {
            spdlog::info("io_thread has started");
            try {
                context.run();
            }
            catch (const std::exception& error) {
                spdlog::error("excpetion in io thread: {}", error.what());
            }
            spdlog::info("io_thread has ended");
        }
    }

    Context::Context(std::unique_ptr<grpc::CompletionQueue> completion_queue)
        : grpc_context_(std::move(completion_queue))
        , io_context_(IO_THREADS_COUNT)
    {
    }

    auto Context::GetGrpcContext() noexcept -> agrpc::GrpcContext& {
        return grpc_context_;
    }

    auto Context::GetIOContext() noexcept -> asio::io_context& {
        return io_context_;
    }

    void Context::Run(std::function<asio::awaitable<void>()> routine) {
        auto io_thread = std::thread(run_io_context, std::ref(io_context_));
        auto signals = asio::signal_set(io_context_, SIGINT, SIGTERM);

        signals.async_wait([&](auto, auto) {
            grpc_context_.stop();
        });

        asio::co_spawn(
            grpc_context_,
            [&]() -> asio::awaitable<void> {
                try {
                    co_await routine();
                }
                catch (const std::exception& error) {
                    spdlog::error(error.what());
                }
            },
            asio::detached
        );

        grpc_context_.run();
        io_context_.stop();
        io_thread.join();
    }

}

