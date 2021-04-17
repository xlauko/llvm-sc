#include <catch2/catch.hpp>

#include <sc/builder.hpp>
#include <sc/constant.hpp>
#include <sc/ranges.hpp>
#include <sc/init.hpp>

#include <range/v3/algorithm.hpp>

#include <utils.hpp>

TEST_CASE( "is", "[closures]" )
{
    using instruction = llvm::Instruction;
    using constant = llvm::Constant;

    sc::context_t ctx;
    sc::init( ctx );

    SECTION( "constant" )
    {
        auto con = sc::i32( 10 );
        REQUIRE( sc::views::is< constant >( con ) );
        REQUIRE( sc::views::is< constant >( *con ) );
        REQUIRE( !sc::views::is< instruction >( con ) );
        REQUIRE( !sc::views::is< instruction >( *con ) );

        REQUIRE( !sc::views::isnot< constant >( con ) );
        REQUIRE( !sc::views::isnot< constant >( *con ) );
        REQUIRE( sc::views::isnot< instruction >( con ) );
        REQUIRE( sc::views::isnot< instruction >( *con ) );
    }
}

TEST_CASE( "cast", "[closures]" )
{
    using instruction = llvm::Instruction;
    using binary = llvm::BinaryOperator;
    using constant = llvm::Constant;

    sc::context_t ctx;
    sc::init( ctx );

    auto bld = sc::stack_builder()
            | sc::action::module{ sc::empty_module() }
            | sc::action::create_function{ "dummy", sc::void_t(), {} }
            | sc::action::create_block{ "entry" };
    
    auto add = bld
            | sc::action::alloc( sc::i8(), "a" )
            | sc::action::alloc( sc::i8(), "b" )
            | sc::action::load( "b" )
            | sc::action::load( "a" )
            | sc::action::add()
            | sc::action::last();

    REQUIRE( sc::views::cast< instruction >( add ) );
    REQUIRE( sc::views::cast< binary >( add ) );
    REQUIRE( sc::views::dyncast< instruction >( add ) );
    REQUIRE( !sc::views::dyncast< constant >( add ) );
}
