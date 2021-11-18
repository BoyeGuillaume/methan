# Setting up minimum cmake required
cmake_minimum_required(VERSION 3.12)

# 
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)

# Discoverring test
message(STATUS "Discovering tests.....................................[ DONE ]")
file(GLOB_RECURSE METHAN_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/methan/**.cpp")

# Add the build target for each test
message(STATUS "Building test")
foreach(TEST_FILE ${METHAN_TEST_FILES})
    get_filename_component(EXECUTABLE_NAME ${TEST_FILE} NAME_WE)

    add_executable(${EXECUTABLE_NAME} ${TEST_FILE})

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE Catch2::Catch2WithMain Methan)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${METHAN_INTERNAL_INCLUDE_DIRECTORY})

    if(${METHAN_BUILD_SHARED})
        if(WIN32)
            set(METHAN_PLATFORM_SPECIFIC_DYNAMIC_LIBRARY "Methan.dll")
        elseif(UNIX AND NOT APPLE)
            set(METHAN_PLATFORM_SPECIFIC_DYNAMIC_LIBRARY "libMethan.so")
        elseif(APPLE)
            set(METHAN_PLATFORM_SPECIFIC_DYNAMIC_LIBRARY "libMethan.dylib")
        else()
            message(FATAL_ERROR "Unknow platform")
        endif()

        add_custom_command(TARGET ${EXECUTABLE_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    "\"${METHAN_INTERNAL_BINARY_DIRECTORY}/${METHAN_PLATFORM_SPECIFIC_DYNAMIC_NAME}\""
                    "\"${CMAKE_CURRENT_BINARY_DIR}/${METHAN_PLATFORM_SPECIFIC_DYNAMIC_NAME}\"")
    endif()

    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}")

    add_test(NAME ${EXECUTABLE_NAME}
             COMMAND ${EXECUTABLE_NAME}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()
