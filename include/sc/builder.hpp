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

#include <llvm/IR/IRBuilder.h>
#include <sc/context.hpp>
#include <sc/types.hpp>
#include <vector>

namespace sc
{
    using value = llvm::Value *;
    namespace build
    {
        namespace detail
        {
            struct binary { value lhs, rhs; };
        } // namespace detail

        struct alloc
        {
            explicit alloc( type t ) : ty( t ) {}
            alloc( type t, std::string n ) : ty( t ), name( n ) {}

            type ty;
            std::optional< std::string > name;
        };

        struct load
        { 
            std::optional< type > ty;
            value ptr;
        };

        struct add : detail::binary {};
        
    } // namespace build

    namespace action
    {
        namespace detail
        {
            struct binary { std::optional< value > lhs, rhs; };
        } // namespace detail

        using alloc = build::alloc;

        struct load
        {
            load() = default;

            explicit load( type t ) : ty( t ) {}
            explicit load( std::string from ) : from_var( from ) {}
            load( type t, std::string from ) : ty( t ), from_var( from ) {}

            std::optional< type > ty;
            std::optional< std::string > from_var;
        };

        using add = detail::binary;

        struct last {}; //last produced value
        
        struct create_block { std::string name = ""; };

        struct create_function
        {
            std::string name = "";
            type rty;
            std::vector< type > args;
            bool vararg = false;
        };

        struct module { llvm::Module * module; };

    } // namespace action

    struct builder_t : llvm::IRBuilder<>
    {
        using parent = llvm::IRBuilder<>;

        using parent::IRBuilder;

        builder_t() : parent( sc::context() ) {}

        using inst = llvm::Instruction;

        static inline auto as_inst( value val )
        {
            return llvm::cast< inst >( val );
        }

        auto alloc( type ty ) { return CreateAlloca( ty ); }
        auto alloc( type ty, const std::string &name )
        {
            return CreateAlloca( ty, nullptr, name );
        }

        auto load( type ty, value ptr ) { return CreateLoad( ty, ptr ); }
        auto load( value ptr ) { return CreateLoad( ptr ); }

        auto add( value l, value r ) { return CreateAdd( l, r ); }

        auto create( build::alloc a )
        {
            if ( a.name.has_value() )
                return alloc( a.ty, a.name.value() );
            return alloc( a.ty );
        }

        auto create( build::load l )
        {
            if ( l.ty.has_value() ) 
                return load( l.ty.value(), l.ptr ); 
            return load( l.ptr );
        }

        auto create( build::add a ) { return add( a.lhs, a.rhs ); }
    };

    struct stack_builder
    {
        using basicblock = llvm::BasicBlock *;
        using function = llvm::Function *;

        stack_builder()
            : builder( std::make_unique< builder_t >() )
        {}

        stack_builder( stack_builder && ) = default;
        stack_builder( const stack_builder& ) = delete;
        
        ~stack_builder() = default;

        void push( value v ) { stack.push_back( v ); }
        void push( basicblock bb ) { blocks.push_back( bb ); }
        void push( function f ) { functions.push_back( f ); }

        value pop() 
        {
            assert( !stack.empty() );
            value v = stack.back(); 
            stack.pop_back();
            return v;
        }

        auto apply( action::alloc a ) &&
        {
            value var = builder->create( a );

            if ( a.name.has_value() )
                vars[ a.name.value() ] = var;
            else
                push( var ); 

            return std::move( *this );
        }
        
        auto apply( action::load l ) &&
        {
            value ptr = l.from_var.has_value() ? vars.at( l.from_var.value() ) : pop();
            push( builder->create( build::load{ l.ty, ptr } ) );
            return std::move(*this);
        }

        auto apply( action::add a ) && 
        {
            value l = a.lhs.has_value() ? a.lhs.value() : pop();
            value r = a.rhs.has_value() ? a.rhs.value() : pop();
            push( builder->create( build::add{ l, r } ) );
            return std::move(*this);
        }

        auto apply( const action::create_block &b ) &&
        {
            if ( !functions.empty() )
                push( llvm::BasicBlock::Create( sc::context(), b.name, functions.back() ) );
            else
                push( llvm::BasicBlock::Create( sc::context(), b.name ) );
            builder->SetInsertPoint( blocks.back() );
            return std::move(*this);
        }

        auto apply( action::module m ) &&
        {
            module = m.module;
            return std::move(*this);
        }

        auto apply( const action::create_function &f ) &&
        {
            auto fty = llvm::FunctionType::get( f.rty, f.args, f.vararg );
            auto linkage = llvm::GlobalValue::LinkageTypes::ExternalLinkage;
            push( llvm::Function::Create( fty, linkage, f.name, module ) );
            return std::move( *this );
        }

        value apply( action::last ) { return pop(); }

        std::unique_ptr< builder_t > builder;

        llvm::Module * module = nullptr;

        std::map< std::string, value > vars;

        std::vector< function > functions;
        std::vector< basicblock > blocks;
        std::vector< value > stack;
    };

    namespace detail
    {
        template< typename Builder, typename Action >
        auto make_stack_builder( Builder &&builder, Action &&action )
        {
            return std::move( builder ).apply( action );
        }
    } // namespace detail

    template< typename Builder, typename Action >
    auto operator|( Builder &&builder, Action &&action )
    {
        return detail::make_stack_builder( std::forward< Builder >( builder ),
                                           std::forward< Action >( action ) );
    }

} // namespace sc
