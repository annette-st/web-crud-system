include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        re2c-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/re2c
        URL https://github.com/skvadrik/re2c/archive/2.0.1.tar.gz
        TIMEOUT 60
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG>
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(re2c-ext install_dir)
add_executable(re2c IMPORTED GLOBAL)
add_executable(lexical::re2c ALIAS re2c)
set_property(TARGET re2c PROPERTY IMPORTED_LOCATION ${install_dir}/bin/re2c)
add_dependencies(re2c re2c-ext)


