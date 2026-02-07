cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
file(GLOB_RECURSE RUNTIME_SRC *.cpp */*.cpp)
add_library(lmvm SHARED ${RUNTIME_SRC})

# Define build macro for DLL export
if(WIN32)
    target_compile_definitions(lmvm PRIVATE LMVM_BUILD)
endif()
# 启用 FetchContent
include(FetchContent)

find_program(HG_EXECUTABLE hg)
if (NOT HG_EXECUTABLE)
    message(FATAL_ERROR
            "'Mercurial' no installed
            please go to 'https://www.mercurial-scm.org/' or 'pip install mercurial'(if you installed python)
")
else ()
    message(STATUS "downloading dyncall...")
endif ()
FetchContent_Declare(
        dyncall
        HG_REPOSITORY https://dyncall.org/pub/dyncall/dyncall
        HG_TAG        default
        OVERRIDE_FIND_PACKAGE
        # 禁用更新检查
        UPDATE_DISCONNECTED ON
)
target_include_directories(lmvm PRIVATE ${CMAKE_BINARY_DIR}/_deps/dyncall-src)
FetchContent_MakeAvailable(dyncall)

target_include_directories(lmvm PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(lmvm dyncall_s dyncallback_s dynload_s)
