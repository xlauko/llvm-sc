#include <catch.hpp>

#include <sc/init.hpp>
#include <sc/constant.hpp>

TEST_CASE( "constant" )
{
    sc::context_t ctx;
    sc::init( ctx );

    SECTION( "literals" )
    {
        using namespace sc::literals;

        REQUIRE( sc::value( 10_i32 ) == sc::value( sc::i32( 10 ) ) );
    }
}
