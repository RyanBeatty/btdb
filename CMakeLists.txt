cmake_minimum_required(VERSION 3.0.0)
project(btdb VERSION 0.1.0)


#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 17)


add_subdirectory(src)
add_subdirectory(libs)
add_subdirectory(apps)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)

    if(BUILD_TESTING)
        add_subdirectory(test)
    endif()
endif()
