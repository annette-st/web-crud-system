set (src
        main.cc
        project.cc
        ipc.cc

        thirdparty/linenoise.c

        commands/abstract_command.cc
        commands/commands_registry.cc
        commands/cmd_help.cc
        commands/cmd_proj.cc
        )

set (hdr

        project.hh
        ipc.hh

        thirdparty/linenoise.h
        thirdparty/cxxopts.hpp

        commands/abstract_command.hh
        commands/commands_registry.hh
        commands/cmd_helper.hh

        )

add_executable(sniffer-plug ${src} ${hdr})
target_include_directories(sniffer-plug PRIVATE ${PROJECT_INCLUDE})
set_target_properties(sniffer-plug PROPERTIES CXX_STANDARD 11)
target_link_libraries(sniffer-plug
        sniffer-data
        binadox-be
        thirdparty::hiredis
        thirdparty::libevent_pthreads
        thirdparty::libevent
        thirdparty::json
        thirdparty::termcolor
        Threads::Threads
        )