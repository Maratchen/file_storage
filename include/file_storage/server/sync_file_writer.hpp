#pragma once

#include <file_storage/server/writer.hpp>
#include <filesystem>

namespace file_storage::server
{
    class SyncFileWriter final : public Writer
    {
    public:
        explicit SyncFileWriter(std::filesystem::path file_path);
        auto WriteChunks(std::vector<std::string>&& chunks) -> boost::asio::awaitable<void> override;
    private:
        std::filesystem::path file_path_;
    };

    class SyncFileWriterFactory : public WriterFactory {
    public:
        explicit SyncFileWriterFactory(const DirectoryPath& directory_path);
        auto MakeWriter(const FileName& filename) -> std::unique_ptr<Writer> override;
    public:
        std::filesystem::path directory_path_;
    };
}

