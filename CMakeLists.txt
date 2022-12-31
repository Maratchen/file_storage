cmake_minimum_required(VERSION 3.24.0)
project(file_storage VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(FILE_STORAGE_BUILD_TEST "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(FILE_STORAGE_BUIILD_PACKAGE "Build package" ${PROJECT_IS_TOP_LEVEL})

find_package(asio-grpc REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS program_options
                        serialization)
find_package(gRPC REQUIRED)
find_package(Protobuf REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(protos)

add_library(file_storage_common)

target_include_directories(file_storage_common
                           PUBLIC include)

target_sources(file_storage_common
               PRIVATE src/client/file_reader.cpp
                       src/client/grpc_control_stream.cpp
                       src/client/grpc_session.cpp
                       src/client/send_file.cpp
                       src/client/udp_sender.cpp
                       src/server/grpc_control_stream.cpp
                       src/server/grpc_session.cpp
                       src/server/receive_file.cpp
                       src/server/sync_file_writer.cpp
                       src/server/udp_receiver.cpp
                       src/client.cpp
                       src/context.cpp
                       src/server.cpp)

target_compile_options(file_storage_common
                       PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
                              $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_link_libraries(file_storage_common
                      PUBLIC asio-grpc::asio-grpc
                             file_storage_protos
                             Boost::serialization
                             gRPC::grpc++
                             spdlog::spdlog)

add_subdirectory(bin)

if (${FILE_STORAGE_BUIILD_PACKAGE})
  set(CPACK_PROJECT_NAME ${PROJECT_NAME})
  set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
  include(CPack)
endif()

if (${FILE_STORAGE_BUILD_TEST})
  include(CTest)
  enable_testing()
endif()
