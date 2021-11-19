# Setting up minimum cmake required
cmake_minimum_required(VERSION 3.12)

# Setting up the project
project(Methan LANGUAGES CXX VERSION 1.0.0)

# Extracting the version form the project version
include(cmake/ExtractVersion.cmake)
extract_version(${CMAKE_PROJECT_VERSION} "METHAN")
message("-- Methan v${METHAN_VERSION}")

# Listing of all the options available to configure the project
option(METHAN_BUILD_TEST "Building the test(s)" OFF)
option(METHAN_BUILD_SHARED "Building METHAN as a Shared library" OFF)
option(METHAN_EXPOSE_PRIVATE "Install private header along side standard headers" OFF)
option(METHAN_DEBUG "Sets METHAN in a debug environment (enhanced testing)" ON)
option(METHAN_FORCE_ASSERTION "Force METHAN to expand assertion (even if METHAN_DEBUG ain't defined)" OFF)

# Setting configuration variable(s)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(METHAN_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/methan")
set(METHAN_INSTALL_INCLUDE_DIR "${METHAN_INSTALL_PREFIX}/include")
set(METHAN_INSTALL_BINARY_DIR "${METHAN_INSTALL_PREFIX}/bin")
set(METHAN_INSTALL_LIBRARY_DIR "${METHAN_INSTALL_PREFIX}/lib")
set(METHAN_INSTALL_CMAKE_DIR "${METHAN_INSTALL_PREFIX}/cmake")

set(METHAN_THIRD_PARTY_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/third_party")

# Adding source subdirectory
add_subdirectory(src)
if(${METHAN_BUILD_TEST})
    message(STATUS "Importing Catch.......................................")
    add_subdirectory("${METHAN_THIRD_PARTY_FOLDER}/Catch2")
    
    enable_testing()
    add_subdirectory(test)
endif()