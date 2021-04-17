/*
 * (c) 2021 Henrich Lauko <xlauko@mail.muni.cz>
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

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/CallSite.h>

#include <range/v3/core.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/algorithm/copy.hpp>
#include <range/v3/range/conversion.hpp>

namespace sc::views
{
    template< typename... Ts > struct overloaded : Ts... { using Ts::operator()...; };
    template< typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template< typename T >
    auto is = overloaded {
        []( auto *v )  { return llvm::isa< T >(  v ); },
        []( auto &&v ) { return llvm::isa< T >( &v ); }
    };
    
    template< typename T >
    auto isnot = overloaded {
        []( auto *v )  { return !llvm::isa< T >(  v ); },
        []( auto &&v ) { return !llvm::isa< T >( &v ); }
    };

} // namespace sc::views
