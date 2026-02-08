cmake_minimum_required(VERSION 3.15)
project(LMX)

set(CMAKE_CXX_STANDARD 23)

# Add MSVC compatibility support
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc /utf-8")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
    endif()
endif ()

# Define DLL export macros
if(BUILD_SHARED_LIBS OR WIN32)
    add_definitions(-DLMX_DLL)
endif()
include_directories(include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(compiler)
add_subdirectory(runtime)

add_subdirectory(tools/lm)

