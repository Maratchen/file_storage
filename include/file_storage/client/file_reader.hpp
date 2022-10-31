#pragma once

#include <file_storage/client/reader.hpp>
#include <file_storage/type_defs.hpp>
#include <filesystem>

namespace file_storage::client
{
    class FileReader final : public Reader
    {
    public:
        FileReader(std::filesystem::path path, ChunkSize chunk_size);
        auto GetFileName() const -> FileName override;
        auto ReadFile() -> std::vector<FileChunk> override;
    private:
        std::filesystem::path path_;
        size_t chunk_size_;
    };

    class FileReaderFactory : public ReaderFactory {
    public:
        explicit FileReaderFactory(std::filesystem::path path);
        auto MakeReader(ChunkSize chunk_size) -> std::unique_ptr<Reader> override;
    private:
        std::filesystem::path path_;
    };
}
