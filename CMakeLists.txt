cmake_minimum_required(VERSION 3.14)
project(PACA VERSION 0.1.0 LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Main Library Target (Header-Only)
# ------------------------------------------------------------------------------
add_library(paca INTERFACE)
add_library(paca::paca ALIAS paca)

# Set include directories
target_include_directories(paca INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Enforce modern C++ standard
target_compile_features(paca INTERFACE cxx_std_20)

# ------------------------------------------------------------------------------
# Testing
# ------------------------------------------------------------------------------
option(PACA_BUILD_TESTS "Build the PACA test suite" ON)

if(PACA_BUILD_TESTS)
    enable_testing()

    # Fetch Catch2 testing framework
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2 # Latest stable Catch2 v3 release
    )
    FetchContent_MakeAvailable(Catch2)

    # Define the test executable
    add_executable(paca_tests tests/test_parser.cpp)
    
    # Link our library and the Catch2 entry point
    target_link_libraries(paca_tests PRIVATE paca::paca Catch2::Catch2WithMain)

    # Register tests with CTest
    include(Catch)
    catch_discover_tests(paca_tests)
endif()
