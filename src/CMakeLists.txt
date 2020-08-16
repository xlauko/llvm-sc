add_library( llvmsc
    init.cpp
    context.cpp
)

target_include_directories( llvmsc
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

set_property( TARGET llvmsc PROPERTY POSITION_INDEPENDENT_CODE ON )

# Options
option( ENABLE_SANITIZE "compile with sanitizers enabled" OFF )
option( ENABLE_COVERAGE "compile with test coverage support" OFF )
option( DISABLE_WERROR "disable --werror" OFF )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

if ( ENABLE_SANITIZE )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined" )
endif()

if ( NOT MSVC AND NOT DISABLE_WERROR )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror" )
endif()

if ( ENABLE_COVERAGE )
    if ( NOT CMAKE_CXX_COMPILER_ID MATCHES GNU )
        message( FATAL_ERROR "coverage test is not supported with this compiler" )
    endif()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage" )
endif()

# Info
message( STATUS "CMake generator: ${CMAKE_GENERATOR}" )

target_compile_features( llvmsc PUBLIC cxx_std_17 )
target_compile_options( llvmsc PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pipe -march=native
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od /Wall /Zi>>
)

find_package( LLVM REQUIRED CONFIG )

message( STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}" )
message( STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}" )

include_directories( ${LLVM_INCLUDE_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )

llvm_map_components_to_libnames( llvm support core irreader )

target_link_libraries( llvmsc ${llvm} )
