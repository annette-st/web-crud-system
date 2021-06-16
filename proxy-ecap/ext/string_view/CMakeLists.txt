include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        string_view-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/string_view

        GIT_REPOSITORY https://github.com/martinmoene/string-view-lite.git
        GIT_PROGRESS TRUE

        TIMEOUT 10
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
)

ExternalProject_Get_Property(string_view-ext source_dir)
add_library(string_view INTERFACE IMPORTED GLOBAL)
file(MAKE_DIRECTORY ${source_dir}/include)
add_library(thirdparty::string_view ALIAS string_view)
set_property(TARGET string_view PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/include/)