//
// Created by igor on 18/11/2020.
//

#ifndef BINADOX_ECAP_CMD_HELPER_HH
#define BINADOX_ECAP_CMD_HELPER_HH

#include "sniffer-plug/commands/commands_registry.hh"

template<typename Command>
struct register_command
{
    register_command() noexcept
    {
        try
        {
            commands_registry::instance().register_command(new Command);
        } catch (...) {}
    }
};

#define _CONCAT_(x,y) x ## y
#define CONCAT(x,y) _CONCAT_(x,y)

#define REGISTER_COMMAND(x) static register_command<x> CONCAT(_register_command, __LINE__)



#define DECLARE_COMMAND_0(CLASS, NAME, DESCR)                                       \
    struct CLASS : public abstract_command_zero_args                                \
    {                                                                               \
        CLASS() : abstract_command_zero_args (NAME, DESCR) {}                       \
        void _run() override;                                                       \
    };                                                                              \
    REGISTER_COMMAND(CLASS);                                                        \
    void CLASS::_run()


#define DECLARE_COMMAND_1(CLASS, NAME, DESCR)                                       \
    struct CLASS : public abstract_command_one_arg                                  \
    {                                                                               \
        CLASS() : abstract_command_one_arg (NAME, DESCR) {}                         \
        void _run(const std::string& arg) override;                                 \
    };                                                                              \
    REGISTER_COMMAND(CLASS);                                                        \
    void CLASS::_run(const std::string& arg)

#endif //BINADOX_ECAP_CMD_HELPER_HH
