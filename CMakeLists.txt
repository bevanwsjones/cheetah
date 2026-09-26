cmake_minimum_required(VERSION 3.20)

project(cheetah LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Project-wide test helper: builds a test executable linked against gtest + gmock
# (with main) and registers its tests with CTest.
#   cheetah_add_test(<name> SOURCES <files...> [LIBS <targets...>])
function(cheetah_add_test name)
    cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
    add_executable(${name} ${ARG_SOURCES})
    target_link_libraries(${name} PRIVATE ${ARG_LIBS} GTest::gtest GTest::gmock GTest::gmock_main)
    gtest_discover_tests(${name})
endfunction()

add_subdirectory(src)
