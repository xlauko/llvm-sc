#pragma once

#include <sc/config.hpp>

#ifdef SC_COROHEADER_FOUND_AND_USABLE

    #include <coroutine>

namespace sc
{
    using std::coroutine_handle;
    using std::noop_coroutine;
    using std::suspend_always;
    using std::suspend_never;

} // namespace sc

#elif __has_include(<experimental/coroutine>)

    #include <experimental/coroutine>

namespace sc
{
    using std::experimental::coroutine_handle;
    using std::experimental::suspend_always;
    using std::experimental::suspend_never;

    #if SC_COMPILER_SUPPORTS_SYMMETRIC_TRANSFER
    using std::experimental::noop_coroutine;
    #endif
} // namespace sc

#else
    #error sc requires a C++20 compiler with coroutine support
#endif