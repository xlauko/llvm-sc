include( CTest )

add_executable( sctests )

target_sources( sctests
    PRIVATE
    src/main.cpp
    src/init.cpp
    src/meta.cpp
    src/constant.cpp
    src/annotation.cpp
    src/transformer.cpp
)

set_target_properties( sctests
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines" )

target_link_libraries( sctests llvmsc )

target_include_directories( sctests
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_compile_features( sctests PRIVATE cxx_std_20 )

target_compile_options(
    sctests
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/std:c++17>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
            $<$<CXX_COMPILER_ID:MSVC>:/WX>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Werror>
            $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-pedantic>
)

# add `-DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE to switch to runtime only checks
# target_compile_definitions(SI_detail_tests PRIVATE
# CATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

add_test( NAME sctests COMMAND sctests )
