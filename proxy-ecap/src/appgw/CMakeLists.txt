set (src
        js_addon.cc
        environment.cc
        addons_registry.cc
        addon_token.cc

        api/console.cc
        api/jsurl.cc
        )
set (hdr
        addons_registry.hh
        js_addon.hh
        environment.hh
        addon_token.hh

        api/console.hh
        api/jsurl.hh
        )

add_library(appgw STATIC ${src} ${hdr})
target_link_libraries(appgw PUBLIC binadox-be thirdparty::dukglue thirdparty::duktape)
target_include_directories(appgw PUBLIC ${PROJECT_INCLUDE})

set_target_properties(appgw PROPERTIES
        CXX_STANDARD 11
        POSITION_INDEPENDENT_CODE ON
        )

add_subdirectory(driver)
