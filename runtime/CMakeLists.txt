set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE RUNTIME_SRC *.cpp */*.cpp)
add_library(lmvm SHARED ${RUNTIME_SRC})

# Define build macro for DLL export
if(WIN32)
    target_compile_definitions(lmvm PRIVATE LMVM_BUILD)
endif()

# Include common headers
target_include_directories(lmvm PUBLIC ${CMAKE_SOURCE_DIR}/include)
