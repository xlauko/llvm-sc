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

#include <llvm/IR/Constants.h>

#include <concepts>

namespace sc
{
    using constant = llvm::Constant;
    using constant_int = llvm::ConstantInt;

    constant_int* i1 ( bool );
    constant_int* i8 ( uint8_t );
    constant_int* i16( uint16_t );
    constant_int* i32( uint32_t );
    constant_int* i64( uint64_t );

    inline auto i_( bool v )     { return i1( v ); }
    inline auto i_( uint8_t v  ) { return i8( v ); }
    inline auto i_( uint16_t v ) { return i16( v ); }
    inline auto i_( uint32_t v ) { return i32( v ); }
    inline auto i_( uint64_t v ) { return i64( v ); }

    inline auto iv( std::integral auto v ) { return i_( v ); }

    using apint = llvm::APInt;

    const apint& value( constant_int * c );

    namespace literals
    {
        inline constant_int * operator "" _i8( unsigned long long v )  { return i8( v ); }
        inline constant_int * operator "" _i16( unsigned long long v ) { return i16( v ); }
        inline constant_int * operator "" _i32( unsigned long long v ) { return i32( v ); }
        inline constant_int * operator "" _i64( unsigned long long v ) { return i64( v ); }
    }
}
