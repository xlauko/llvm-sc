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

#include <map>

namespace sc::adt
{
    
    template< typename A, typename B > struct bimap;

    namespace detail
    {
        template< typename A, typename B >
        struct bimap_impl
        {
            B operator[]( const A &key ) const { return self().left()[ key ]; }
            A operator[]( const B &key ) const { return self().right()[ key ]; }

            size_t count( const A &key ) const { return self().left().count( key ); }
            size_t count( const B &key ) const { return self().right().count( key ); }
        private:
            const bimap< A, B > &self() const
            {
                return *static_cast< const bimap< A, B > * >( this );
            }
        };

        template< typename A >
        struct bimap_impl< A, A > {};

        template< typename K, typename V >
        struct rmap : std::map< K, V > {

            template< typename... Args >
            rmap( Args &&...args ) : std::map< K, V >( std::forward< Args >( args )... ) { }

            using std::map< K, V >::operator[]; // non-const one
            const V &operator[]( const K &key ) const {
                return this->at( key );
            }
        };
    
    } // namesapce detail

    template< typename A, typename B >
    struct bimap : detail::bimap_impl< A, B >
    {
        using value_type = std::pair< const A, const B >;

        static_assert( std::is_same< value_type,
            typename std::map< A, const B >::value_type >::value,
            "value_type" );

        bimap() = default;

        bimap( std::initializer_list< value_type > init ) 
        {
            for ( auto &p : init )
                insert( std::move( p ) );
        }

        template< typename iterator >
        bimap( iterator begin, iterator end )
        {
            for ( ; begin != end; ++begin )
                insert( *begin );
        }

        const detail::rmap< A, const B > &left() const { return _left; }
        const detail::rmap< B, const A > &right() const { return _right; }

        bool insert( const value_type &value )
        {
            return insert( value.first, value.second );
        }

        bool insert( const A &a, const B &b )
        {
            auto l = _left.insert( { a, b } );
            auto r = _right.insert( { b, a } );
            if ( l.second != r.second ) {
                if ( l.second )
                    _left.erase( l.first );
                else
                    _right.erase( r.first );
                throw std::invalid_argument( "bimap: Insert would break bijection" );
            }
            return l.second;
        }

        void erase( const value_type &value )
        {
            auto l = _left.find( value.first );
            auto r = _right.find( value.second );
            if ( l != _left.end() && r != _right.end() ) {
                _left.erase( l );
                _right.erase( r );
            }
        }

    private:
        detail::rmap< A, const B > _left;
        detail::rmap< B, const A > _right;

        static std::pair< const B, const A > swap( const std::pair< const A, const B > &pair ) 
        {
            return { pair.second, pair.first };
        }
        
        static std::pair< const B, const A > swap( std::pair< const A, const B > &&pair )
        {
            return { std::move( pair.second ), std::move( pair.first ) };
        }
    };
} // namspace sc::adt