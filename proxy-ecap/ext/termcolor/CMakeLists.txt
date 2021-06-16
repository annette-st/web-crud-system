include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        termcolor-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/termcolor
        GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor.git
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG>
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(termcolor-ext source_dir)
add_library(termcolor INTERFACE IMPORTED GLOBAL)
file(MAKE_DIRECTORY ${source_dir}/)
add_library(thirdparty::termcolor ALIAS termcolor)
set_property(TARGET termcolor PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/)