cmake_minimum_required(VERSION 3.0.0)
project(btdb VERSION 0.1.0)


#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 17)

add_subdirectory(cmake)
add_subdirectory(src)
add_subdirectory(libs)
add_subdirectory(apps)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)

    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()

# Custom make command to reset the data directory
add_custom_target(reset_data_dir
    COMMAND rm -rf data_dir && mkdir data_dir
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Custom make command to compile and run btdb executable. 
add_custom_target(run_btdb
    COMMAND ./build/apps/btdb
    # Need to run from project root so that we can find data directory.
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS btdb
)
