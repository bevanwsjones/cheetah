find_package(Threads REQUIRED)

add_library(platform    
    src/process.cpp
    src/shared_memory_posix.cpp
)

target_include_directories(platform
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(platform
    PUBLIC Threads::Threads
)

target_compile_features(platform PUBLIC cxx_std_20)

if(BUILD_TESTING)
    add_executable(test_binary test/main_test_bin.cpp)

    cheetah_add_test(process_test
        SOURCES test/test_process_posix.cpp
        LIBS plaform
    ) # Add test_binary dependency.
endif()