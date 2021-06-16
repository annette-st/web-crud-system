include(ExternalProject)

# git@github.com:synhershko/clucene.git

ExternalProject_Add(
        clucene-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/clucene
        GIT_REPOSITORY git@github.com:synhershko/clucene.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""

        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DBUILD_STATIC_LIBRARIES:BOOL=TRUE
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_CXX_FLAGS=-std=gnu++98
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(clucene-ext install_dir)
macro(DEF_CLUCENE_LIB NAME LIB)
    add_library(${NAME} STATIC IMPORTED GLOBAL)
    add_library(thirdparty::${NAME} ALIAS ${NAME})
    set_property(TARGET ${NAME} PROPERTY IMPORTED_LOCATION ${install_dir}/lib/${LIB})
    set_property(TARGET ${NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir})
    add_dependencies(${NAME} clucene-ext)
endmacro()

DEF_CLUCENE_LIB(clucene-core "libclucene-core-static.a")
DEF_CLUCENE_LIB(clucene "libclucene-shared-static.a")

