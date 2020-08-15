#include <catch.hpp>
#include <sc/init.hpp>

TEST_CASE( "init" )
{
    sc::context_t ctx;
    sc::init( ctx );
}
