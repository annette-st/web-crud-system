#include <iostream>
#include "appgw/js_addon.hh"
#include "sniffer-plug/sniffer_data/input_archive.hh"
#include <cpr/cpr.h>
#include <termcolor/termcolor.hpp>

static binadox::sniffer::transaction load_transaction(const std::string& item)
{
    std::cout << termcolor::white << "Loading item " << item << termcolor::reset;
    static const std::string base_url = "http://localhost:9999/api/sniffer-items/";
    std::string url = base_url + item;
    cpr::Response r = cpr::Get(cpr::Url{url},
                               cpr::Header{{"Authorization", "ApiToken 706cf490-77f9-4a5d-996d-6b3ac98aa4f7"}});
    if (r.status_code != 200)
    {
        if (r.status_code == 0)
        {
            throw std::runtime_error("Connection error");
        }
        throw std::runtime_error(r.text);
    }
    std::cout << termcolor::green << " OK" << termcolor::reset << std::endl;
    std::cout << termcolor::white << "Parsing JSON transaction " << termcolor::reset;
    auto tr = binadox::sniffer::load_from_json(r.text);
    std::cout << termcolor::green << " OK" << termcolor::reset << std::endl;
    return tr;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "USAGE " << argv[0] << " <path-to-script> <item-id>" << std::endl;
        return 1;
    }

    const char* path_to_script = argv[1];
    const char* item = argv[2];

    try
    {
        auto tr = load_transaction(item);
        std::cout << termcolor::white << "Loading script " << path_to_script << termcolor::reset;
        binadox::appgw::js_addon addon = binadox::appgw::js_addon::from_file(path_to_script);
        std::cout << termcolor::green << " OK" << termcolor::reset << std::endl;
        std::cout << termcolor::bright_grey << "----------------------------------------------" << std::endl;
        std::cout << termcolor::white << "get_plugin_name() " << termcolor::yellow << addon.name() << termcolor::reset << std::endl;
        std::cout << termcolor::white << "wants_url(" << termcolor::blue << tr.m_url << termcolor::white << ")" << termcolor::reset;
        const bool wants_url = addon.wants_url(tr.m_url);
        std::cout << termcolor::yellow << " " << (wants_url ? "true" : "false") <<  termcolor::reset << std::endl;

        std::cout << termcolor::cyan << "HEADERS = {" << std::endl;
        for (const auto& kv : tr.m_request.m_headers)
        {
            std::cout << "\t" << kv.first << " : " << kv.second << std::endl;
        }
        std::cout << "}" << termcolor::reset << std::endl;

        const bool accept = addon.accept(tr.m_request.m_method, tr.m_url, tr.m_request.m_headers);
        std::cout << termcolor::white << "accept(" << termcolor::blue
            << tr.m_request.m_method << ","
            << tr.m_url << ","
            << "HEADERS"
            << termcolor::white << ")" << termcolor::reset;
        std::cout << termcolor::yellow << " " << (accept ? "true" : "false") <<  termcolor::reset << std::endl;
        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << std::endl << termcolor::red << "Error: " << e.what() << termcolor::reset << std::endl;
        return 1;
    }
}
