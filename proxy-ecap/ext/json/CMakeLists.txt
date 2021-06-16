include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        json-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/json
        URL https://github.com/nlohmann/json/archive/v3.9.1.tar.gz
        URL_MD5 e386222fb57dd2fcb8a7879fc016d037
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG> -DJSON_BuildTests=OFF
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(json-ext source_dir)
add_library(json INTERFACE IMPORTED GLOBAL)
file(MAKE_DIRECTORY ${source_dir}/)
add_library(thirdparty::json ALIAS json)
set_property(TARGET json PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/)