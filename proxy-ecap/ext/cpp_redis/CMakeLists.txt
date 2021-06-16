include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        cpp_redis-ext
		PREFIX ${CMAKE_BINARY_DIR}/thirdparty/cpp_redis
        GIT_REPOSITORY https://github.com/cpp-redis/cpp_redis.git
		GIT_SUBMODULES tacopie
		GIT_PROGRESS TRUE
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
		CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

IMPORT_THIRDPARTY(cpp_redis-ext cpp_redis libcpp_redis.a)
IMPORT_THIRDPARTY(cpp_redis-ext tacopie libtacopie.a)



