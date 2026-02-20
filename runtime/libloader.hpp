#pragma once
#ifndef TARGET_WASM
#include "lmx_export.hpp"
#include <cstdint>
#include <string>

#include <unordered_map>
#include <vector>
#ifdef _WIN32

#include <windows.h>
#else
#include <dlfcn.h>
#endif


namespace lmx::runtime {
class VirtualCore;
enum CBasicTypes : uint8_t {
    Void, Char, Short, Int, LongLong, Float, Double, Ptr, Bool,
    NO_ENUM_VALUE
};

inline auto dn_prefix =
#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
       "lib";
#else
       "";
#endif
inline auto dn_suffix =
#if defined(__APPLE__) && defined(__MACH__)
       ".dylib";
#elif defined(__linux__)
       ".so";
#elif defined(_WIN32) || defined(_WIN64)
       ".dll";
#else
                "";
#endif
extern size_t sizeof_ctypes[NO_ENUM_VALUE];
extern const char* ctypes_str[NO_ENUM_VALUE];

struct DynFunc {
    void* func;
    CBasicTypes ret_type;
    uint8_t arg_count;
    std::vector<CBasicTypes> arg_type;

    void call(VirtualCore* vm) const;
    [[nodiscard]] size_t max_size() const;
};

struct DynLib {
    std::string name;
    void* handle;
    std::unordered_map<const char*, DynFunc> funcs;

    explicit DynLib(const std::string& name);
    ~DynLib() noexcept{
        if (handle) {
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
        }
    }


    void set_func(const char* n, std::vector<CBasicTypes> args_type, CBasicTypes ret_type);
    bool contain(const char* n) const;
    const DynFunc* find(const char* n) const;
    void call(const char* n, VirtualCore* vm) const;
};


}
#endif
