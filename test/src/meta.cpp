#include <catch2/catch.hpp>
#include <sc/init.hpp>
#include <sc/meta.hpp>

TEST_CASE( "meta" )
{
    sc::context_t ctx;
    sc::init( ctx );

    REQUIRE( sc::context_ptr() != nullptr );

    SECTION( "create" )
    {
        auto *n = sc::meta::node( "info" );
        REQUIRE( sc::meta::get_string( n ) );
    }
}
