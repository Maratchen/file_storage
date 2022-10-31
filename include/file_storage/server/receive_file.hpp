#pragma once

#include <file_storage/server/receiver.hpp>
#include <file_storage/server/session.hpp>
#include <file_storage/server/writer.hpp>

namespace file_storage::server {

    boost::asio::awaitable<std::vector<std::string>> ReceiveChunks(
        Session* session,
        Receiver* receiver
    );

    boost::asio::awaitable<std::string> ReceiveEnd(
        Session* session,
        Receiver* receiver
    );

    boost::asio::awaitable<void> ReceiveFile(
        Session* session,
        ReceiverFactory* receiver_factory,
        WriterFactory* writer_factory
    );

    boost::asio::awaitable<void> ReceiveFiles(
        SessionFactory* session_factory,
        ReceiverFactory* receiver_factory,
        WriterFactory* writer_factory
    );

}

