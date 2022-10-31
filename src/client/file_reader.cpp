#include <file_storage/client/file_reader.hpp>

#include <spdlog/spdlog.h>
#include <fstream>

namespace file_storage::client
{
    FileReader::FileReader(std::filesystem::path path, ChunkSize chunk_size)
        : path_(std::move(path))
        , chunk_size_(chunk_size)
    {
    }

    FileName FileReader::GetFileName() const {
        return FileName{path_.filename().string()};
    }

    std::vector<FileChunk> FileReader::ReadFile() {
        const auto file_size = std::filesystem::file_size(path_);

        auto chunks = std::vector<FileChunk>();
        auto file = std::ifstream(path_.string(), std::ios::binary);

        for (std::uintmax_t offset = 0; offset < file_size; offset += chunk_size_) {
            const auto buffer_size = std::min(chunk_size_, file_size - offset);
            auto buffer = std::string(buffer_size, '\0');
            file.read(buffer.data(), buffer.size());
            chunks.emplace_back(std::move(buffer));
        }

        return chunks;
    }

    FileReaderFactory::FileReaderFactory(std::filesystem::path path)
        : path_(path)
    {
    }

    std::unique_ptr<Reader> FileReaderFactory::MakeReader(ChunkSize chunk_size) {
        return std::make_unique<FileReader>(path_, chunk_size);
    }

}
