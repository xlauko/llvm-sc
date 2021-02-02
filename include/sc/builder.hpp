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
    using values = std::vector< value >;

    using basicblock = llvm::BasicBlock *;
    using function = llvm::Function *;

    struct phi_edge
    {
        value val;
        basicblock from;
    };

    struct stack_builder;

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

        struct phi
        {
            std::vector< phi_edge > edges;
        };

        struct bitcast
        {
            value val;
            type to;
        };

        struct zfit
        {
            value val;
            type to;
        };

        struct condbr
        {
            value cond;
            basicblock thenbb, elsebb;
        };

        struct branch
        {
            basicblock dst;
        };

        struct call
        {
            function fn;
            values args;
        };

        struct ret
        {
            std::optional< value > val;
        };

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

        struct bitcast
        {
            std::optional< value > val;
            type to;
        };

        struct zfit
        {
            std::optional< value > val;
            type to;
        };

        using phi = build::phi;

        struct condbr
        {
            explicit condbr( value c ) : cond( c ) {}

            std::optional< value > cond;
            std::optional< basicblock > thenbb, elsebb;
        };

        struct branch
        {
            std::optional< basicblock > dst;
        };

        struct call
        {
            call( function _fn, const values &as )
                : fn( _fn )
            {
                for ( auto arg : as )
                    args.push_back( arg );
            }

            call( function _fn, std::vector< std::optional< value > > &&as )
                : fn( _fn ), args( std::move( as ) )
            {}

            function fn;
            std::vector< std::optional< value > > args;
        };

        struct ret
        {
            std::optional< value > val;
        };

        struct inspect
        {
            using callback = std::function< void( stack_builder* ) >;
            explicit inspect( callback &&c ) : call( std::move( c ) ) {}

            callback call;
        };

        struct last {}; // last produced value

        struct create_block { std::string name = ""; };
        struct advance_block { int value; };
        struct set_block { std::string name; };

        struct create_function
        {
            std::string name = "";
            type rty;
            std::vector< type > args;
            bool vararg = false;
        };

        struct function { llvm::Function * function; };

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

        auto bitcast( value v, type to ) { return CreateBitCast( v, to ); }

        auto zfit( value v, type to )
        {
            if ( v->getType() == to )
                return v;
            return CreateZExtOrTrunc( v, to );
        }

        auto phi( const std::vector< phi_edge > &edges )
        {
            auto n = static_cast< unsigned >( edges.size() );
            assert( !edges.empty() );
            auto phi = CreatePHI( edges.front().val->getType(), n );
            for ( auto edge : edges )
                phi->addIncoming( edge.val, edge.from );
            return phi;
        }

        auto condbr( value c, basicblock t, basicblock f ) { return CreateCondBr( c, t, f ); }
        auto br( basicblock dst ) { return CreateBr(dst); }

        auto call( function fn, const values &args ) { return CreateCall( fn, args ); }

        auto ret( value val ) { return CreateRet( val ); }
        auto retvoid() { return CreateRetVoid(); }

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

        auto create( build::phi p ) { return phi( p.edges ); }

        auto create( build::bitcast c ) { return bitcast( c.val, c.to ); }
        auto create( build::zfit z ) { return zfit( z.val, z.to ); }

        auto create( build::condbr b ) { return condbr( b.cond, b.thenbb, b.elsebb ); }
        auto create( build::branch b ) { return br( b.dst ); }

        auto create( const build::call &c ) { return call( c.fn, c.args ); }

        auto create( build::ret r )
        {
            return r.val.has_value() ? ret( r.val.value() ) : retvoid();
        }
    };

    struct stack_builder
    {
        stack_builder()
            : builder( std::make_unique< builder_t >() )
        {}

        stack_builder( stack_builder&& ) = default;
        stack_builder( const stack_builder& ) = delete;

        stack_builder& operator=( stack_builder&& ) = default;
        stack_builder& operator=( const stack_builder& ) = delete;

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

        auto apply( action::phi p ) &&
        {
            push( builder->create( p ) );
            return std::move(*this);
        }

        auto apply( action::bitcast c ) &&
        {
            value val = c.val.has_value() ? c.val.value() : pop();
            push( builder->create( build::bitcast{ val, c.to } ) );
            return std::move(*this);
        }

        auto apply( action::zfit z ) &&
        {
            value val = z.val.has_value() ? z.val.value() : pop();
            push( builder->create( build::zfit{ val, z.to } ) );
            return std::move(*this);
        }

        auto apply( action::condbr br ) &&
        {
            value cond = br.cond.has_value() ? br.cond.value() : pop();
            basicblock thenbb = br.thenbb.has_value() ? br.thenbb.value() : *std::next( current_block );
            basicblock elsebb = br.elsebb.has_value() ? br.elsebb.value() : *std::next( current_block, 2 );
            push( builder->create( build::condbr{ cond, thenbb, elsebb } ) );
            return std::move(*this);
        }

        auto apply( action::branch br ) &&
        {
            basicblock dst = br.dst.has_value() ? br.dst.value() : *std::next( current_block );
            push( builder->create( build::branch{ dst } ) );
            return std::move(*this);
        }

        auto apply( const action::call &c ) &&
        {
            values args;
            for ( auto arg : c.args )
                args.push_back( arg.has_value() ? arg.value() : pop() );
            push( builder->create( build::call{ c.fn, args } ) );
            return std::move(*this);
        }

        auto apply( action::ret r ) &&
        {
            auto fn = functions.back();
            if ( fn->getReturnType()->isVoidTy() ) {
                assert( !r.val.has_value() );
                builder->create( build::ret{} );
            } else {
                value val = r.val.has_value() ? r.val.value() : pop();
                builder->create( build::ret{ val } );
            }
            blocks.pop_back(); // basic block terminator
            return std::move(*this);
        }

        auto apply( const action::create_block &b ) &&
        {
            if ( !functions.empty() )
                push( llvm::BasicBlock::Create( sc::context(), b.name, functions.back() ) );
            else
                push( llvm::BasicBlock::Create( sc::context(), b.name ) );
            current_block = std::prev( blocks.end() );
            builder->SetInsertPoint( *current_block );
            return std::move(*this);
        }

        auto apply( action::advance_block advance ) &&
        {
            std::advance( current_block, advance.value );
            builder->SetInsertPoint( *current_block );
            return std::move(*this);
        }

        auto apply( action::set_block set ) &&
        {
            current_block = std::ranges::find_if( blocks, [&] ( const auto &block ) {
                return block->getName() == set.name;
            } );

            assert( current_block != blocks.end() );
            builder->SetInsertPoint( *current_block );
            return std::move(*this);
        }

        auto apply( action::module m ) &&
        {
            module = m.module;
            return std::move(*this);
        }

        auto apply( action::function f ) &&
        {
            module = f.function->getParent();
            push( f.function );
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

        auto apply( action::inspect ins ) &&
        {
            ins.call( this );
            return std::move( *this );
        }

        std::unique_ptr< builder_t > builder;

        llvm::Module * module = nullptr;

        std::map< std::string, value > vars;

        std::vector< function > functions;

        std::vector< basicblock > blocks;
        std::vector< basicblock >::iterator current_block;

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
