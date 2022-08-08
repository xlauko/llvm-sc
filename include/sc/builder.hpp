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
#include <sc/bimap.hpp>

#include <span>
#include <vector>
#include <algorithm>

namespace sc
{
    using value = llvm::Value *;
    using values = std::vector< value >;

    using basicblock = llvm::BasicBlock *;
    using function = llvm::Function *;

    using function_callee = llvm::FunctionCallee;

    using predicate = llvm::CmpInst::Predicate;
    using binop = llvm::Instruction::BinaryOps;

    struct phi_edge
    {
        value val;
        basicblock from;
    };

    struct stack_builder;

    namespace build
    {
        struct alloc
        {
            explicit alloc( type t ) : ty( t ) {}
            alloc( type t, std::string n ) : ty( t ), name( n ) {}

            type ty;
            std::optional< std::string > name;
        };

        struct load
        {
            type ty;
            value ptr;
        };

        struct store
        {
            value val;
            value ptr;
        };

        template< binop op >
        struct bin { value lhs, rhs; };

        template< predicate pred >
        struct cmp { value lhs, rhs; };

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
            function_callee fn;
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

            // explicit load( type t ) : ty( t ) {}
            load( type t, std::string from ) : ty( t ), from_var( from ) {}

            type ty;
            std::string from_var;
        };

        struct load_ptr
        {
            load_ptr( type t, value f ) : ty( t ), from( f ) {}

            type ty;
            std::optional< value > from;
        };

        struct store
        {
            std::optional< value > val, dst;
        };

        template< binop op >
        struct bin
        {
            std::optional< value > lhs, rhs;
        };

        using add  = bin< binop::Add >;
        using fadd = bin< binop::FAdd >;
        using sub  = bin< binop::Sub >;
        using fsub = bin< binop::FSub >;
        using mul  = bin< binop::Mul >;
        using fmul = bin< binop::FMul >;
        using udiv = bin< binop::UDiv >;
        using sdiv = bin< binop::SDiv >;
        using fdiv = bin< binop::FDiv >;
        using urem = bin< binop::URem >;
        using srem = bin< binop::SRem >;
        using frem = bin< binop::FRem >;

        using shl  = bin< binop::Shl >;
        using lshr = bin< binop::LShr >;
        using ashr = bin< binop::AShr >;
        using and_ = bin< binop::And >;
        using or_  = bin< binop::Or >;
        using xor_ = bin< binop::Xor >;

        template< predicate pred >
        struct cmp
        {
            std::optional< value > lhs, rhs;
        };

        using oeq = cmp< predicate::FCMP_OEQ >;
        using ogt = cmp< predicate::FCMP_OGT >;
        using oge = cmp< predicate::FCMP_OGE >;
        using olt = cmp< predicate::FCMP_OLT >;
        using ole = cmp< predicate::FCMP_OLE >;
        using one = cmp< predicate::FCMP_ONE >;
        using ord = cmp< predicate::FCMP_ORD >;
        using uno = cmp< predicate::FCMP_UNO >;
        using fueq = cmp< predicate::FCMP_UEQ >;
        using fugt = cmp< predicate::FCMP_UGT >;
        using fuge = cmp< predicate::FCMP_UGE >;
        using fult = cmp< predicate::FCMP_ULT >;
        using fule = cmp< predicate::FCMP_ULE >;
        using fune = cmp< predicate::FCMP_UNE >;

        using eq  = cmp< predicate::ICMP_EQ >;
        using ne  = cmp< predicate::ICMP_NE >;
        using ugt = cmp< predicate::ICMP_UGT >;
        using uge = cmp< predicate::ICMP_UGE >;
        using ult = cmp< predicate::ICMP_ULT >;
        using ule = cmp< predicate::ICMP_ULE >;
        using sgt = cmp< predicate::ICMP_SGT >;
        using sge = cmp< predicate::ICMP_SGE >;
        using slt = cmp< predicate::ICMP_SLT >;
        using sle = cmp< predicate::ICMP_SLE >;

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
            explicit condbr( std::optional< value > c, basicblock tbb, basicblock ebb )
                : cond( c ), thenbb( tbb ), elsebb( ebb )
            {}

