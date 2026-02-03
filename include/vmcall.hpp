#pragma once
#include "../runtime/vm.hpp"


namespace lmx::runtime {
class VMCall {
#define VMCALL_INDEX_MAX UINT16_MAX
#define VMC_REGISTER(name) static void lm_##name(lmx::runtime::VirtualCore* self)
    VMC_REGISTER(out);
    VMC_REGISTER(in);
    VMC_REGISTER(exit);
    VMC_REGISTER(open_file);
    VMC_REGISTER(close);
    VMC_REGISTER(read);
    VMC_REGISTER(write);
    VMC_REGISTER(dyn_load);
    VMC_REGISTER(dyn_call);
#undef VMC_REGISTER
#define VMC_REGISTER(name) void lmx::runtime::VMCall::lm_##name(lmx::runtime::VirtualCore* self)
public:
    using VmCallType = void (*)(VirtualCore* self);

    static inline VmCallType vmcall_table[] = {
        lm_out, lm_in, lm_exit,
    };

    static inline uint16_t vmcall_count = sizeof(vmcall_table) / sizeof(VmCallType);
};
}

