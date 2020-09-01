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

#include <experimental/type_traits>
#include <llvm/Support/Casting.h>
#include <type_traits>
#include <utility>

namespace sc
{
    template< typename T >
    using get_operand_t =
        decltype( std::declval< T & >().getOperand( std::declval< int >() ) );

    template< typename Value > struct LLVMTransformer
    {
        using value_type =
            typename std::conditional< std::is_pointer_v< Value >, Value, Value * >::type;

        explicit constexpr LLVMTransformer( const value_type &val ) : state( val ) {}
        explicit constexpr LLVMTransformer( value_type &&val ) : state( std::move( val ) )
        {}

        template< typename F > constexpr decltype( auto ) apply( F f ) noexcept
        {
            using R = decltype( f( state ) );
            if ( state ) return LLVMTransformer< R >( f( state ) );
            return LLVMTransformer< R >( nullptr );
        }

        constexpr decltype( auto ) operand( std::uint32_t idx ) noexcept
        {
            constexpr auto detected =
                std::experimental::is_detected_v< get_operand_t,
                                                  std::remove_pointer_t< Value > >;
            if constexpr ( detected ) {
                    return apply(
                        [ idx ]( const auto &v ) { return v->getOperand( idx ); } );
            } else {
                    return LLVMTransformer< value_type >( nullptr );
                }
        }

        template< typename T > constexpr decltype( auto ) cast() noexcept
        {
            return apply( []( const auto &v ) { return llvm::dyn_cast< T >( v ); } );
        }

        constexpr value_type freeze() noexcept
        {
            return state;
        }

    private:
        value_type state;
    };

    template< typename T > decltype( auto ) transformer( T val )
    {
        return LLVMTransformer< T >( val );
    }

} // namespace sc
