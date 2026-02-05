#include "libloader.hpp"
#include "../compiler/generator/generator.hpp"
#include <iostream>
#include "dyncall/dyncall.h"
#include <string>
#include "../runtime/vm.hpp"


namespace lmx::runtime {

// 初始化全局数组
size_t sizeof_ctypes[NO_ENUM_VALUE] = {
    0, sizeof(char), sizeof(short), sizeof(int), sizeof(long long), 
    sizeof(float), sizeof(double), sizeof(void*), sizeof(bool)
};

const char* ctypes_str[NO_ENUM_VALUE] = {
    "void", "char", "short", "int", "long", "float", "double", "", "_Bool"
};

// DynFunc 成员函数实现
void DynFunc::call(VirtualCore* vm) const {
    const auto caller = dcNewCallVM(max_size());
    dcMode(caller, DC_CALL_C_DEFAULT);
    dcReset(caller);
    
    size_t reg = REG_COUNT_INDEX_MAX;
    for (const auto& kv : arg_type) {
        switch (kv) {
        case Void:      dcArgPointer(caller, vm->get_register(reg).ptr); break;
        case Char:      dcArgChar(caller, vm->get_register(reg).i8); break;
        case Short:     dcArgShort(caller, vm->get_register(reg).i16); break;
        case Int:       dcArgInt(caller, vm->get_register(reg).i32); break;
        case LongLong:  dcArgLongLong(caller, vm->get_register(reg).i64); break;
        case Float:     dcArgFloat(caller, vm->get_register(reg).f32); break;
        case Double:    dcArgDouble(caller, vm->get_register(reg).f64); break;
        case Ptr:       dcArgPointer(caller, vm->get_register(reg).ptr); break;
        case Bool:      dcArgBool(caller, vm->get_register(reg).i8); break;
        default:break;
        }
        reg--;
    }
    
    switch (ret_type) {
    case Void:      dcCallVoid(caller, func); break;
    case Char:      vm->get_register(0).i8  = dcCallChar(caller, func); break;
    case Short:     vm->get_register(0).i16 = dcCallShort(caller, func); break;
    case Int:       vm->get_register(0).i32 = dcCallInt(caller, func); break;
    case LongLong:  vm->get_register(0).i64 = dcCallLongLong(caller, func); break;
    case Float:     vm->get_register(0).f32 = dcCallFloat(caller, func); break;
    case Double:    vm->get_register(0).f64 = dcCallDouble(caller, func); break;
    case Ptr:       vm->get_register(0).ptr = dcCallPointer(caller, func); break;
    case Bool:      vm->get_register(0).b   = dcCallBool(caller, func); break;
    default:break;
    }
    
    dcFree(caller);
}

size_t DynFunc::max_size() const {
    size_t size =
#if _WIN32 || _WIN64
        32;
#else
        0;
#endif
    for (const auto& at : arg_type)
        size += 8;
    return size;
}

// DynLib 成员函数实现
DynLib::DynLib(const std::string& name) : name(dn_prefix + name + dn_suffix) {

#ifdef _WIN32
    handle = LoadLibraryA(this->name.c_str());
#else
    handle = dlopen(this->name.c_str(), RTLD_LAZY | RTLD_GLOBAL);
#endif
    if (!handle) {
        std::cerr << "error: cannot load lib: " << this->name << std::endl;
        exit(1);
    }
}



void DynLib::set_func(const char* n, std::vector<CBasicTypes> args_type, CBasicTypes ret_type) {
    if (!funcs.contains(n)) {
        funcs.insert({n, DynFunc{
#ifdef _WIN32
            (void*)GetProcAddress(static_cast<HMODULE>(handle), n),
#else
            dlsym(handle, n),
#endif
            ret_type, static_cast<uint8_t>(args_type.size()), std::move(args_type)
        }});
    }
}

bool DynLib::contain(const char* n) const {
    return funcs.contains(n);
}

const DynFunc* DynLib::find(const char* n) const {
    const auto it = funcs.find(n);
    if (it == funcs.end()) {
        std::cerr << "the func `" << n << "` is not found in lib `" << name << "`" << std::endl;
        return nullptr;
    }
    return &it->second;
}

void DynLib::call(const char* n, VirtualCore* vm) const {
    const auto f = find(n);
    if (f != nullptr) {
        f->call(vm);
    }
}

} // namespace lmx::runtime