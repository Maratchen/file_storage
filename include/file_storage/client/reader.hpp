#pragma once

#include <file_storage/type_defs.hpp>
#include <memory>
#include <vector>

namespace file_storage::client
{
    class Reader
    {
    public:
        virtual ~Reader() noexcept = default;
        virtual auto GetFileName() const -> FileName = 0;
        virtual auto ReadFile() -> std::vector<FileChunk> = 0;
    };

    class ReaderFactory {
    public:
        virtual ~ReaderFactory() = default;
        virtual auto MakeReader(ChunkSize chunk_size) -> std::unique_ptr<Reader> = 0;
    };
}
