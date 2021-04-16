#include <catch2/catch.hpp>
#include <sc/builder.hpp>
#include <sc/constant.hpp>
#include <sc/init.hpp>
#include <sc/transformer.hpp>

TEST_CASE( "transformer" )
{
    sc::context_t ctx;
    sc::init( ctx );

    sc::builder_t b;

    SECTION( "operand" )
    {
        using namespace sc::literals;

        auto i = b.add( 10_i8, 5_i8 );
        REQUIRE( llvm::isa< llvm::Constant >( i ) );
    }
}
