/*
 * (c) 2022 Henrich Lauko <xlauko@mail.muni.cz>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include "config.hpp"

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
