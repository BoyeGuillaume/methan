# Setting up minimum cmake required
cmake_minimum_required(VERSION 3.12)
include(CMakePackageConfigHelpers)

# Discovering all source files that are part of the library
file(GLOB_RECURSE METHAN_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/methan/**.cpp")
message(STATUS "Discovering source files..............................[ OK ]")

# Writing configuration files to the library
message(STATUS "Writing the configuration files.......................[ OK ]")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/configuration.hpp.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/methan/core/configuration.hpp"
)

# Create the library
message(STATUS "Creating the library target...........................")
if(${METHAN_BUILD_SHARED})
    add_library(Methan STATIC SHARED ${METHAN_SOURCE_FILES})
    message("   > Shared")
    set(METHAN_BUILD_MODE SHARED)

    # Adding the Methan
    get_dynamic_lib_name(_METHAN_DYNAMIC_LIB_NAME "Methan")
    list(APPEND METHAN_INTERNAL_SHARED_LIST ${_METHAN_DYNAMIC_LIB_NAME})
    set(METHAN_INTERNAL_SHARED_LIST ${METHAN_INTERNAL_SHARED_LIST} PARENT_SCOPE)

else()
    add_library(Methan STATIC ${METHAN_SOURCE_FILES})
    message("   > Static")
    set(METHAN_BUILD_MODE STATIC)
endif()


# Configure dependencies of the Methan library
if(WIN32)
    target_link_libraries(Methan PRIVATE spdlog::spdlog wsock32 ws2_32)
else()
    target_link_libraries(Methan PRIVATE spdlog::spdlog)
endif()

# Configuring internal variable used throughout the project
set(METHAN_INTERNAL_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)

# Configuring the build
message(STATUS "Configuration of the build............................")
target_include_directories(Methan PRIVATE "${METHAN_INTERNAL_INCLUDE_DIRECTORY}" "${METHAN_THIRD_PARTY_FOLDER}/Brynet/include/")
target_compile_definitions(Methan PRIVATE "METHAN_EXPORT")

# Configuring the library
message(STATUS "Configuration of the library..........................")
set_target_properties(Methan PROPERTIES
    LINKER_LANGUAGE CXX
    SOVERSION ${METHAN_VERSION_MAJOR}.${METHAN_VERSION_MINOR})

# Setup the install target
message(STATUS "Setting up the installation target....................")
message("   > Install directory: ${METHAN_INSTALL_PREFIX}/")
install(TARGETS Methan
        # EXPORT  MethanTargets
        ARCHIVE DESTINATION ${METHAN_INSTALL_LIBRARY_DIR}
        LIBRARY DESTINATION ${METHAN_INSTALL_LIBRARY_DIR}
        RUNTIME DESTINATION ${METHAN_INSTALL_BINARY_DIR})      
        
# Install include/
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/methan"
        DESTINATION "${METHAN_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING
        PATTERN "**.hpp"
        PATTERN "**.inl"
        PATTERN "methan/private" EXCLUDE)

if(${METHAN_EXPOSE_PRIVATE})
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/methan"
        DESTINATION "${METHAN_INSTALL_INCLUDE_DIR}"
        FILES_MATCHING
        PATTERN "**.hpp"
        PATTERN "**.inl"
        PATTERN "methan/private")
endif()

# Install all dll EXCEPT the Methan dll (already done)
foreach(_DynamicLib ${METHAN_INTERNAL_SHARED_LIST})
    install(FILES "${METHAN_INTERNAL_BINARY_DIRECTORY}/${_DynamicLib}"
            DESTINATION "${METHAN_INSTALL_BINARY_DIR}")
endforeach()

# Install cmake(s) file(s) in order to make it a cmake package
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MethanConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MethanConfig.cmake"
    INSTALL_DESTINATION "${METHAN_INSTALL_CMAKE_DIR}"
    PATH_VARS METHAN_INSTALL_INCLUDE_DIR METHAN_INSTALL_LIBRARY_DIR METHAN_INSTALL_BINARY_DIR)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/MethanConfig.cmake"
              "${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindMethan.cmake"
        DESTINATION "${METHAN_INSTALL_CMAKE_DIR}")

