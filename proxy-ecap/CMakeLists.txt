cmake_minimum_required(VERSION 3.0)
projectweb-crud=ecap CXX C)

# cmake configuration
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


# the `pkg_check_modules` function is created with this call
find_package(PkgConfig REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

pkg_check_modules(ECAP REQUIRED IMPORTED_TARGET libecap)


# Output binary to predictable location.
set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_OUT_DIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIB_OUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_OUT_DIR})

set (PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set (PROJECT_INCLUDE ${PROJECT_ROOT}/src)


find_package(Qt5 COMPONENTS Core)
if (NOT Qt5_FOUND)
    message("QT5 is missing")
else()
    SET(BUILD_INSPECTOR TRUE)
endif()


add_subdirectory(ext)
add_subdirectory(src/plugin)
add_subdirectory(src/backend)
add_subdirectory(src/sniffer-plug/sniffer_data)
add_subdirectory(src/sniffer-plug)
add_subdirectory(src/appgw)

add_subdirectory(src/unittest)




if (BUILD_INSPECTOR)
    message("Building inspector")
    add_subdirectory(src/inspector)
endif()