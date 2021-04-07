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

#include <llvm/IR/Type.h>
#include <llvm/IR/DataLayout.h>

#include <sc/ir.hpp>

namespace sc
{
    using type     = llvm::Type *;
    using value    = llvm::Value *;
    using int_type = llvm::IntegerType *;
    using ptr_type = llvm::PointerType *;
    using data_layout_t = llvm::DataLayout;

    type void_t();

    int_type ii( unsigned n );

    int_type i1();
    int_type i8();
    int_type i16();
    int_type i32();
    int_type i64();
    int_type i128();

    ptr_type iip( unsigned n, unsigned as = 0 );

    ptr_type i1p  ( unsigned as = 0 );
    ptr_type i8p  ( unsigned as = 0 );
    ptr_type i16p ( unsigned as = 0 );
    ptr_type i32p ( unsigned as = 0 );
    ptr_type i64p ( unsigned as = 0 );

    unsigned bits ( type ty, const data_layout_t &dl );
    unsigned bytes( type ty, const data_layout_t &dl );
    
    unsigned bits ( value v );
    unsigned bytes( value v );

} // namespace sc
