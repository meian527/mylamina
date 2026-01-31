#include "common/file_run.hpp"
#include "common/repl.hpp"
#include <string>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        return run_repl();
    }
    return file_run(argv[1]);
}
