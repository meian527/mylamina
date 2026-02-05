add_executable(lm main.cpp common/repl.cpp common/file_run.cpp)
target_link_libraries(lm lmc lmvm )

# Windows 需要额外库
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# 链接系统库
if(WIN32)
    target_link_libraries(lm ole32 shell32)  # 用于SHGetKnownFolderPath
endif()