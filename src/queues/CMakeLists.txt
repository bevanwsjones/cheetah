find_package(Threads REQUIRED)

add_library(queues
    src/concurrent_queue.cpp
    src/shrd_mem_ring_queue.cpp
)

target_include_directories(queues
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(queues
    PUBLIC Threads::Threads
)

target_compile_features(queues PUBLIC cxx_std_20)

if(BUILD_TESTING)
    add_executable(queues_test
        test/test_concurrent_queue.cpp
    )

    target_link_libraries(queues_test
        PRIVATE queues Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(queues_test)
endif()