            std::optional< value > cond;
            basicblock thenbb, elsebb;
        };

        struct branch
        {
            basicblock dst;
        };

        struct call
        {
            call( function_callee callee, const std::span< value > &as )
                : fn( callee )
            {
                for ( auto arg : as )
                    args.push_back( arg );
            }

            call( function_callee callee, std::vector< std::optional< value > > &&as )
                : fn( callee ), args( std::move( as ) )
            {}

            function_callee fn;
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

        struct keep_stack {};

        struct push { value val; };

        struct pop { unsigned n = 1; };

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

        auto store( value val, value ptr ) { return CreateStore( val, ptr ); }

        template< binop op >
        auto bin( value l, value r ) { return CreateBinOp( op, l, r ); }

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

        auto condbr( value c, basicblock t, basicblock f ) {
            return CreateCondBr( c, t, f );
        }

        auto br( basicblock dst ) { return CreateBr(dst); }

        auto call( function fn, const values &args ) { return CreateCall( fn, args ); }
        auto call( function_callee fn, const values &args ) { return CreateCall( fn, args ); }

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
            assert( l.ty );
            return load( l.ty, l.ptr );
        }

        auto create( build::store s ) { return store( s.val, s.ptr ); }

        template< binop op >
        auto create( build::bin< op > a ) { return bin< op >( a.lhs, a.rhs ); }

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

        auto create( build::call c ) { return call( c.fn, c.args ); }

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

        explicit stack_builder( sc::instruction *inst )
            : builder( std::make_unique< builder_t >(inst) )
        {}

        stack_builder( stack_builder&& ) = default;
        stack_builder( const stack_builder& ) = delete;

        stack_builder& operator=( stack_builder&& ) = default;
        stack_builder& operator=( const stack_builder& ) = delete;

        ~stack_builder() = default;

        void push( value v ) { stack.push_back( v ); }
        void push( const std::string &name, basicblock bb ) { blocks.insert(name, bb); }
        void push( function f ) { functions.push_back( f ); }

        value pop()
        {
            assert( !stack.empty() );
            value v = stack.back();
            stack.pop_back();
            return v;
        }

        value back()
        {
            assert( !stack.empty() );
            return stack.back();
        }

        value popvalue( std::optional< value > v )
        {
            return v.has_value() ? v.value() : (keep_stack ? back() : pop());
        }

        auto apply( action::alloc a ) &&
        {
            value var = builder->create( a );

            if ( a.name.has_value() ) {
                vars[ a.name.value() ] = var;
            }

            push( var );
            return std::move( *this );
        }

        auto apply( action::load l ) &&
        {
            push( builder->create( build::load{ l.ty, vars.at( l.from_var ) } ) );
            return std::move(*this);
        }

        auto apply( action::load_ptr l ) &&
        {
            push( builder->create( build::load{ l.ty, popvalue( l.from ) } ) );
            return std::move(*this);
        }

        auto apply( action::store s ) &&
        {
            value v = popvalue( s.val );
            value d = popvalue( s.dst );
            builder->create( build::store{ v, d } );
            return std::move(*this);
        }

        template< binop op >
        auto apply( action::bin< op > a ) &&
        {
            using bin = build::bin< op >;
            value l = popvalue( a.lhs );
            value r = popvalue( a.rhs );
            push( builder->create( bin{ l, r } ) );
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
            basicblock thenbb = br.thenbb;
            basicblock elsebb = br.elsebb;
            push( builder->create( build::condbr{ cond, thenbb, elsebb } ) );
            return std::move(*this);
        }

        auto apply( action::branch br ) &&
        {
            basicblock dst = br.dst;
            push( builder->create( build::branch{ dst } ) );
            return std::move(*this);
        }

        auto function_type_from_value( value v )
        {
            if ( v->getType()->isPointerTy() ) {
                return llvm::cast< llvm::FunctionType > (
                    v->getType()->getPointerElementType()
                );
            }
            return llvm::cast< llvm::Function >(v)->getFunctionType();
        }

        auto function_argument_type( function_callee callee, unsigned idx )
        {
            return callee.getFunctionType()->getParamType( idx );
        }

        auto apply( action::call c ) &&
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
            auto bb = [&] {
                if ( !functions.empty() )
                    return llvm::BasicBlock::Create( sc::context(), b.name, functions.back() );
                return llvm::BasicBlock::Create( sc::context(), b.name );
            } ();

            blocks.insert(b.name, bb);
            current_block = bb;
            builder->SetInsertPoint( current_block );
            return std::move(*this);
        }

        auto apply( action::set_block set ) &&
        {
            assert( blocks.count(set.name) );
            current_block = blocks[set.name];
            builder->SetInsertPoint( current_block );
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

        value apply( action::last ) { return back(); }

        auto apply( action::keep_stack ) &&
        {
            keep_stack = true;
            return std::move( *this );
        }

        auto apply( action::push p ) &&
        {
            push( p.val );
            return std::move( *this );
        }

        auto apply( action::pop p ) &&
        {
            for (unsigned i = 0; i < p.n; i++)
                pop();
            return std::move( *this );
        }

        auto apply( action::inspect ins ) &&
        {
            ins.call( this );
            return std::move( *this );
        }

        basicblock block( const std::string &name )
        {
            assert( blocks.count(name) );
            return blocks[name];
        }

        std::string current_block_name()
        {
            return blocks[current_block];
        }

        std::unique_ptr< builder_t > builder;

        llvm::Module * module = nullptr;

        std::map< std::string, value > vars;

        std::vector< function > functions;

        sc::adt::bimap< std::string, basicblock > blocks;
        basicblock current_block;

        bool keep_stack = false;
        std::vector< value > stack;
    };

    namespace detail
    {
        template< typename Action >
        auto make_stack_builder( stack_builder &&builder, Action &&action )
        {
            return std::move( builder ).apply( action );
        }
    } // namespace detail

    template< typename Action >
    auto operator|( stack_builder &&builder, Action &&action )
    {
        return detail::make_stack_builder( std::move(builder), std::forward< Action >( action ) );
    }

} // namespace sc
