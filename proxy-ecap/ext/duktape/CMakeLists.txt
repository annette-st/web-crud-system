include(ExternalProject)

set(DT_PREFIX "${CMAKE_BINARY_DIR}/thirdparty/duktape")
set(DT_SRC_ROOT "${CMAKE_BINARY_DIR}/thirdparty/duktape/src/duktape-ext")

#DUK_USE_DEBUGGER_SUPPORT and DUK_USE_INTERRUPT_COUNTER
file(MAKE_DIRECTORY ${DT_PREFIX}/gen)
file(WRITE ${DT_PREFIX}/gen/CMakeLists.txt

        "cmake_minimum_required(VERSION 3.0)\n"
        "project(duktape)\n"
        "\n"
        "include_directories(${DT_SRC_ROOT}/src)\n"
        "add_library(duktape ${DT_SRC_ROOT}/src/duktape.c)\n"
        "target_compile_definitions(duktape PRIVATE DUK_USE_DEBUGGER_SUPPORT  DUK_USE_INTERRUPT_COUNTER)\n"
        "install(TARGETS duktape DESTINATION lib)\n"
        "install(FILES ${DT_SRC_ROOT}/src/duktape.h ${DT_SRC_ROOT}/src/duk_config.h DESTINATION include)\n"
        )

# Download, configure, build and install.
ExternalProject_Add(duktape-ext
        PREFIX            ${DT_PREFIX}
        URL               https://duktape.org/duktape-2.6.0.tar.xz
        URL_HASH          MD5=01ee8ecf3dd5c6504543c8679661bb20
        UPDATE_COMMAND    ${CMAKE_COMMAND} -E copy ${DT_PREFIX}/gen/CMakeLists.txt ${DT_SRC_ROOT}
        INSTALL_DIR       ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
        )

IMPORT_THIRDPARTY(duktape-ext duktape libduktape.a)

set(DG_PREFIX "${CMAKE_BINARY_DIR}/thirdparty/dukglue")
ExternalProject_Add(dukglue-ext
        PREFIX            ${DG_PREFIX}
        GIT_REPOSITORY    https://github.com/Aloshi/dukglue.git
        GIT_PROGRESS      TRUE
        UPDATE_COMMAND    ""
        INSTALL_DIR       ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
        )
ExternalProject_Get_Property(dukglue-ext source_dir)
add_library(dukglue INTERFACE IMPORTED GLOBAL)
file(MAKE_DIRECTORY ${source_dir}/)
add_library(thirdparty::dukglue ALIAS dukglue)
set_property(TARGET dukglue PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/)