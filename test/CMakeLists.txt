find_package( Catch2 CONFIG REQUIRED )

add_executable( llvmsc-tests )

add_library( catch_main STATIC src/main.cpp )
target_link_libraries( catch_main 
    PRIVATE
        sc_project_options
    PUBLIC
        Catch2::Catch2
)

target_include_directories( catch_main
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_sources( llvmsc-tests
    PRIVATE
        src/init.cpp
        src/meta.cpp
        src/builder.cpp
        src/constant.cpp
        src/annotation.cpp
        src/transformer.cpp
        src/ranges.cpp
)

target_link_libraries( llvmsc-tests
    PRIVATE sc_project_options sc_project_warnings catch_main llvmsc ${llvm}
)

enable_sanitizers( llvmsc-tests )

target_include_directories( llvmsc-tests
    PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

if( NOT LLVM_ENABLE_RTTI )
  set_target_properties( llvmsc-tests PROPERTIES COMPILE_FLAGS "-fno-rtti" )
endif()

include( CTest )
include( Catch )
catch_discover_tests( llvmsc-tests )
