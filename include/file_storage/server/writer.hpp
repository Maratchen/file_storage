#pragma once

#include <boost/asio/awaitable.hpp>
#include <file_storage/type_defs.hpp>
#include <memory>
#include <vector>

namespace file_storage::server
{
    class Writer {
    public:
        virtual ~Writer() noexcept = default;
        virtual auto WriteChunks(std::vector<std::string>&& chunks) -> boost::asio::awaitable<void> = 0;
    };

    class WriterFactory {
    public:
        virtual ~WriterFactory() = default;
        virtual auto MakeWriter(const FileName& filename) -> std::unique_ptr<Writer> = 0;
    };
}
