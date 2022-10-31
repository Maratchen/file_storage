set(FILE_STORAGE_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(FILE_STORAGE_PROTO_FILES file_storage.proto)

add_library(file_storage_protos OBJECT)

target_sources(file_storage_protos
               PRIVATE ${FILE_STORAGE_PROTO_FILES})

target_link_libraries(file_storage_protos
                      PUBLIC gRPC::grpc++)

target_include_directories(file_storage_protos
                           PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(MAKE_DIRECTORY "${FILE_STORAGE_GENERATED_DIR}")

protobuf_generate(TARGET file_storage_protos
                  LANGUAGE cpp
                  PROTOC_OUT_DIR ${FILE_STORAGE_GENERATED_DIR})

protobuf_generate(TARGET file_storage_protos
                  LANGUAGE grpc
                  GENERATE_EXTENSIONS .grpc.pb.h
                                      .grpc.pb.cc
                  PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
                  PROTOC_OUT_DIR ${FILE_STORAGE_GENERATED_DIR})
