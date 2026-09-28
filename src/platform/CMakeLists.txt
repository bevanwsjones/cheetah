find_package(Threads REQUIRED)

add_library(platform    
    src/process_posix.cpp
    src/shared_memory_posix.cpp
)

target_include_directories(platform
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(platform
    PUBLIC Threads::Threads
)

target_compile_features(platform PUBLIC cxx_std_20)

if(${BUILD_TESTING})
    add_executable(platform_test_binary test/platform_test_binary.cpp)
    target_link_libraries(platform_test_binary PRIVATE cxxopts::cxxopts)

    cheetah_add_test(process_test
        SOURCES test/test_process_posix.cpp
        LIBS platform
    )
    add_dependencies(process_test platform_test_binary)

endif()