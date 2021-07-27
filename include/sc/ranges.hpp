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

#include <ranges>
#include <algorithm>

namespace sc::views
{
    template< typename... Ts > struct overloaded : Ts... { using Ts::operator()...; };
    template< typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template< std::ranges::range R >
    auto to_vector(R&& r) {
        auto r_common = r | std::views::common;
        return std::vector(r_common.begin(), r_common.end());
    }

    using llvmmodule = llvm::Module;
    using function   = llvm::Function;
    using basicblock = llvm::BasicBlock;

    template< typename T >
    auto is = overloaded {
        []( auto  *v ) { return llvm::isa< T >(  v ); },
        []( auto &&v ) { return llvm::isa< T >( &v ); }
    };

    template< typename T >
    auto isnot = overloaded {
        []( auto  *v ) { return !llvm::isa< T >(  v ); },
        []( auto &&v ) { return !llvm::isa< T >( &v ); }
    };

    template< typename T >
    auto dyncast = overloaded {
        []( auto  *v ) { return llvm::dyn_cast< T >(  v ); },
        []( auto &&v ) { return llvm::dyn_cast< T >( &v ); }
    };

    template< typename T >
    auto cast = overloaded {
        []( auto  *v ) { return llvm::cast< T >(  v ); },
        []( auto &&v ) { return llvm::cast< T >( &v ); }
    };

    static const auto notnull = [] ( auto *v ) -> bool { return v != nullptr; };
    static const auto type = [] ( auto *val ) { return val->getType(); };

    // map
    static const auto pointer = [] ( auto &ref ) { return std::addressof(ref); };
    static const auto pointers = std::views::transform( pointer );

    static const auto types = std::views::transform( type );

    template< typename T >
    static const auto mapdyncast = std::views::transform( dyncast< T > );

    template< typename T >
    static auto mapcast = std::views::transform( cast< T > );

    // filter
    template< typename T >
    static const auto filter_range = mapdyncast< T > | std::views::filter( notnull );

    static const auto instructions = overloaded {
        []( basicblock *bb ) { return *bb | pointers; },
        []( basicblock &bb ) { return  bb | pointers; },
        []( function   *fn ) { return *fn | std::views::join | pointers; },
        []( function   &fn ) { return  fn | std::views::join | pointers; },
        []( llvmmodule  *m ) { return  *m | std::views::join | std::views::join | pointers; },
        []( llvmmodule  &m ) { return   m | std::views::join | std::views::join | pointers; },
    };

    static const auto basicblocks = overloaded {
        []( function *fn ) { return std::views::all(*fn); },
        []( function &fn ) { return std::views::all( fn); }
    };

    template< typename T >
    auto filter( auto &llvm ) {
        return instructions( llvm ) | filter_range< T >;
    }


} // namespace sc::views
