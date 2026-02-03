#include "vmcall.hpp"
#include <cstdlib>

#include "../compiler/generator/generator.hpp"




VMC_REGISTER(out) {
    fputs(
        static_cast<const char *>(self->get_constant())
        +
        self->get_register(REG_COUNT_INDEX_MAX).i32,

        stdout);
}

VMC_REGISTER(in) {
    fgets(self->get_register(0).str, 1024, stdin);
}
VMC_REGISTER(exit) {
    exit(self->get_register(REG_COUNT_INDEX_MAX).i32);
}

