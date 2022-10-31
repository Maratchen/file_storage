#include <file_storage/server/receive_file.hpp>

#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/this_coro.hpp>
#include <functional>
#include <map>

namespace asio = boost::asio;

using namespace asio::experimental::awaitable_operators;

namespace file_storage::server {

    asio::awaitable<std::vector<std::string>> ReceiveChunks(
        Session* session,
        Receiver* receiver
    ) {
        auto chunks = std::vector<std::string>();
        auto chunks_by_offset = std::map<size_t, std::string>();
        for (;;) {
            auto [offset, chunk] = co_await receiver->Receive();
            if (chunk.empty()) {
                break;
            }
            chunks_by_offset.insert_or_assign(offset, std::move(chunk));
            co_await session->Ack(FileOffset{offset});
        }
        chunks.reserve(chunks_by_offset.size());
        for (auto&& [offset, chunk] : chunks_by_offset) {
            chunks.push_back(std::move(chunk));
        }
        co_return chunks;
    }

    asio::awaitable<std::string> ReceiveEnd(
        Session* session,
        Receiver* receiver
    ) {
        auto filename = co_await session->End();
        receiver->Close();
        co_return filename;
    }

    asio::awaitable<void> ReceiveFile(
        Session* session,
        ReceiverFactory* receiver_factory,
        WriterFactory* writer_factory
    ) {
        auto udp_port = co_await session->Start(receiver_factory->GetChunkSize());
        auto receiver = receiver_factory->MakeReceiver(udp_port);

        auto [chunks, filename] = co_await (
            ReceiveChunks(session, receiver.get()) &&
            ReceiveEnd(session, receiver.get())
        );

        auto writer = writer_factory->MakeWriter(FileName{filename});
        co_await writer->WriteChunks(std::move(chunks));
    }

    asio::awaitable<void> ReceiveFiles(
        SessionFactory* session_factory,
        ReceiverFactory* receiver_factory,
        WriterFactory* writer_factory
    ) {
        for (;;) {
            auto session = co_await session_factory->MakeSession();
            auto executor = co_await asio::this_coro::executor;

            asio::co_spawn(
                executor,
                [=, session = std::move(session)] () -> asio::awaitable<void> {
                    co_await ReceiveFile(session.get(), receiver_factory, writer_factory);
                },
                asio::detached
            );
        }
    }

}
