add_executable(unittest
        main.cc
        test_base64.cc
        test_sha256.cc
        test_duktape.cc
        )

target_link_libraries(unittest PRIVATE
        binadox-ecap
        thirdparty::doctest
        )
target_include_directories(unittest PRIVATE ${PROJECT_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(unittest PRIVATE DATA_ROOT="${PROJECT_ROOT}/test" BINADOX_WITH_UNITTESTS)
