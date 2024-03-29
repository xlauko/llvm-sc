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

#include <sc/generator.hpp>
#include <llvm/IR/Module.h>
#include <sc/transformer.hpp>
#include <string_view>

namespace sc
{
    struct annotation;

    template< typename Value >
    using llvm_value =
        typename std::conditional< std::is_pointer_v< Value >, Value, Value * >::type;

    template< typename Value >
    using annotated = std::pair< llvm_value< Value >, annotation >;

    struct annotation
    {
        using parts_t        = std::vector< std::string >;
        using iterator       = parts_t::iterator;
        using const_iterator = parts_t::const_iterator;

        template< typename... parts_t > constexpr explicit annotation( parts_t... parts )
        {
            ( _parts.push_back( parts ), ... );
        }

        explicit annotation( llvm::StringRef anno )
        {
            size_t oldoff = 0, off = 0;
            do {
                    off = anno.find( '.', oldoff );
                    _parts.emplace_back( anno.substr( oldoff, off - oldoff ) );
                    oldoff = off + 1;
            } while ( off != std::string::npos );
        }

        annotation( const_iterator begin, const_iterator end ) : _parts( begin, end ) {}

        [[nodiscard]] const std::string &back() const;
        [[nodiscard]] std::string str() const;

        [[nodiscard]] size_t size() const;

        [[nodiscard]] std::string_view name() const;
        [[nodiscard]] annotation get_namespace() const;

        [[nodiscard]] bool in_namespace( const annotation &ns ) const;

        bool operator==( const annotation & ) const = default;

        parts_t _parts;

        template< typename Value >
        static sc::generator< annotated< Value > > enumerate( llvm::Module &m );

        template< typename Value >
        static sc::generator< annotated< Value > > enumerate_in_namespace( annotation ns,
                                                                       llvm::Module &m );

        template< typename ostream >
        friend auto operator<<( ostream &o, const annotation &ann ) -> decltype( o << "" )
        {
            return o << ann.str();
        }
    };

} // namespace sc

#include "annotation.tpp"
