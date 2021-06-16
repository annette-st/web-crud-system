//
// Created by igor on 15/11/2020.
//
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "thirdparty/cxxopts.hpp"
#include "thirdparty/linenoise.h"

#include <termcolor/termcolor.hpp>
#include "commands/commands_registry.hh"
#include "project.hh"
#include "ipc.hh"

#include "sniffer_data/input_archive.hh"


ipc* global_ipc = nullptr;
// -----------------------------------------------------------------------------------------------------
static void sig_handler(int sig)
{
    if (global_ipc)
    {
        global_ipc->stop();
    }
}
// -----------------------------------------------------------------------------------------------------
static bool is_interactive()
{
    return 1 == isatty(fileno(stdin));
}
// -----------------------------------------------------------------------------------------------------
static void completion(const char *buf, linenoiseCompletions *lc)
{
    auto commands = commands_registry::instance().commands();
    auto ch = buf[0];
    for (const auto& c : commands)
    {
        if (!c.empty() && ch == c[0])
        {
            linenoiseAddCompletion(lc,c.c_str());
        }
    }
}
// -----------------------------------------------------------------------------------------------------
static char* read_input()
{
    std::string default_prompt = "binadox";
    auto current_proj = project::instance().current_project().m_name;
    if (!current_proj.empty())
    {
        default_prompt = current_proj;
        auto current_ses = project::instance().current_session().m_name;
        if (!current_ses.empty())
        {
            default_prompt = current_ses + "@" + default_prompt;
        }
    }
    std::string prompt = default_prompt +"> ";
    return linenoise(prompt.c_str());
}
// -----------------------------------------------------------------------------------------------------
struct itr : public channel_iterator
{
public:
    explicit itr(const std::string& channel) : channel_iterator(channel) {}
    void call (const char* data, std::size_t length) override {};
};
// -----------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{



    const bool interactive_mode = is_interactive();

    cxxopts::Options options("SnifferPlug", "Binadox sniffer tester");
    options.add_options()
        ("p,path", "Where to store projects", cxxopts::value<std::string>()->default_value("."))
        ("r,redis", "Redis port", cxxopts::value<int>()->default_value("6379"))
        ("h,help", "Print usage");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      return 0;
    }

    int redis_port = result["redis"].as<int>();
    std::string proj_path = result["path"].as<std::string>();

    ipc redis_ipc(redis_port);

    global_ipc = &redis_ipc;

    project::init(proj_path, redis_ipc);

    if (interactive_mode)
    {
        linenoiseSetCompletionCallback(completion);
        linenoiseHistorySetMaxLen(100);
    }

    char* line = nullptr;
    while((line = read_input()) != nullptr)
    {
        if (line[0] != 0)
        {
            try
            {
                commands_registry::instance().run(line);
                if (interactive_mode)
                {
                    linenoiseHistoryAdd(line);
                }
            }
            catch (std::exception& e)
            {
                std::cout << termcolor::red << e.what() << termcolor::reset << std::endl;
            }
        }
        free(line);
        line = nullptr;
    }
    if (interactive_mode)
    {
        redis_ipc.stop();
    }
    else
    {
        std::cout << "Current proj/session " << project::instance().current_project().m_name
                  << "/" << project::instance().current_session().m_name << std::endl;

        std::cout << termcolor::yellow << "Hit Ctrl+C to exit" << termcolor::reset << std::endl;
        signal(SIGINT, sig_handler);
    }
    redis_ipc.wait();
    return 0;
}
