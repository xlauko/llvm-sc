#include <catch2/catch.hpp>
#include <sc/annotation.hpp>

TEST_CASE( "annotation" )
{
    SECTION( "create" )
    {
        auto a = sc::annotation( "lart", "abstract" );
        REQUIRE( a.str() == "lart.abstract" );
    }
}
