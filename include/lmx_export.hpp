#pragma once

#ifdef LMX_DLL
    #ifdef _WIN32
        #ifdef LMC_BUILD
            #define LMC_API __declspec(dllexport)
        #else
            #define LMC_API __declspec(dllimport)
        #endif
        
        #ifdef LMVM_BUILD
            #define LMVM_API __declspec(dllexport)
        #else
            #define LMVM_API __declspec(dllimport)
        #endif
    #else
        #define LMC_API
        #define LMVM_API
    #endif
#else
    #define LMC_API
    #define LMVM_API
#endif
