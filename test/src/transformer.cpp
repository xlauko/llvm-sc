#include <catch.hpp>

#include <sc/init.hpp>
#include <sc/transformer.hpp>
#include <sc/builder.hpp>
#include <sc/constant.hpp>

TEST_CASE( "transformer" )
{
    sc::context_t ctx;
    sc::init( ctx );

    sc::builder_t b;

    using sc::i8;

    SECTION( "operand" )
    {
        auto a = b.add( i8( 10 ), i8( 5 ) );
        auto lhs = sc::transformer( a ).operand( 0 ).freeze();
        REQUIRE( llvm::isa< sc::constant_int >( lhs ) );
    }
}
