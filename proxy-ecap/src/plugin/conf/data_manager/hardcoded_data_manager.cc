//
// Created by igor on 31/12/2020.
//

#include "hardcoded_data_manager.hh"
#include "plugin/conf/403.hh"
#include "backend/fs.hh"
#include "backend/work_dir.hh"
#include "plugin/logger.hh"

static void read_files_in_dir(const fs::path& path, const std::string& extension, std::map<std::string, std::string>& out)
{
    if (!fs::exists(path) || !fs::is_directory(path))
    {
        LOG_TRACE("Can not read files from", path);
        return;
    }
    for (const auto& fsentry : fs::directory_iterator(path))
    {
        if (fsentry.is_directory())
        {
            continue;
        }
        auto name = fsentry.path().filename();
        const auto ext = name.extension();
        if (ext ==extension)
        {
            auto full_path = path / name;
            auto plain_name = name.replace_extension().string();
            out [plain_name] = binadox::read_file_to_string(full_path);
        }
    }
}
// ======================================================================================================
class hardcoded_data_source : public binadox::data_source
{
public:
    hardcoded_data_source(const binadox::config& cfg);

    list_of_strings get_ignore_list() const override;
    list_of_strings get_black_list() const override;
    void set_black_list(const list_of_strings& data) override;

    map_of_strings get_login_include_scripts() const override;
    map_of_strings  get_onload_scripts() const override;
    std::string get_injection_script() const override;
    std::string get_403_page() const override;

    list_of_strings get_addons_list() const override;

    void clear(type_t what);
private:
    fs::path path_to_script;
    fs::path path_to_logins;
    fs::path path_to_onload;

    list_of_strings m_black_list;

};

// -------------------------------------------------------------------------------------------------------
hardcoded_data_source::hardcoded_data_source(const binadox::config& cfg)
        : path_to_script(cfg.path_to_script),
        path_to_logins(cfg.path_to_logins),
        path_to_onload(cfg.path_to_onload)
{

}
// -------------------------------------------------------------------------------------------------------
hardcoded_data_source::list_of_strings hardcoded_data_source::get_ignore_list() const
{
    static list_of_strings out{
            "*.firefox.com",
            "*.googlesyndication.com",
            "*.googleads.*",
            "googleads.*",
            "*.doubleclick.net",
            "*.uptolike.*",
            "*.getsale.io",
            "*.phoenix-widget.com",
            "*.openx.net",
            "cdn.firstimpression.io",
            "*.googleapis.com",
            "truvidplayer.com",
            "*.moatads.com",
            "*.trustarc.com",
            "*.adnxs.com",
            "*.casalemedia.com",
            "*.demdex.net",
            "ads.pubmatic.com",
            "*.optimizely.com",
            "*.bounceexchange.com",
            "*.indexww.com",
            "*.rubiconproject.com",
            "widgets.outbrain.com",
            "*.criteo.com",
            "*.consensu.org",
            "*.amazon-adsystem.com",
            "adservice.*",
            "detectportal.firefox.com",
            "incoming.telemetry.mozilla.org",
            "detectportal.firefox.com",
            "push.services.mozilla.com",
            "snippets.*.mozilla.net",
            "*.services.mozilla.com",
            "tracking-protection.cdn.mozilla.net",
            "*.google-analytics.com",
            "google-analytics.com",
            "content-signature*.cdn.mozilla.net"
    };
    return out;
}
// -------------------------------------------------------------------------------------------------------
void hardcoded_data_source::set_black_list(const list_of_strings& data)
{
    m_black_list = data;
}
// -------------------------------------------------------------------------------------------------------
hardcoded_data_source::list_of_strings hardcoded_data_source::get_black_list() const
{
    return m_black_list;
}
// -------------------------------------------------------------------------------------------------------
hardcoded_data_source::map_of_strings hardcoded_data_source::get_login_include_scripts() const
{
    map_of_strings out;
    read_files_in_dir(path_to_logins, ".js", out);
    return out;
}
// -------------------------------------------------------------------------------------------------------
hardcoded_data_source::map_of_strings  hardcoded_data_source::get_onload_scripts() const
{
    map_of_strings out;
    read_files_in_dir(path_to_onload, ".js", out);
    return out;
}
// -------------------------------------------------------------------------------------------------------
std::string hardcoded_data_source::get_injection_script() const
{
    LOG_TRACE("Loading script from ", path_to_script);
    return binadox::read_file_to_string(path_to_script);
}
// -------------------------------------------------------------------------------------------------------
std::string hardcoded_data_source::get_403_page() const
{
    return binadox::page403;
}
// -------------------------------------------------------------------------------------------------------
hardcoded_data_source::list_of_strings hardcoded_data_source::get_addons_list() const
{
    return {};
}
// -------------------------------------------------------------------------------------------------------
void hardcoded_data_source::clear(type_t what)
{
    if (what == eBLACK_LIST)
    {
        m_black_list.clear();
    }
    else
    {
        data_source::clear(what);
    }
}
// ==================================================================================================
hardcoded_data_manager::hardcoded_data_manager(const binadox::config& config)
        : m_config(config)
{

}
// --------------------------------------------------------------------------------------------------
std::unique_ptr<binadox::data_source> hardcoded_data_manager::init()
{
    return std::unique_ptr<binadox::data_source>(new hardcoded_data_source(m_config));
}