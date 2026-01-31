add_executable(lm main.cpp common/repl.cpp common/file_run.cpp)
target_link_libraries(lm lmc lmvm )