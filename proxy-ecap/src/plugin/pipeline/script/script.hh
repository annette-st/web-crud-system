//
// Created by igor on 31/08/2020.
//

#ifndef BINADOX_ECAP_SCRIPT_HH
#define BINADOX_ECAP_SCRIPT_HH

#include <time.h>
#include <memory>
#include "plugin/pipeline/script/script_data.hh"

namespace binadox
{
    class script
    {
    public:
        script();
        ~script();

        void set(const std::string& key, const std::string& value);
        void render(std::ostream& os) const;
    private:
        std::shared_ptr<script_data> loaded_script;
        te::values_map_t values_map;
    };

    std::ostream& operator << (std::ostream& os, const script& sc);
} // ns binadox
#endif //BINADOX_ECAP_SCRIPT_HH
