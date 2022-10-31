add_executable(file_storage_client client.cpp)
add_executable(file_storage_server server.cpp)

target_link_libraries(file_storage_client
                      PRIVATE file_storage_common
                              Boost::program_options)

target_link_libraries(file_storage_server
                      PRIVATE file_storage_common
                              Boost::program_options)
