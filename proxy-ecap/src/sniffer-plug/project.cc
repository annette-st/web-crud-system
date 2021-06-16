//
// Created by igor on 16/11/2020.
//
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include "project.hh"
#include "ipc.hh"
#include <nlohmann/json.hpp>
#include "sniffer_data/archive.hh"

class data_handler : public channel_iterator
{
public:
    explicit data_handler (const std::string& channel);
    void call (const char* data, std::size_t length) override;
private:
    enum state_t
    {
        EXPECT_META,
        EXPECT_CHUNK,
        EXPECT_DATA
    };
    state_t m_state;
    std::string m_transaction;
    bool m_is_req;
    int m_chunk_num;
};
// ----------------------------------------------------------------------------------
data_handler::data_handler (const std::string& channel)
: channel_iterator(channel),
  m_state(EXPECT_META),
  m_chunk_num(-1)
{
    m_is_req = (channel.find("REQ-") == 0);
    auto pos = channel.find("-");
    m_transaction = channel.substr(pos+1);
}
// ----------------------------------------------------------------------------------
void data_handler::call (const char* data, std::size_t length)
{
    if (EXPECT_META == m_state)
    {
        // STORE STATE
        project::instance().write_meta(data, length, m_transaction, m_is_req);
        m_state = EXPECT_CHUNK;
    }
    else
    {
        if (m_state == EXPECT_CHUNK)
        {
            auto message = nlohmann::json::parse(std::string(data, length));
            if (message.contains("id"))
            {
                try
                {
                    std::istringstream is(message["id"].get<std::string>());
                    is >> m_chunk_num;
                }
                catch (std::exception& e)
                {
                    std::cout << e.what() << std::endl;
                }

            }
            m_state = EXPECT_DATA;
        }
        else
        {
            project::instance().write_data(data, length, m_chunk_num, m_transaction, m_is_req);
            m_state = EXPECT_CHUNK;
        }
    }
}
// ==================================================================================
static entry_description from_path(fs::path path)
{
    auto tm = fs::last_write_time(path);
    std::ostringstream os;
    std::time_t cftime = decltype(tm)::clock::to_time_t(tm); // assuming system_clock
    os << std::asctime(std::localtime(&cftime));
    entry_description pd;
    pd.m_name = path.filename();
    pd.m_time = os.str();
    return pd;
}
// ==================================================================================
project* project::s_instance = nullptr;
ipc* s_ipc = nullptr;
// ----------------------------------------------------------------------------------
project::project(const std::string root)
{
    if (root == ".")
    {
        m_root = fs::path(root) / fs::path("sniffer-proj");
    }
    else
    {
        m_root = root;
    }
    if (!fs::exists(m_root))
    {
        fs::create_directories(m_root);
    }
}
// ----------------------------------------------------------------------------------
void project::init(const std::string& root, ipc& global_ipc)
{
    if (!s_instance)
    {
        s_instance = new project(root);
        s_ipc = &global_ipc;
    }
}
// ----------------------------------------------------------------------------------
project& project::instance()
{
    return *s_instance;
}
// ----------------------------------------------------------------------------------
std::vector<entry_description> project::list_projects() const
{
    std::vector<entry_description> ret;
    for (const auto& de : fs::directory_iterator(m_root))
    {
        if (de.is_directory())
        {
            ret.push_back(from_path(de.path()));
        }
    }
    return ret;
}
// ----------------------------------------------------------------------------------
void project::create_project(const std::string& name)
{
    auto new_path = m_root / name;
    if (fs::exists(new_path))
    {
        throw std::runtime_error(name + " already exists");
    }
    fs::create_directories(new_path);
}
// ----------------------------------------------------------------------------------
std::string project::root() const
{
    return m_root.string();
}
// ----------------------------------------------------------------------------------
entry_description project::current_project() const
{
    entry_description pd;
    if (m_cwd.empty())
    {
        return pd;
    }
    return from_path(m_root / m_cwd);
}
// ----------------------------------------------------------------------------------
void project::change_project(const std::string& name)
{
    auto new_path = m_root / name;
    if (fs::exists(new_path))
    {
        m_cwd = name;
    }
    else
    {
        throw std::runtime_error(name + " does not exists");
    }
}
// ----------------------------------------------------------------------------------
void project::create_session(const std::string& name)
{
    auto new_path = m_root / m_cwd / name;
    if (!fs::exists(new_path))
    {
        fs::create_directories(new_path);
    }
    else
    {
        throw std::runtime_error(name + "@" + m_cwd + " already exists");
    }
}
// ----------------------------------------------------------------------------------
std::vector<entry_description> project::list_sessions() const
{
    if (m_cwd.empty())
    {
        throw std::runtime_error("no current project selected");
    }
    std::vector<entry_description> ret;
    for (const auto& de : fs::directory_iterator(m_root / m_cwd ))
    {
        if (de.is_directory())
        {
            ret.push_back(from_path(de.path()));
        }
    }
    return ret;
}
// ----------------------------------------------------------------------------------
entry_description project::current_session() const
{
    entry_description pd;
    if (m_cwd.empty() || m_session.empty())
    {
        return pd;
    }
    return from_path(m_root / m_cwd / m_session);
}
// ----------------------------------------------------------------------------------
void project::change_session(const std::string& name)
{
    if (m_cwd.empty())
    {
        throw std::runtime_error("no current project selected");
    }

    auto new_path = m_root / m_cwd / name;
    if (!fs::exists(new_path))
    {
        throw std::runtime_error("Session " + name + "@" + m_cwd + " does not exists");
    }
    m_session = name;
}
// ----------------------------------------------------------------------------------
void project::record(const std::string& ip)
{
    s_ipc->start_record(ip);
}
// ----------------------------------------------------------------------------------
void project::pause()
{
    s_ipc->stop_record();
}
// ----------------------------------------------------------------------------------
void project::write_meta(const char* data, std::size_t len, const std::string& trid, bool is_req)
{
    auto new_path = m_root / m_cwd / m_session / trid / (is_req ? "REQ" : "RESP");
    if (!fs::exists(new_path))
    {
        fs::create_directories(new_path);
    }
    auto f = new_path / "headers";
    std::ofstream ofs(f.string(), std::ios::binary);
    ofs.write(data, len);
}
// ----------------------------------------------------------------------------------
void project::write_data(const char* data, std::size_t len, int chunk, const std::string& trid, bool is_req)
{
    auto new_path = m_root / m_cwd / m_session / trid / (is_req ? "REQ" : "RESP");
    if (!fs::exists(new_path))
    {
        fs::create_directories(new_path);
    }
    std::ostringstream os;
    auto f = new_path / "chunk";
    os << f.string() << "." << chunk;
    std::ofstream ofs(os.str(), std::ios::binary);
    ofs.write(data, len);
}
// ----------------------------------------------------------------------------------
void project::export_session()
{
    if (m_cwd.empty() || m_session.empty())
    {
        throw std::runtime_error("current session is undefined");
    }

    auto path = m_root / m_cwd / m_session;
    if (!fs::exists(path))
    {
        throw std::runtime_error("current session is corrupted");
    }

    binadox::sniffer::create_output_archive(path.string());

}
// ----------------------------------------------------------------------------------
void project::handle_data(const std::string& channel)
{
    s_ipc->iterate_channel(new data_handler(channel));
}
