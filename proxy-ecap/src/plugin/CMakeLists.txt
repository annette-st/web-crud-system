set(PRE_CONFIGURE_FILE "version_info.cc.in")
set(POST_CONFIGURE_FILE "${CMAKE_CURRENT_BINARY_DIR}/version_info.cc")
include(git_watcher)


MACRO(RE2C VAR OPTIONS SRC)
  GET_FILENAME_COMPONENT(DST "${SRC}" PATH)
  GET_FILENAME_COMPONENT(NAME "${SRC}" NAME_WE)
  SET (ONAME ${CMAKE_CURRENT_BINARY_DIR}/${DST}/${NAME}.c)
  ADD_CUSTOM_COMMAND(
          OUTPUT "${ONAME}"
          BYPRODUCTS ${ONAME}
          COMMAND cmake -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${DST}"
          COMMAND $<TARGET_FILE:re2c> ${OPTIONS} -W -s -o ${ONAME} "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}"
          DEPENDS "${SRC}"
          DEPENDS ${ARGN}
  )
  SET(${VAR} ${${VAR}} ${ONAME})
ENDMACRO()

RE2C(CSP_SCANNER "" pipeline/csp/csp_scanner.re2c)
RE2C(TE_SCANNER  "" template_engine/te_scanner.re2c)
RE2C(HTML_SCANNER "--storable-state" pipeline/html/html_scanner.re2c)

set (src
        logger.cc
        service.cc
        constants.cc
        fast_rand.cc
        string_utils.cc
        content_encoding.cc

        conf/config_reader.cc
        conf/inih.c
        conf/ini_reader.cc
        conf/configurator.cc
        conf/key_value_storage.cc
        conf/403.cc
        conf/script_config.cc
        conf/forbidden_page_config.cc
        conf/includes_config.cc

        conf/data_manager/hardcoded_data_manager.cc
        conf/data_manager/dynamic_data_manager.cc
        conf/includes_loader/abstract_includes_loader.cc
        conf/includes_loader/fs_includes_loader.cc
        conf/includes_loader/ds_includes_loader.cc

        ipc/ipc.cc
        ipc/factory.cc
        ipc/redis_ipc.cc
        ipc/sender_log.cc
        ipc/message_parser.cc

        filter/ignore_list.cc
        filter/glob_match.cc
        filter/response_acceptor.cc
        filter/regdom/lookup_string_in_fixed_set.cc
        filter/regdom/registry_controlled_domain.cc
        filter/trie.cc
        filter/blacklist.cc
        filter/tld.cc
        filter/sniffer.cc
        filter/replace_engine.cc

        processors/request_processor.cc
        processors/response_processor.cc
        processors/basic_processor.cc
        processors/perf_counter.cc

        pipeline/pipeline_context.cc
        pipeline/pipeline.cc
        pipeline/response_pipeline.cc
        pipeline/request_pipeline.cc
        pipeline/script/script_writer.cc
        pipeline/script/script.cc
        pipeline/script/script_data.cc
        pipeline/zlc.cc
        pipeline/csp/csp_header.cc
        pipeline/csp/csp_parser.cc

        pipeline/script_ipc_pipeline.cc
        pipeline/blacklist_pipeline.cc
        pipeline/response_replace_pipeline.cc
        pipeline/sniffer/basic_sniffer.cc
        pipeline/sniffer/sniffer_req_pipeline.cc
        pipeline/sniffer/sniffer_resp_pipeline.cc
        pipeline/sniffer/appgw_req_pipeline.cc


        sniffer_db/sniffer_meta_data.cc
        sniffer_db/sniffer_db.cc
        sniffer_db/sniffer_ipc.cc

        template_engine/te.cc

        ${HTML_SCANNER}
        ${CSP_SCANNER}
        ${TE_SCANNER}

        ${POST_CONFIGURE_FILE}
        )

set (hdr
        logger.hh
        service.hh
        constants.hh
        fast_rand.hh
        string_utils.hh
        content_encoding.hh

        conf/config_reader.hh
        conf/inih.h
        conf/ini_reader.hh
        conf/configurator.hh
        conf/key_value_storage.hh
        conf/403.hh
        conf/script_config.hh
        conf/forbidden_page_config.hh
        conf/includes_config.hh
        conf/data_manager/hardcoded_data_manager.hh
        conf/data_manager/dynamic_data_manager.hh

        conf/includes_loader/abstract_includes_loader.hh
        conf/includes_loader/fs_includes_loader.hh
        conf/includes_loader/ds_includes_loader.hh

        ipc/ipc.hh
        ipc/factory.hh
        ipc/redis_ipc.hh
        ipc/sender_log.hh
        ipc/message_parser.hh

        filter/ignore_list.hh
        filter/glob_match.hh
        filter/response_acceptor.hh
        filter/regdom/lookup_string_in_fixed_set.h
        filter/regdom/registry_controlled_domain.h
        filter/trie.hh
        filter/blacklist.hh
        filter/tld.hh
        filter/sniffer.hh
        filter/replace_engine.hh

        processors/request_processor.hh
        processors/response_processor.hh
        processors/basic_processor.hh
        processors/nanotimer.hh
        processors/perf_counter.hh

        pipeline/pipeline.hh
        pipeline/pipeline_context.hh
        pipeline/response_pipeline.hh
        pipeline/request_pipeline.hh
        pipeline/script_ipc_pipeline.hh
        pipeline/zlc.hh
        pipeline/html/html_scanner.h
        pipeline/script/script_writer.hh
        pipeline/script/script.hh
        pipeline/script/script_data.hh
        pipeline/csp/base64.hh
        pipeline/csp/sha256.hh
        pipeline/csp/csp_header.hh
        pipeline/csp/csp_scanner.h
        pipeline/csp/csp_parser.hh
        pipeline/blacklist_pipeline.hh
        pipeline/response_replace_pipeline.hh
        pipeline/sniffer/basic_sniffer.hh
        pipeline/sniffer/sniffer_req_pipeline.hh
        pipeline/sniffer/sniffer_resp_pipeline.hh
        pipeline/sniffer/appgw_req_pipeline.hh

        template_engine/te_scanner.h
        template_engine/te.hh

        sniffer_db/sniffer_meta_data.hh
        sniffer_db/sniffer_db.hh
        sniffer_db/sniffer_ipc.hh

        version_info.hh
        )




add_library(binadox-ecap SHARED ${src} ${hdr})
target_include_directories(binadox-ecap PRIVATE ${PROJECT_INCLUDE})
target_compile_definitions(binadox-ecap PRIVATE DATA_ROOT="${PROJECT_ROOT}/test" BINADOX_WITH_UNITTESTS)
set_target_properties(binadox-ecap PROPERTIES CXX_STANDARD 11)

target_link_libraries(binadox-ecap PUBLIC
          appgw
          binadox-be
          PkgConfig::ECAP
          thirdparty::zlibng
          thirdparty::brotli_enc
          thirdparty::brotli_dec
          thirdparty::brotli_comm
          thirdparty::json
          thirdparty::termcolor
          thirdparty::duktape
          thirdparty::doctest
          Threads::Threads
        )

target_compile_options(binadox-ecap PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)
