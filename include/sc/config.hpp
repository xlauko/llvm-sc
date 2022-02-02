#pragma once

// Compiler Detection

#if defined(_MSC_VER)
    #define SC_COMPILER_MSVC _MSC_FULL_VER
#else
    #define SC_COMPILER_MSVC 0
#endif

#if defined(__clang__)
    #define SC_COMPILER_CLANG \
        (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#else
    #define SC_COMPILER_CLANG 0
#endif

#if defined(__GNUC__)
    #define SC_COMPILER_GCC (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
    #define SC_COMPILER_GCC 0
#endif

#if SC_COMPILER_CLANG
    #if __clang_major__ >= 7
        #define SC_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER 1
    #endif
#endif
#ifndef SC_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER
    #define SC_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER 0
#endif

#if SC_COMPILER_MSVC
    #if __has_include(<coroutine>)
        #include <yvals_core.h>
        #ifdef __cpp_lib_coroutine
            #define SC_COROHEADER_FOUND_AND_USABLE
        #endif
    #endif
#else
    #if __has_include(<coroutine>)
        #define SC_COROHEADER_FOUND_AND_USABLE
    #endif
#endif