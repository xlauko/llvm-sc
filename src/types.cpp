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

#include <sc/types.hpp>
#include <sc/context.hpp>

#include <llvm/IR/Type.h>

namespace sc
{
    using Type = llvm::Type;

    type void_t() { return Type::getVoidTy( context() ); }

    int_type ii( unsigned n ) { return Type::getIntNTy( context(), n ); }

    int_type i1()   { return Type::getInt1Ty  ( context() ); }
    int_type i8()   { return Type::getInt8Ty  ( context() ); }
    int_type i16()  { return Type::getInt16Ty ( context() ); }
    int_type i32()  { return Type::getInt32Ty ( context() ); }
    int_type i64()  { return Type::getInt64Ty ( context() ); }
    int_type i128() { return Type::getInt128Ty( context() ); }

    ptr_type iip( unsigned n, unsigned as )
    {
        return Type::getIntNPtrTy( context(), n, as );
    }

    ptr_type i1p   ( unsigned as ) { return Type::getInt1PtrTy( context(), as ); }
    ptr_type i8p   ( unsigned as ) { return Type::getInt8PtrTy( context(), as ); }
    ptr_type i16p  ( unsigned as ) { return Type::getInt16PtrTy( context(), as ); }
    ptr_type i32p  ( unsigned as ) { return Type::getInt32PtrTy( context(), as ); }
    ptr_type i64p  ( unsigned as ) { return Type::getInt64PtrTy( context(), as ); }

    unsigned bits( type ty, const data_layout_t &dl )
    {
        return unsigned( dl.getTypeSizeInBits( ty ).getFixedSize() );
    }

    unsigned bytes( type ty, const data_layout_t &dl )
    {
        return ( bits( ty, dl ) + 7 ) / 8;
    }

    unsigned bits( value v )
    {
        return bits( v->getType(), data_layout( v ) );
    }

    unsigned bytes( value v )
    {
        return bytes( v->getType(), data_layout( v ) );
    }

} // namespace sc
