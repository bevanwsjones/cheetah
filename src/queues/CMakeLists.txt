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
    cheetah_add_test(queues_test
        SOURCES test/test_concurrent_queue.cpp
        LIBS queues
    )

    cheetah_add_test(test_shrd_mem_ring_queue
        SOURCES test/test_shrd_mem_ring_queue.cpp
        LIBS queues
    )
endif()
