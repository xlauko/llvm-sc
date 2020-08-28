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

namespace sc
{
    template< typename Value >
    auto annotation::enumerate( llvm::Module &m ) -> generator< annotation >
    {
        using const_array = llvm::ConstantArray;
        const auto annos  = m.getNamedGlobal( "llvm.global.annotations" );

        if ( auto arr = transformer( annos ).operand( 0 ).template cast< const_array >().freeze() )
        {
            for ( const auto &op : arr->operands() ) {
                auto cs = transformer( op.get() ).template cast< llvm::ConstantStruct >();
                if ( auto val = cs.operand( 0 ).operand( 0 ).template cast< Value >().freeze() )
                {
                    auto anno = cs.operand( 1 )
                        .operand( 0 )
                        .template cast< llvm::GlobalVariable >()
                        .operand( 0 )
                        .template cast< llvm::ConstantDataArray >()
                        .freeze();
                    co_yield annotation( anno->getAsCString() );
                }
            }
        }
    }

    template< typename Value >
    auto annotation::enumerate_in_namespace( annotation ns, llvm::Module &m )
        -> generator< annotation >
    {
        for ( auto &&ann : enumerate< Value >( m ) )
            if ( ann.in_namespace( ns ) )
                co_yield( ann );
    }

} // namespace sc
