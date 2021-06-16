//
// Created by igor on 15/11/2020.
//

#ifndef BINADOX_ECAP_COMMANDS_REGISTRY_HH
#define BINADOX_ECAP_COMMANDS_REGISTRY_HH

#include "sniffer-plug/commands/abstract_command.hh"
#include <map>

class commands_registry
{
public:
    static commands_registry& instance();

    void register_command(abstract_command* cmd);

    std::vector<std::string> commands() const;
    void help(const std::string& name, std::ostream& os) const;

    void run(const std::string& line);
private:
    commands_registry() = default;
private:
    std::map<std::string, abstract_command*> m_commands;
};




#endif //BINADOX_ECAP_COMMANDS_REGISTRY_HH
