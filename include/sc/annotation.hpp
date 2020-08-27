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

#include <coroutine>
#include <external/coro/generator.hpp>
#include <llvm/IR/Module.h>
#include <sc/transformer.hpp>

namespace sc
{
    struct annotation
    {
        template< typename... parts_t > constexpr explicit annotation( parts_t... parts )
        {
            ( _parts.push_back( parts ), ... );
        }

        [[nodiscard]] const std::string &back() const;

        [[nodiscard]] std::string str() const;

        bool operator==( const annotation & ) const = default;

        std::vector< std::string > _parts;
    };

    template< typename T > using generator = cppcoro::generator< T >;

    template< typename Value >
    generator< annotation > enumerate( llvm::Module &m )
    {
        co_yield annotation( "test" );
    }

} // namespace sc
