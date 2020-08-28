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

#include <llvm/IR/Argument.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instruction.h>
#include <sc/case.hpp>
#include <sc/context.hpp>
#include <sc/transformer.hpp>
#include <sc/meta.hpp>

namespace sc::meta
{
    namespace detail
    {
        void arg_set( llvm::Argument *, meta_str ) {}

        template< typename T > 
        void set( T *val, tag_t tag, meta_str meta )
        {
            if constexpr ( std::is_same_v< T, llvm::Argument > )
                argument::set( val, meta );
            else
                val->setMetadata( tag, tuple::create( node( meta ) ) );
        }

        maybe_meta_str value( node_t node, unsigned idx = 0 )
        {
            if ( !node )
                return std::nullopt;

            assert( node->getNumOperands() );
            
            auto & op = node->getOperand( idx );
            auto str = llvm::cast< llvm::MDString >( llvm::cast< llvm::MDNode >( op )->getOperand( 0 ) );
            
            if (auto res = str->getString().str(); !res.empty() )
                return res;
            return std::nullopt;
        }

        template< typename T > 
        maybe_meta_str get( T *val, tag_t tag )
        {
            if constexpr ( std::is_same_v< T, llvm::Argument > )
                return argument::get( val );
            else
                return value( val->getMetadata( tag ) );
        }

        void function_init( llvm::Function *fn )
        {
            auto size = fn->arg_size();

            if ( !fn->getMetadata( tag::arguments ) ) 
            {
                auto value = [ & ] { return node( tag::none ); };
                auto *data = tuple::create( size, value );
                fn->setMetadata( tag::arguments, data );
            }
        }

    } // namespace detail

    node_t node( meta_str str )
    {
        auto &ctx = context();
        return llvm::MDNode::get( ctx, llvm::MDString::get( ctx, str ) );
    }

    maybe_meta_str get_string( node_t n )
    {
        if ( !n || !n->getNumOperands() ) return std::nullopt;

        const auto &str = n->getOperand( 0 );
        auto res        = llvm::cast< llvm::MDString >( str )->getString().str();
        if ( res.empty() ) return std::nullopt;
        return res;
    }

    void argument::set( llvm::Argument *arg, meta_str str )
    {
        argument::set( arg, meta::node( str ) );
    }

    void argument::set( llvm::Argument *arg, node_t node )
    {
        auto fn = arg->getParent();
        detail::function_init( fn );

        auto meta = fn->getMetadata( tag::arguments );
        meta->replaceOperandWith( arg->getArgNo(), node );
    }

    maybe_meta_str argument::get( llvm::Argument *arg )
    {
        auto fn = arg->getParent();
        if ( auto node = fn->getMetadata( tag::arguments ) )
            return detail::value( node, arg->getArgNo() );
        return std::nullopt;
    }

    void set( llvm::Value *v, tag_t t, meta_str m )
    {
        sc::llvmcase(
            v,
            [ & ]( llvm::Argument *a ) { detail::set( a, t, m ); },
            [ & ]( llvm::Instruction *i ) { detail::set( i, t, m ); },
            [ & ]( llvm::GlobalVariable *g ) { detail::set( g, t, m ); },
            [ & ]( llvm::Function *f ) { detail::set( f, t, m ); },
            [ & ]( llvm::Value * ) { __builtin_unreachable(); } );
    }

    maybe_meta_str get( llvm::Value *val, tag_t tag )
    {
        maybe_meta_str str = std::nullopt;
        sc::llvmcase(
            val,
            [ & ]( llvm::Argument *a ) { str = detail::get( a, tag ); },
            [ & ]( llvm::Instruction *i ) { str = detail::get( i, tag ); },
            [ & ]( llvm::GlobalVariable *g ) { str = detail::get( g, tag ); },
            [ & ]( llvm::Function *f ) { str = detail::get( f, tag ); },
            [ & ]( llvm::Value * ) { __builtin_unreachable(); } );
        return str;
    }

} // namespace sc::meta
