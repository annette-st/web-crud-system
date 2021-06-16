//
// Created by igor on 13/08/2020.
//

#ifndef BINADOX_ECAP_TE_HH
#define BINADOX_ECAP_TE_HH

#include <vector>
#include <map>
#include <string>
#include <ostream>

#include <nonstd/string_view.hpp>

namespace binadox
{
    class te_parser;

    class te
    {
        friend class te_parser;
    public:
        typedef std::map<std::string, std::string> values_map_t;
    public:
        explicit te(const std::string& input);
        explicit te(const nonstd::string_view& text);
        ~te ();

        void render(std::ostream& os, const values_map_t& vm) const;
        std::string render(const values_map_t& vm) const;
    private:
        struct chunk_t
        {
            chunk_t(const std::string& atext, bool var)
            : text(atext),
              is_var(var)
            {

            }
            std::string text;
            bool is_var;
        };
        std::vector<chunk_t> chunks;
    };
}

#endif //BINADOX_ECAP_TE_HH
