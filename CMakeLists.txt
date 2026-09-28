cmake_minimum_required(VERSION 3.20)

project(cheetah LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_TESTING ON) # For the moment hard code to on

include(FetchContent)

# testing content
if(${BUILD_TESTING})
    include(CTest)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)


    FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG        v3.3.1
        GIT_SHALLOW    TRUE
    )
    set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(CXXOPTS_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(cxxopts)

    # Project-wide test helper: builds a test executable linked against gtest + gmock
    # (with main) and registers its tests with CTest.
    #   cheetah_add_test(<name> SOURCES <files...> [LIBS <targets...>])
    function(cheetah_add_test name)
        cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
        add_executable(${name} ${ARG_SOURCES})
        target_link_libraries(${name} PRIVATE ${ARG_LIBS} GTest::gtest GTest::gmock GTest::gmock_main)
        gtest_discover_tests(${name})
    endfunction()
endif()



add_subdirectory(src)
