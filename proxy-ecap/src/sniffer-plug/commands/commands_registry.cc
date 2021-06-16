//
// Created by igor on 15/11/2020.
//

#include <stdexcept>
#include <sstream>
#include "commands_registry.hh"
commands_registry& commands_registry::instance()
{
    static commands_registry inst;
    return inst;
}
// --------------------------------------------------------------------------------------
void commands_registry::register_command(abstract_command* cmd)
{
    m_commands[cmd->name()] = cmd;
}
// --------------------------------------------------------------------------------------
std::vector<std::string> commands_registry::commands() const
{
    std::vector<std::string> ret;
    for (const auto& kv : m_commands)
    {
        ret.push_back(kv.first);
    }
    return ret;
}
// --------------------------------------------------------------------------------------
void commands_registry::help(const std::string& name, std::ostream& os) const
{
    auto itr = m_commands.find(name);
    if (itr == m_commands.end())
    {
        throw std::runtime_error("Unknown command " + name);
    }
    itr->second->help(os);
}
// --------------------------------------------------------------------------------------
static std::vector<std::string> split(const std::string& str, char delim)
{
    std::vector<std::string> strings;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        strings.push_back(str.substr(start, end - start));
    }
    return strings;
}

void commands_registry::run(const std::string& line)
{
    auto tokens = split(line, ' ');
    auto name = tokens[0];
    tokens.erase(tokens.begin());
    auto itr = m_commands.find(name);
    if (itr == m_commands.end())
    {
        throw std::runtime_error("Unknown command " + name);
    }
    auto* cmd = itr->second;
    if (cmd->nargs() >= 0)
    {
        if (cmd->nargs() != tokens.size())
        {
            std::ostringstream os;
            os << "Command " << name << " expects exactly " << cmd->nargs() << " arguments. " << tokens.size() << " args were given";
            throw std::runtime_error(os.str());
        }
    }
    cmd->run(tokens);
}
