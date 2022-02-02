/*
 * (c) 2021 Henrich Lauko <xlauko@mail.muni.cz>
 * (c) 2019 Petr Ročkai <code@fixp.eu>
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

#include <range/v3/range/primitives.hpp>

#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <experimental/iterator>

#include <range/v3/range_fwd.hpp>
#include <range/v3/view/transform.hpp>

namespace sc::fmt
{
    using split_view = std::pair< std::string_view, std::string_view >;
    
    namespace ranges = ::ranges;
    
    static inline split_view split( std::string_view p, char d, bool reverse = false ) noexcept
    {
        auto s = reverse ? p.rfind( d ) : p.find( d );
        if ( s == p.npos )
            return reverse ? split_view{ "", p } : split_view{ p, "" };
        auto q = p;
        q.remove_prefix( s + 1 );
        p.remove_suffix( p.size() - s );
        return { p, q };
    }

    template< typename gen_t, typename state_t > struct generator
    {
        using value_type = std::tuple_element_t< 0, ranges::result_of_t< gen_t( state_t ) > >;
        gen_t _gen;
        state_t _initial;

        struct proxy
        {
            value_type v;
            value_type *operator->() { return &v; }
        };

        struct iterator
        {
            gen_t _gen;
            state_t _state;

            iterator( gen_t g, state_t s ) : _gen( g ), _state( s ) {}
            iterator &operator=( const iterator & ) = default;
            iterator &operator++() { _state = std::get< 1 >( _gen( _state ) ); return *this; }
            iterator operator++( int ) { auto x = *this; ++*this; return x; }
            value_type operator*() { return std::get< 0 >( _gen( _state ) ); }
            proxy operator->() { return { **this }; }
            bool operator==( const iterator &o ) const { return _state == o._state; }
            bool operator!=( const iterator &o ) const { return _state != o._state; }
        };

        auto begin() const { return iterator( _gen, _initial ); }
        auto end()   const { return iterator( _gen, state_t() ); }

        generator( gen_t gen, state_t init ) : _gen( gen ), _initial( init ) {}
    };

    static auto splitter( std::string_view s, char d, bool reverse = false )
    {
        return generator( [=]( auto _s ) { return split( _s, d, reverse ); }, s );
    }

    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    // trim from end (in place)
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // trim from both ends (in place)
    inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    template< typename T >
    concept llvm_printable = requires(T *v) {
        v->print( std::declval< llvm::raw_string_ostream& >() );
    };

    inline std::string llvm_to_string( const llvm_printable auto *val )
    {
        std::string buffer;
        llvm::raw_string_ostream rso( buffer );
        val->print( rso );
        return buffer;
    }

    inline std::string llvm_to_string( const ranges::range auto &vals, std::string_view separator = ", " )
    {
        auto to_llvm = [] (const auto &value) { return llvm_to_string(value); };

        std::stringstream ss;
        auto strs = ranges::views::transform(vals, to_llvm);
        std::copy(strs.begin(), strs.end(), std::experimental::make_ostream_joiner(ss, separator));
        return ss.str();
    }

    inline std::string llvm_name( llvm::Value * value )
    {
        if ( value->hasName() )
            return value->getName().str();

        auto str = llvm_to_string( value );
        if ( llvm::isa< llvm::Instruction >( value ) ) {
            str = std::string( *splitter( str, '=' ).begin() );
            trim(str);
        }

        return str;
    }

    inline std::string llvm_name( llvm::Type * type )
    {
        return llvm_to_string( type );
    }

    inline std::string type( llvm::Type * t )
    {
        if ( t->isPointerTy() )
            return type( t->getPointerElementType() ) + "p";
        return llvm_name(t);
    }

} // namespace sc::fmt
