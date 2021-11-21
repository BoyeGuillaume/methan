# Setting up minimum cmake required
cmake_minimum_required(VERSION 3.12)

# 
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)

# Discoverring test
message(STATUS "Discovering tests.....................................[ DONE ]")
file(GLOB_RECURSE METHAN_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/methan/**.cpp")

# Add custom target for the copy for all files
foreach(_SHARED_LIBS ${METHAN_INTERNAL_SHARED_LIST})
    add_custom_target(METHAN_INTERNAL_TARGET_TEST_COPY_${_SHARED_LIBS} ALL
        ${CMAKE_COMMAND} -E copy
            "\"${METHAN_INTERNAL_BINARY_DIRECTORY}/${_SHARED_LIBS}\""
            "\"${CMAKE_CURRENT_BINARY_DIR}/${_SHARED_LIBS}\"") # This little hack ensure this command is always run
    add_dependencies(METHAN_INTERNAL_TARGET_TEST_COPY_${_SHARED_LIBS} Methan)
    add_dependencies(METHAN_INTERNAL_TARGET_TEST_COPY_${_SHARED_LIBS} spdlog)
endforeach()


# Add the build target for each test
message(STATUS "Building test")
foreach(TEST_FILE ${METHAN_TEST_FILES})
    get_filename_component(EXECUTABLE_NAME ${TEST_FILE} NAME_WE)

    add_executable(${EXECUTABLE_NAME} ${TEST_FILE})

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE Catch2::Catch2WithMain Methan spdlog::spdlog)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${METHAN_INTERNAL_INCLUDE_DIRECTORY})

    foreach(_SHARED_LIBS ${METHAN_INTERNAL_SHARED_LIST})
        add_dependencies(${EXECUTABLE_NAME} METHAN_INTERNAL_TARGET_TEST_COPY_${_SHARED_LIBS})
    endforeach()
    
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
