include( CTest )

add_executable( tests )

target_sources( tests
    PRIVATE
    src/main.cpp
    src/init.cpp
    src/meta.cpp
)

target_link_libraries( tests llvmsc )

target_include_directories( tests
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_compile_features( tests PRIVATE cxx_std_17 )

target_compile_options(
    tests
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

add_test( NAME tests COMMAND tests )
