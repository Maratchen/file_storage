#pragma once

#include <boost/serialization/strong_typedef.hpp>
#include <chrono>
#include <cstdint>
#include <string>

namespace file_storage
{
    BOOST_STRONG_TYPEDEF(size_t, FileOffset);
    BOOST_STRONG_TYPEDEF(uint16_t, ChunkSize);
    BOOST_STRONG_TYPEDEF(uint16_t, TcpPortNumber);
    BOOST_STRONG_TYPEDEF(uint16_t, UdpPortNumber);
    BOOST_STRONG_TYPEDEF(std::chrono::milliseconds, ReceiveTimeout);
    BOOST_STRONG_TYPEDEF(std::string, DirectoryPath);
    BOOST_STRONG_TYPEDEF(std::string, FileChunk);
    BOOST_STRONG_TYPEDEF(std::string, FileName);
    BOOST_STRONG_TYPEDEF(std::string, ServerAddress);
}