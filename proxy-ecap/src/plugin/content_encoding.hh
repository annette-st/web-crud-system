//
// Created by igor on 05/11/2020.
//

#ifndef BINADOX_ECAP_CONTENT_TYPE_H
#define BINADOX_ECAP_CONTENT_TYPE_H

#include <libecap/common/area.h>


namespace binadox {
    enum content_encoding_t {
        eNONE,
        eGZIP,
        eDEFLATE,
        eBR
    };

    std::string to_string(content_encoding_t enc);

    content_encoding_t encoding_from_string(const libecap::Area &header);
}


#endif //BINADOX_ECAP_CONTENT_TYPE_H
