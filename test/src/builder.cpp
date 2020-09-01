#include <catch.hpp>
#include <sc/builder.hpp>
#include <sc/constant.hpp>
#include <sc/init.hpp>

#include <utils.hpp>

TEST_CASE( "builder" )
{
    sc::context_t ctx;
    sc::init( ctx );

    auto builder = sc::stack_builder()
          | sc::action::module( sc::empty_module() )
          | sc::action::create_function( "dummy", sc::void_t(), {} );

    SECTION( "constant" )
    {
        using namespace sc::literals;

        auto i = builder
          | sc::action::create_block( "constant-test" )
          | sc::action::add( 10_i8, 5_i8 )
          | sc::action::last();

        REQUIRE( llvm::isa< llvm::Constant >( i ) );
    }

    SECTION( "chain" )
    {
        auto inst = builder
          | sc::action::create_block( "chain-test" )
          | sc::action::alloc( sc::i8() )
          | sc::action::load()
          | sc::action::last();

        REQUIRE( llvm::isa< llvm::LoadInst >( inst ) );
    }

    SECTION( "chain stacked" )
    {
        auto inst = builder
          | sc::action::create_block( "chain-stacked-test" )
          | sc::action::alloc( sc::i8(), "a" )
          | sc::action::alloc( sc::i8(), "b" )
          | sc::action::load( "a" )
          | sc::action::load( "b" )
          | sc::action::add()
          | sc::action::last();

        REQUIRE( llvm::isa< llvm::Instruction >( inst ) );
        REQUIRE( llvm::cast< llvm::Instruction >( inst )->getOpcode()
                 == llvm::Instruction::Add );
    }

    SECTION( "mixed load" )
    {
        using namespace sc::literals;

        auto inst = builder
          | sc::action::create_block( "mixed-load-test" )
          | sc::action::alloc( sc::i8(), "a" )
          | sc::action::load( "a" )
          | sc::action::add( {}, 5_i8 )
          | sc::action::last();

        REQUIRE( llvm::isa< llvm::Instruction >( inst ) );
        REQUIRE( llvm::cast< llvm::Instruction >( inst )->getOpcode()
                 == llvm::Instruction::Add );
    }
}
