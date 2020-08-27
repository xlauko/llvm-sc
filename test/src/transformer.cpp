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
        using namespace sc::literals;

        auto i = b.add( 10_i8, 5_i8 );
        REQUIRE( llvm::isa< sc::instruction >( i ) );
    }
}
