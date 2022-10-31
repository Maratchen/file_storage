#include <file_storage/client/send_file.hpp>

#include <boost/asio/experimental/awaitable_operators.hpp>

namespace asio = boost::asio;
using namespace asio::experimental::awaitable_operators;

namespace file_storage::client {

    asio::awaitable<void> SendChunk(
        Session* session,
        Sender* sender,
        FileOffset offset,
        const std::string& chunk
    ) {
        auto ack = std::optional<FileOffset>();
        while (!ack) {
            ack = co_await (
                sender->Send(offset, chunk) &&
                session->Ack()
            );
        }
    }

    asio::awaitable<void> SendFile(
        SessionFactory* session_factory,
        ReaderFactory* reader_factory,
        SenderFactory* sender_factory
    ) {
        auto sender = sender_factory->MakeSender();
        auto session = co_await session_factory->MakeSession();
        auto max_chunk_size = co_await session->Start(sender->GetRemotePort());

        auto reader = reader_factory->MakeReader(max_chunk_size);
        auto chunks = reader->ReadFile();
        auto offset = size_t();

        for (const auto& chunk : chunks) {
            co_await SendChunk(session.get(), sender.get(), FileOffset{offset}, chunk);
            offset += std::size(static_cast<const std::string&>(chunk));
        }

        co_await session->End(reader->GetFileName());
    }

}
