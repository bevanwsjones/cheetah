find_package(Threads REQUIRED)

add_library(platform    
    src/shared_memory_posix.cpp
)

target_include_directories(platform
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(platform
    PUBLIC Threads::Threads
)

target_compile_features(platform PUBLIC cxx_std_20)
