//
// Created by igor on 16/11/2020.
//

#include <iostream>

#include "sniffer-plug/commands/cmd_helper.hh"
#include "sniffer-plug/project.hh"

// =======================================================================================================
DECLARE_COMMAND_0(cmd_proj_cwd, "project-get-current", "prints current project")
{
       std::cout << project::instance().current_project().m_name << std::endl;
}
// =======================================================================================================
DECLARE_COMMAND_1(cmd_proj_new, "project-new", "creates new project")
{
    project::instance().create_project(arg);
    project::instance().change_project(arg);
}
// =======================================================================================================
DECLARE_COMMAND_0(cmd_proj_list, "project-list", "lists all projects")
{
    for (const auto& pd : project::instance().list_projects())
    {
        std::cout << pd.m_name << "\t" << pd.m_time << std::endl;
    }
}
// =======================================================================================================
DECLARE_COMMAND_1(cmd_proj_set, "project-set", "change current project")
{
    project::instance().change_project(arg);
}
// =======================================================================================================
DECLARE_COMMAND_0(cmd_proj_root, "project-get-root", "prints root path for all projects")
{
    std::cout << project::instance().root() << std::endl;
}
// =======================================================================================================
// Sessions
// =======================================================================================================
DECLARE_COMMAND_1(cmd_ses_new, "session-new", "creates new session")
{
    project::instance().create_session(arg);
    project::instance().change_session(arg);
}
// =======================================================================================================
DECLARE_COMMAND_0(cmd_ses_list, "session-list", "lists all sessions")
{
    for (const auto& pd : project::instance().list_sessions())
    {
        std::cout << pd.m_name << "\t" << pd.m_time << std::endl;
    }
}
// =======================================================================================================
DECLARE_COMMAND_1(cmd_ses_set, "session-set", "change current session")
{
    project::instance().change_session(arg);
}
// =======================================================================================================
DECLARE_COMMAND_0(cmd_ses_cwd, "session-get-current", "prints current session")
{
    std::cout << project::instance().current_session().m_name << std::endl;
}
// =======================================================================================================
DECLARE_COMMAND_0(cmd_ses_pause, "session-pause", "pauses current session")
{
    project::instance().pause();
    std::cout << project::instance().current_session().m_name << " - PAUSED" << std::endl;
}
// =======================================================================================================
DECLARE_COMMAND_0(cmd_ses_export, "session-export", "export current session")
{
    project::instance().export_session();
    std::cout << project::instance().current_session().m_name << " exported" << std::endl;
}
// =======================================================================================================
class cmd_ses_record : public abstract_command
{
public:
    cmd_ses_record()
            : abstract_command ("session-record", -1)
    {

    }

    void run(const std::vector<std::string>& args) override
    {
        if (args.empty())
        {
            project::instance().record("127.0.0.1");
            std::cout << project::instance().current_session().m_name << " - Recording from localhost" << std::endl;
        }
        else
        {
            if (args.size() == 1)
            {
                auto s = args[0];
                project::instance().record(s);
                std::cout << project::instance().current_session().m_name << " - Recording from " << s << std::endl;
            }
            else
            {
                throw std::runtime_error("record <ip>");
            }
        }
    }

    void help(std::ostream& os) const override
    {
        os << "Records session";
    }
};

REGISTER_COMMAND(cmd_ses_record);