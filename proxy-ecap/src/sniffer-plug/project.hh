//
// Created by igor on 16/11/2020.
//

#ifndef BINADOX_ECAP_PROJECT_HH
#define BINADOX_ECAP_PROJECT_HH

#include <string>
#include <vector>
#include "backend/fs.hh"

struct entry_description
{
    std::string m_name;
    std::string m_time;
};

class ipc;

class project
{
public:
    static void init(const std::string& root, ipc& global_ipc);
    static project& instance();

    std::vector<entry_description> list_projects() const;
    void create_project(const std::string& name);

    std::string root() const;
    entry_description current_project() const;
    void change_project(const std::string& name);

    void create_session(const std::string& name);
    std::vector<entry_description> list_sessions() const;
    entry_description current_session() const;
    void change_session(const std::string& name);
    void record(const std::string& ip);
    void pause();


    void handle_data(const std::string& channel);

    void write_meta(const char* data, std::size_t len, const std::string& trid, bool is_req);
    void write_data(const char* data, std::size_t len, int chunk, const std::string& trid, bool is_req);

    void export_session();

private:
    explicit project(const std::string root);
private:
    fs::path m_root;
    std::string m_cwd;
    std::string m_session;
private:
    static project* s_instance;
};


#endif //BINADOX_ECAP_PROJECT_HH
