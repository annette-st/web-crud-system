include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        brotli-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/brotli
        GIT_REPOSITORY https://github.com/google/brotli.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG>
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)



ExternalProject_Get_Property(brotli-ext install_dir)

macro(DEF_BROTLI_LIB NAME LIB)
    add_library(${NAME} STATIC IMPORTED GLOBAL)
    add_library(thirdparty::${NAME} ALIAS ${NAME})
    set_property(TARGET ${NAME} PROPERTY IMPORTED_LOCATION ${install_dir}/lib/${LIB})
    set_property(TARGET ${NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir})
    add_dependencies(${NAME} brotli-ext)
endmacro()

DEF_BROTLI_LIB(brotli_enc libbrotlienc-static.a)
DEF_BROTLI_LIB(brotli_dec libbrotlidec-static.a)
DEF_BROTLI_LIB(brotli_comm libbrotlicommon-static.a)

