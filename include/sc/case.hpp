/*
 * (c) 2020 Henrich Lauko <xlauko@mail.muni.cz>
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

#include <llvm/Support/Casting.h>

namespace sc
{
    template< typename >
    struct ReturnType { };

    template< typename R, typename... Args >
    struct ReturnType< R (*)( Args... ) > { using T = R; };

    template< unsigned i, typename... Empty >
    struct _Get { };

    template< typename _T, typename... Ts >
    struct _Get< 0, _T, Ts... > { using T = _T; };

    template< unsigned i, typename _T, typename... Ts >
    struct _Get< i, _T, Ts... > { using T = typename _Get< i - 1, Ts... >::T; };

    template< unsigned, typename >
    struct MethodArg { };

    template< unsigned i, typename Obj, typename R, typename... Args >
    struct MethodArg< i, R (Obj::*)( Args... ) > {
        using T = typename _Get< i, Args... >::T;
    };

    template< unsigned i, typename Obj, typename R, typename... Args >
    struct MethodArg< i, R (Obj::*)( Args... ) const > {
        using T = typename _Get< i, Args... >::T;
    };

    template< typename What >
    bool llvmcase( What * ) { return false; }

    template< typename What, typename Lambda, typename... Lambdas >
    bool llvmcase( What *w, Lambda lambda, Lambdas &&...lambdas ) {
        if ( auto val = llvm::dyn_cast<
                typename std::remove_pointer<
                    typename MethodArg< 0, decltype( &Lambda::operator() ) >::T
                >::type >( w ) )
        {
            lambda( val );
            return true;
        }
        return llvmcase( w, std::forward< Lambdas >( lambdas )... );
    }

    template< typename What, typename... Lambdas >
    bool llvmcase( What &w, Lambdas &&...lambdas ) {
        return llvmcase( &w, std::forward< Lambdas >( lambdas )... );
    }

}