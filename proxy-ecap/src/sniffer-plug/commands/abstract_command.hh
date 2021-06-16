//
// Created by igor on 15/11/2020.
//

#ifndef BINADOX_ECAP_ABSTRACT_COMMAND_HH
#define BINADOX_ECAP_ABSTRACT_COMMAND_HH

#include <string>
#include <vector>
#include <ostream>

class abstract_command
{
public:
    abstract_command(const std::string& name, int nargs);
    virtual ~abstract_command() noexcept = default;

    std::string name() const;
    int nargs() const;

    virtual void run(const std::vector<std::string>& args) = 0;
    virtual void help(std::ostream& os) const = 0;
private:
    std::string m_name;
    int m_nargs;
};

class abstract_command_zero_args : public abstract_command
{
public:
    explicit abstract_command_zero_args (const std::string& name);

    abstract_command_zero_args (const std::string& name, const std::string& descr);

    void run(const std::vector<std::string>& args) override;
    void help(std::ostream& os) const override;
protected:
    virtual void _run() = 0;
private:
    std::string m_descr;
};

class abstract_command_one_arg : public abstract_command
{
public:
    explicit abstract_command_one_arg (const std::string& name);
    abstract_command_one_arg (const std::string& name, const std::string& descr);

    void run(const std::vector<std::string>& args) override;
    void help(std::ostream& os) const override;
protected:
    virtual void _run(const std::string& arg) = 0;
private:
    std::string m_descr;
};

#endif //BINADOX_ECAP_ABSTRACT_COMMAND_HH
