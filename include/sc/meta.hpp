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

#include <llvm/IR/Metadata.h>
#include <optional>

namespace sc::meta
{
    using node_t = llvm::MDNode *;

    using meta_str       = llvm::StringRef;
    using maybe_meta_str = std::optional< meta_str >;

    using tag_t  = llvm::StringRef;
    using ctag_t = llvm::StringLiteral;

    namespace tag
    {
        constexpr ctag_t none      = "sc.meta.none";
        constexpr ctag_t arguments = "sc.meta.arguments";
    } // namespace tag

    node_t node( meta_str str );

    maybe_meta_str get_string( node_t n );

    void set( llvm::Value *val, tag_t tag, meta_str meta = tag::none );

    maybe_meta_str get( llvm::Value *val, tag_t tag );

    struct tuple
    {
        using meta_array = llvm::ArrayRef< llvm::Metadata * >;

        static inline llvm::MDTuple *create( const meta_array &arr );

        template< typename Init >
        static inline llvm::MDTuple *create( unsigned size, Init init );
    };

    struct argument
    {
        static void set( llvm::Argument *arg, meta_str str );
        static void set( llvm::Argument *arg, node_t node );
        static maybe_meta_str get( llvm::Argument *arg );
    };

} // namespace sc::meta

#include <sc/meta.tpp>
