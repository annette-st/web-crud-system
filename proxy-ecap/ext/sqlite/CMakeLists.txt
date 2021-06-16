include(ExternalProject)

set(SQLITE_PREFIX "${CMAKE_BINARY_DIR}/thirdparty/sqlite")
set(SQLITE_SRC_ROOT "${CMAKE_BINARY_DIR}/thirdparty/sqlite/src/sqlite-ext")


file(MAKE_DIRECTORY ${SQLITE_PREFIX}/gen)
file(WRITE ${SQLITE_PREFIX}/gen/CMakeLists.txt

        "cmake_minimum_required(VERSION 3.0)\n"
        "project(sqlite)\n"
        "\n"
        "include_directories(${SQLITE_SRC_ROOT})\n"
        "add_library(sqlite3 ${SQLITE_SRC_ROOT}/sqlite3.c)\n"
        "install(TARGETS sqlite3 DESTINATION lib)\n"
        "install(FILES sqlite3.h sqlite3ext.h DESTINATION include)\n"
        )

# Download, configure, build and install.
ExternalProject_Add(sqlite-ext
        PREFIX            ${SQLITE_PREFIX}
        URL               https://www.sqlite.org/2019/sqlite-amalgamation-3290000.zip
        URL_HASH          SHA1=a0eba79e5d1627946aead47e100a8a6f9f6fafff
        UPDATE_COMMAND    ${CMAKE_COMMAND} -E copy ${SQLITE_PREFIX}/gen/CMakeLists.txt ${SQLITE_SRC_ROOT}
        INSTALL_DIR       ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG> -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR>
        )

IMPORT_THIRDPARTY(sqlite-ext sqlite libsqlite3.a)

