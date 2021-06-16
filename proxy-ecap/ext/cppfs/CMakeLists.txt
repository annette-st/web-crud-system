include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        cppfs-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/cppfs
        GIT_REPOSITORY https://github.com/cginternals/cppfs.git
        GIT_PROGRESS TRUE
        GIT_TAG master
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DBUILD_SHARED_LIBS=OFF
            -DOPTION_BUILD_TESTS=OFF
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

IMPORT_THIRDPARTY(cppfs-ext cppfs libcppfsd.a)


