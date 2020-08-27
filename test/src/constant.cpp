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

        auto a = 10_i;
        auto b = sc::i32(10);
        REQUIRE( sc::value( a ) == sc::value( b ) );
    }
}
