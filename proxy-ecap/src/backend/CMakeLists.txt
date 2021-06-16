set (src
        sqlite/sqlite.cc

        sniffer/sniffer_storage.cc
        sniffer/fs_sniffer_storage.cc

        whereami.c
        work_dir.cc
        active_object.cc
        data_source.cc
        data_manager.cc
        ev_util.cc

        redis/redis_io.cc
        crc_64.cc
)

set(hdr
        sqlite/sqlite.hh

        sniffer/sniffer_storage.hh
        sniffer/fs_sniffer_storage.hh

        whereami.h
        work_dir.hh
        abstract_printer.hh
        raise.hh
        active_object.hh
        fs.hh
        unused.hh
        data_source.hh
        data_manager.hh

        ev_util.hh

        redis/redis_io.hh
        redis/redis_connection_parameters.hh
        crc_64.hh
)

add_library(binadox-be STATIC ${src} ${hdr})
target_include_directories(binadox-be PRIVATE ${PROJECT_INCLUDE})
target_compile_definitions(binadox-be PRIVATE DATA_ROOT="${PROJECT_ROOT}/test" BINADOX_WITH_UNITTESTS)
set_target_properties(binadox-be PROPERTIES
        CXX_STANDARD 11
        POSITION_INDEPENDENT_CODE ON
        )

target_compile_options(binadox-be PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>: -Wall -Wextra -pedantic>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

target_link_libraries(binadox-be PUBLIC
        thirdparty::sqlite
        thirdparty::string_view
        thirdparty::filesystem
        thirdparty::hiredis
        thirdparty::libevent_pthreads
        thirdparty::libevent
        ${CMAKE_DL_LIBS}
        )