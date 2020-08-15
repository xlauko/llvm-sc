include( CTest )

add_executable( tests )

target_sources( tests
    PRIVATE
    src/main.cpp
    src/expr.cpp
)

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

add_test( NAME tests COMMAND tests )
© 2020 GitHub, Inc.
