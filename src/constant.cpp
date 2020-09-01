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

#include <sc/constant.hpp>
#include <sc/types.hpp>

namespace sc
{
    constant_int* i1 ( bool v )     { return constant_int::get(  i1(), v ); }
    constant_int* i8 ( uint8_t  v ) { return constant_int::get(  i8(), v ); }
    constant_int* i16( uint16_t v ) { return constant_int::get( i16(), v ); }
    constant_int* i32( uint32_t v ) { return constant_int::get( i32(), v ); }
    constant_int* i64( uint64_t v ) { return constant_int::get( i64(), v ); }

    const apint& get_value( constant_int * c ) { return c->getValue(); }
} // namespace sc
