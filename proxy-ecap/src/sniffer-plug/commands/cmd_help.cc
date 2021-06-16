//
// Created by igor on 15/11/2020.
//

#include <iostream>
#include "termcolor/termcolor.hpp"
#include "sniffer-plug/commands/cmd_helper.hh"

class cmd_help : public abstract_command
{
public:
    cmd_help()
    : abstract_command ("help", -1)
    {

    }

    void run(const std::vector<std::string>& args) override
    {
        if (args.empty())
        {
            for (const auto& s : commands_registry::instance().commands())
            {
                std::cout << termcolor::yellow << s << termcolor::reset << "  ";
                commands_registry::instance().help(s, std::cout);
                std::cout << std::endl;
            }
        }
        else
        {
            if (args.size() == 1)
            {
                auto s = args[0];
                std::cout << s << "  ";
                commands_registry::instance().help(s, std::cout);
            }
            else
            {
                throw std::runtime_error("help <command name>");
            }
        }
    }

    void help(std::ostream& os) const override
    {
        os << "Print help";
    }
};

REGISTER_COMMAND(cmd_help);