#include <catch2/catch.hpp>

#include <sc/builder.hpp>
#include <sc/constant.hpp>
#include <sc/ranges.hpp>
#include <sc/init.hpp>

#include <algorithm>

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

TEST_CASE( "llvm views", "[ranges]" )
{
    using namespace sc::literals;

    sc::context_t ctx;
    sc::init( ctx );

    auto bld = sc::stack_builder()
          | sc::action::module{ sc::empty_module() }
          | sc::action::create_function{ "dummy", sc::void_t(), {} }
          | sc::action::create_block{ "entry" }
          | sc::action::create_block{ "then" }
          | sc::action::create_block{ "else" };

    auto tbb = bld.block( "then" );
    auto ebb = bld.block( "else" );

    /* entry block */
    bld = bld
          | sc::action::set_block{ "entry" }
          | sc::action::alloc( sc::i8(), "a" )
          | sc::action::alloc( sc::i8(), "b" )
          | sc::action::load( "b" )
          | sc::action::load( "a" )
          | sc::action::ne()
          | sc::action::condbr( {}, tbb, ebb );

    /* then block */
    bld = bld
          | sc::action::set_block{ "then" }
          | sc::action::load( "a" )
          | sc::action::add{ {}, 5_i8 };

    /* else block */
    bld = bld
          | sc::action::set_block{ "else" }
          | sc::action::load( "b" )
          | sc::action::sub{ {}, 5_i8 }
          | sc::action::add{ {}, {} };

    auto m = bld.module;
    auto fn = m->getFunction( "dummy" );

    SECTION( "instructions" )
    {
        REQUIRE( ranges::distance( sc::views::instructions(m) ) == 11 );
        REQUIRE( ranges::distance( sc::views::instructions(*m) ) == 11 );

        REQUIRE( ranges::distance( sc::views::instructions(fn) ) == 11 );
        REQUIRE( ranges::distance( sc::views::instructions(*fn) ) == 11 );

        REQUIRE( ranges::distance( sc::views::instructions(tbb) ) == 2 );
        REQUIRE( ranges::distance( sc::views::instructions(*tbb) ) == 2 );
        REQUIRE( ranges::distance( sc::views::instructions(ebb) ) == 3 );
        REQUIRE( ranges::distance( sc::views::instructions(*ebb) ) == 3 );
    }

    SECTION( "filter instructions" )
    {
        auto insts = sc::views::instructions(m);

        using load = llvm::LoadInst;
        auto loads = insts | sc::views::mapdyncast< load >
                           | std::views::filter( sc::views::notnull );

        REQUIRE( ranges::distance( loads ) == 4 );

        auto isi8 = [] (auto v) { return v == sc::i8(); };
        REQUIRE( ranges::all_of( loads | sc::views::types, isi8 ) );

        using bin = llvm::BinaryOperator;
        REQUIRE( ranges::distance( sc::views::filter< bin >( m ) ) == 3 );
        REQUIRE( ranges::distance( sc::views::filter< bin >( *m ) ) == 3 );
    }
}
