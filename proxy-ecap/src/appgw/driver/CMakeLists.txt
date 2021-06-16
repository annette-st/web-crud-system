set (src main.cc)
set (hdr)

add_executable(appgw-driver ${src} ${hdr})
target_link_libraries(appgw-driver
        PRIVATE appgw
        sniffer-data
        thirdparty::cpr
        thirdparty::cpr-curl

        ssl
        crypto
        pthread
        z
        )