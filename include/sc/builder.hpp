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

#include <range/v3/algorithm.hpp>

namespace sc
{
    using value = llvm::Value *;
    using values = std::vector< value >;

    using basicblock = llvm::BasicBlock *;
    using function = llvm::Function *;

    using predicate = llvm::CmpInst::Predicate;

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
        struct or_ : detail::binary {};
        
        template< predicate _pred >
        struct cmp
        {
            static constexpr predicate pred = _pred;
            value lhs, rhs; 
        };

        struct phi
        {
            std::vector< phi_edge > edges;
        };

        struct cast
        {
            cast( value v, type t ) : val( v ), to( t ) {}

            value val;
            type to;
        };

        struct bitcast  : cast { using cast::cast; };
        struct zfit     : cast { using cast::cast; };
        struct fptoui   : cast { using cast::cast; };
        struct ptrtoint : cast { using cast::cast; };
        struct inttoptr : cast { using cast::cast; };

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

        struct add
        {
            std::optional< value > lhs, rhs;
        };

        struct or_
        {
            std::optional< value > lhs, rhs;
        };

        template< predicate _pred >
        struct cmp
        {
            static constexpr predicate pred = _pred;
            std::optional< value > lhs, rhs;
        };
        
        using ne = cmp< predicate::ICMP_NE >;
        using ugt = cmp< predicate::ICMP_UGT >;

        struct cast
        {
            cast( std::optional< value > v, type t ) : val( v ), to( t ) {}

            std::optional< value > val;
            type to;
        };

        struct bitcast  : cast { using cast::cast; };
        struct zfit     : cast { using cast::cast; };
        struct fptoui   : cast { using cast::cast; };
        struct ptrtoint : cast { using cast::cast; };
        struct inttoptr : cast { using cast::cast; };

        using phi = build::phi;

        struct condbr
        {
            explicit condbr( value c ) : cond( c ) {}
            explicit condbr( value c, basicblock tbb, basicblock ebb )
                : cond( c ), thenbb( tbb ), elsebb( ebb )
            {}

            std::optional< value > cond;
            std::optional< basicblock > thenbb, elsebb;
        };

        struct branch
        {
            std::optional< basicblock > dst;
        };

        struct call
        {
            call( value _fn, const std::span< value > &as )
                : call( function( _fn ), as )
            {}

            call( function _fn, const std::span< value > &as )
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
        auto or_( value l, value r ) { return CreateOr( l, r ); }

        template< predicate pred >
        auto cmp( value l, value r )
        {
            return llvm::CmpInst::isFPPredicate( pred )
               ? CreateFCmp( pred, l, r )
               : CreateICmp( pred, l, r );
        }

        auto bitcast( value v, type to ) { return CreateBitCast( v, to ); }

        auto zfit( value v, type to )
        {
            if ( v->getType() == to )
                return v;
            return CreateZExtOrTrunc( v, to );
        }

        auto fptoui( value v, type to )
        {
            if ( v->getType() == to )
                return v;
            return CreateFPToUI( v, to );
        }

        auto ptrtoint( value v, type to ) { return CreatePtrToInt( v, to ); }
        auto inttoptr( value v, type to ) { return CreateIntToPtr( v, to ); }

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
        auto create( build::or_ a ) { return or_( a.lhs, a.rhs ); }

        template< predicate pred >
        auto create( build::cmp< pred > c ) { return cmp< pred >( c.lhs, c.rhs ); }

        auto create( build::phi p ) { return phi( p.edges ); }

        auto create( build::bitcast c )  { return bitcast( c.val, c.to ); }
        auto create( build::zfit z )     { return zfit( z.val, z.to ); }
        auto create( build::fptoui z )   { return fptoui( z.val, z.to ); }
        auto create( build::ptrtoint z ) { return ptrtoint( z.val, z.to ); }
        auto create( build::inttoptr z ) { return inttoptr( z.val, z.to ); }

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

        value popvalue( std::optional< value > v )
        {
            return v.has_value() ? v.value() : pop();
        }

        basicblock popblock( std::optional< basicblock > bb, unsigned pos = 1 )
        {
            return bb.has_value() ? bb.value() : *std::next( current_block, pos );
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
            value l = popvalue( a.lhs );
            value r = popvalue( a.rhs );
            push( builder->create( build::add{{ l, r }} ) );
            return std::move(*this);
        }

        auto apply( action::or_ a ) &&
        {
            value l = popvalue( a.lhs );
            value r = popvalue( a.rhs );
            push( builder->create( build::or_{{ l, r }} ) );
            return std::move(*this);
        }

        template< predicate pred >
        auto apply( action::cmp< pred > c )
        {
            using cmp = build::cmp< pred >;
            value l = popvalue( c.lhs );
            value r = popvalue( c.rhs );
            push( builder->create( cmp{ l, r } ) );
            return std::move(*this);
        }

        auto apply( action::phi p ) &&
        {
            push( builder->create( p ) );
            return std::move(*this);
        }

        template< typename cast >
        auto apply_cast( const auto &c )
        {
            value val = popvalue( c.val );
            push( builder->create( cast( val, c.to ) ) );
            return std::move(*this);
        }

        auto apply( action::bitcast c )  && { return apply_cast< build::bitcast >( c ); }
        auto apply( action::zfit c )     && { return apply_cast< build::zfit >( c ); }
        auto apply( action::fptoui c )   && { return apply_cast< build::fptoui >( c ); }
        auto apply( action::ptrtoint c ) && { return apply_cast< build::ptrtoint >( c ); }
        auto apply( action::inttoptr c ) && { return apply_cast< build::inttoptr >( c ); }

        auto apply( action::condbr br ) &&
        {
            value cond = popvalue( br.cond );
            basicblock thenbb = popblock( br.thenbb );
            basicblock elsebb = popblock( br.elsebb, 2 );
            push( builder->create( build::condbr{ cond, thenbb, elsebb } ) );
            return std::move(*this);
        }

        auto apply( action::branch br ) &&
        {
            basicblock dst = popblock( br.dst );
            push( builder->create( build::branch{ dst } ) );
            return std::move(*this);
        }

        auto function_argument_type( function fn, unsigned idx )
        {
            using function_type = llvm::FunctionType;
            
            function_type *fntype = [fn] {
                if ( fn->getType()->isPointerTy() )
                    return llvm::cast< function_type > (
                        fn->getType()->getPointerElementType()
                    );
                return fn->getFunctionType();
            } ();

            return fntype->getParamType( idx );
        }

        auto apply( const action::call &c ) &&
        {
            values args;
            
            unsigned idx = 0;
            for ( auto arg : c.args ) {
                auto a = popvalue( arg );
                auto to = function_argument_type( c.fn, idx++ );
                if ( to != a->getType() ) {
                    a = builder->create( build::bitcast( a, to ) );
                }

                args.push_back( a );
            }
    
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
                value val = popvalue( r.val );
                builder->create( build::ret{ val } );
            }
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
            current_block = ranges::find_if( blocks, [&] ( const auto &block ) {
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

        basicblock block( const std::string &name )
        {
            auto found_block = ranges::find_if( blocks, [&] ( const auto &block ) {
                return block->getName() == name;
            } );

            assert( found_block != blocks.end() );
            return *found_block;
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
