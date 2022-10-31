#pragma once

#include <file_storage/server/control_stream.hpp>
#include <generated/file_storage.grpc.pb.h>

namespace file_storage::server {

    class GrpcControlStreamFactory : public ControlStreamFactory {
    public:
        explicit GrpcControlStreamFactory(FileStorage::AsyncService& service);
        auto MakeControlStream() -> boost::asio::awaitable<std::unique_ptr<ControlStream>> override;
    private:
        FileStorage::AsyncService& service_;
    };

}
