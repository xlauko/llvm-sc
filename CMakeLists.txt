cmake_minimum_required( VERSION 3.17 )

project( llvm-sc
    LANGUAGES CXX
    VERSION 0.1.0
)

add_subdirectory( src )

enable_testing()
add_subdirectory( test )
