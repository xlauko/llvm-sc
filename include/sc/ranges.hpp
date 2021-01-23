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

#include <ranges>

namespace sc::views
{
    template< typename F >
    auto map( F &&f ) { return std::views::transform( std::forward< F >( f ) ); }

    static const auto flatten = std::views::join;

    static const auto pointer = [] ( auto &ref ) { return std::addressof(ref); };
    static const auto pointers = map(pointer);

    static const auto notnull = [] ( auto *v ) -> bool { return v != nullptr; };

    template< typename R >
    using frozen = std::vector< std::ranges::range_value_t< R > >;

    inline auto freeze( std::ranges::range auto r ) -> frozen< decltype(r) >
    {
        frozen< decltype(r) > v;
        if constexpr( std::ranges::sized_range< decltype(r) > ) {
            v.reserve( std::ranges::size(r) );
        }

        std::ranges::copy( r, std::back_inserter(v) );
        return v;
    }

    template< typename T >
    struct IsClosure {
        template< typename F >
        bool operator()( F *v ) const { return llvm::isa< T >( v ); }
        template< typename F >
        bool operator()( F &v ) const { return llvm::isa< T >( &v ); }
    };

    template< typename T >
    struct IsNotClosure {
        template< typename F >
        bool operator()( F *v ) const { return !llvm::isa< T >( v ); }
        template< typename F >
        bool operator()( F &v ) const { return !llvm::isa< T >( &v ); }
    };

    template< typename T > const IsClosure< T > is;
    template< typename T > const IsNotClosure< T > isnot;

    struct IsCallClosure
    {
        template< typename F >
        bool operator()( F *v ) const { return static_cast< bool >( llvm::CallSite{ v } ); }
        template< typename F >
        bool operator()( F &v ) const { return static_cast< bool >( llvm::CallSite{ &v } ); }
    };

    static const IsCallClosure is_call;

    template< typename T >
    struct DynCastClosure {
        template< typename F >
        T *operator()( F *f ) const { return llvm::dyn_cast< T >( f ); }
        template< typename F >
        T *operator()( F &f ) const { return llvm::dyn_cast< T >( &f ); }
    };

    template< typename T >
    struct CastClosure {
        template< typename F >
        T *operator()( F *f ) { return llvm::cast< T >( f ); }
        template< typename F >
        T *operator()( F &f ) { return llvm::cast< T >( &f ); }
    };

    template< typename T > const DynCastClosure< T > llvmdyncast;
    template< typename T > const CastClosure< T > llvmcast;

    template< typename T >
    static auto mapdyncast = map( llvmdyncast< T > );

    template< typename T >
    static auto mapcast = map( llvmcast< T > );

    inline auto instructions( llvm::Module &m )
    {
        return m | map( flatten ) | flatten | pointers;
    }

    inline auto instructions( llvm::Function &f )
    {
        return f | flatten | pointers;
    }

    template< typename T, typename LLVM >
    auto filter( LLVM &ll )
    {
        auto r = freeze( sc::views::instructions( ll ) ); // FIXME get rid of one copy
        return freeze( r | std::views::filter( is< T > ) | mapcast< T > );
    }

} // namespace sc::views
