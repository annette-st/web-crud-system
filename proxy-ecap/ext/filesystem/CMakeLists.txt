include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        filesystem-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/filesystem
        GIT_REPOSITORY https://github.com/gulrak/filesystem.git
        GIT_PROGRESS TRUE
        GIT_TAG master
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DGHC_FILESYSTEM_BUILD_TESTING=OFF
            -DGHC_FILESYSTEM_BUILD_EXAMPLES=OFF
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(filesystem-ext install_dir)
add_library(filesystem INTERFACE IMPORTED GLOBAL)
add_library(thirdparty::filesystem ALIAS doctest)
set_property(TARGET filesystem PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include)




