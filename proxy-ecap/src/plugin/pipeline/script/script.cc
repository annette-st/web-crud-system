//
// Created by igor on 31/08/2020.
//

#include "plugin/pipeline/script/script.hh"
#include "plugin/conf/configurator.hh"

namespace binadox
{
    script::script()
    : loaded_script(nullptr)
    {
        configurator* cfg = get_config();
        if (cfg)
        {
            loaded_script = cfg->get_script();
        }
    }
    // ---------------------------------------------------------------------------------------
    script::~script()
    {

    }
    // ---------------------------------------------------------------------------------------
    void script::set(const std::string& key, const std::string& value)
    {
        values_map[key] = value;
    }
    // ---------------------------------------------------------------------------------------
    void script::render(std::ostream& os) const
    {
        if (loaded_script)
        {
            loaded_script->get_engine().render(os, values_map);
        }
    }
    // ---------------------------------------------------------------------------------------
    std::ostream& operator << (std::ostream& os, const script& sc)
    {
        sc.render(os);
        return os;
    }
} // ns binadox
