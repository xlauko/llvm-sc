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
