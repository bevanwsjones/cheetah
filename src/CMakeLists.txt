add_subdirectory(platform)
add_subdirectory(queues)

# temporary scratch executable for manual experimentation, not part of the real build
add_executable(scratch main.cpp)
target_link_libraries(scratch)

