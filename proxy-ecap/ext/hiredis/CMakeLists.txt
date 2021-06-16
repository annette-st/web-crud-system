include(ExternalProject)
find_package(Git REQUIRED)

#-----------------------------
# Define libevent options
#-----------------------------

set(libevent_OPTIONS
  -DEVENT__DISABLE_OPENSSL=OFF
  -DEVENT__DISABLE_THREAD_SUPPORT=OFF
  -DEVENT__DISABLE_BENCHMARK=ON
  -DEVENT__DISABLE_TESTS=ON
  -DEVENT__DISABLE_REGRESS=ON
  -DEVENT__DISABLE_SAMPLES=ON
  -DEVENT__BUILD_SHARED_LIBRARIES=OFF
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)

ExternalProject_Add(
        	libevent-ext
			PREFIX ${CMAKE_BINARY_DIR}/thirdparty/libevent
        	GIT_REPOSITORY git://github.com/libevent/libevent.git
        	GIT_TAG release-2.1.12-stable
			GIT_PROGRESS TRUE
        	UPDATE_COMMAND ""
            INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
			CMAKE_ARGS  ${libevent_OPTIONS} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG>
			BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
        )




IMPORT_THIRDPARTY(libevent-ext libevent libevent.a)
IMPORT_THIRDPARTY(libevent-ext libevent_core libevent_core.a)
IMPORT_THIRDPARTY(libevent-ext libevent_extra libevent_extra.a)
IMPORT_THIRDPARTY(libevent-ext libevent_pthreads libevent_pthreads.a)

ExternalProject_Add(
        hiredis-ext
        DEPENDS libevent-ext
		PREFIX ${CMAKE_BINARY_DIR}/thirdparty/hiredis
        GIT_REPOSITORY https://github.com/redis/hiredis.git
        GIT_TAG v1.0.0
		GIT_PROGRESS TRUE
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
		CONFIGURE_COMMAND ""
        BUILD_IN_SOURCE 1
        BUILD_COMMAND make static
        INSTALL_COMMAND make PREFIX=<INSTALL_DIR> install
)

IMPORT_THIRDPARTY(hiredis-ext hiredis libhiredis.a)




