#include <file_storage/server/sync_file_writer.hpp>

#include <spdlog/spdlog.h>
#include <numeric>
#include <fstream>

namespace file_storage::server
{
    SyncFileWriter::SyncFileWriter(std::filesystem::path file_path)
        : file_path_(std::move(file_path))
    {
    }

    boost::asio::awaitable<void> SyncFileWriter::WriteChunks(
        std::vector<std::string>&& chunks
    ) {
        auto file_path = file_path_.string();
        auto file_size = std::accumulate(
            chunks.begin(), chunks.end(), size_t(),
            [] (auto size, const auto& chunk) {
                return size + chunk.size();
            }
        );
        spdlog::info("writing {} bytes to {}...", file_size, file_path);
        auto file = std::ofstream(file_path, std::ios::binary);
        for (const auto& chunk : chunks) {
            file.write(chunk.data(), chunk.size());
        }
        co_return;
    }

    SyncFileWriterFactory::SyncFileWriterFactory(const DirectoryPath& directory_path)
        : directory_path_(static_cast<const std::string&>(directory_path))
    {
    }

    std::unique_ptr<Writer> SyncFileWriterFactory::MakeWriter(const FileName& filename) {
        return std::make_unique<SyncFileWriter>(directory_path_ / static_cast<const std::string&>(filename));
    }
}
