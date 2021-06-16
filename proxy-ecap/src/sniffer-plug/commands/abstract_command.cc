//
// Created by igor on 15/11/2020.
//

#include "abstract_command.hh"

abstract_command::abstract_command(const std::string& name, int nargs)
: m_name (name), m_nargs(nargs)
{

}
// ------------------------------------------------------------------------------------------
std::string abstract_command::name() const
{
    return m_name;
}
// ------------------------------------------------------------------------------------------
int abstract_command::nargs() const
{
    return m_nargs;
}
// ===========================================================================================
abstract_command_zero_args::abstract_command_zero_args (const std::string& name)
: abstract_command(name, 0)
{

}
// ------------------------------------------------------------------------------------------------------------
abstract_command_zero_args::abstract_command_zero_args (const std::string& name, const std::string& descr)
        : abstract_command(name, 0),
        m_descr(descr)
{

}
// ------------------------------------------------------------------------------------------
void abstract_command_zero_args::help(std::ostream& os) const
{
    os << m_descr;
}
// ------------------------------------------------------------------------------------------
void abstract_command_zero_args::run(const std::vector<std::string>&)
{
    _run();
}
// ===========================================================================================
abstract_command_one_arg::abstract_command_one_arg (const std::string& name)
        : abstract_command(name, 1)
{

}
// ------------------------------------------------------------------------------------------
abstract_command_one_arg::abstract_command_one_arg (const std::string& name, const std::string& descr)
        : abstract_command(name, 1),
        m_descr(descr)
{

}
// ------------------------------------------------------------------------------------------
void abstract_command_one_arg::help(std::ostream& os) const
{
    os << m_descr;
}
// ------------------------------------------------------------------------------------------
void abstract_command_one_arg::run(const std::vector<std::string>& args)
{
    _run(args[0]);
}
