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

#include <sc/context.hpp>
#include <sc/meta.hpp>

namespace sc::meta
{
    node create( meta_str str )
    {
        auto &ctx = context();
        return llvm::MDNode::get( ctx, llvm::MDString::get( ctx, str ) );
    }

    maybe_meta_str get_string( node n )
    {
        if ( !n || !n->getNumOperands() ) return std::nullopt;

        const auto& str = n->getOperand( 0 );
        auto res  = llvm::cast< llvm::MDString >( str )->getString().str();
        if ( res.empty() ) return std::nullopt;
        return res;
    }

} // namespace sc::meta
