cmake_minimum_required(VERSION 3.16)

project(cpp_message_spool VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(message-spool-core
    message_spool.cpp
)

target_include_directories(message-spool-core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(message-spool
    main.cpp
)

target_link_libraries(message-spool
    PRIVATE
        message-spool-core
        Threads::Threads
)

enable_testing()

add_executable(message-spool-tests
    test.cpp
)

add_test(
    NAME QueueFIFOTest
    COMMAND message-spool-tests
)

add_executable(persistence-test
    persistence_test.cpp
)

target_link_libraries(persistence-test
    PRIVATE message-spool-core
)

add_test(
    NAME PersistenceTest
    COMMAND persistence-test
)