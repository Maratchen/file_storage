#pragma once

#include <file_storage/client/reader.hpp>
#include <file_storage/client/sender.hpp>
#include <file_storage/client/session.hpp>

namespace file_storage::client {

    boost::asio::awaitable<void> SendChunk(
        Session* session,
        Sender* sender,
        FileOffset offset,
        const std::string& chunk
    );

    boost::asio::awaitable<void> SendFile(
        SessionFactory* session_factory,
        ReaderFactory* reader_factory,
        SenderFactory* sender_factory
    );

}
