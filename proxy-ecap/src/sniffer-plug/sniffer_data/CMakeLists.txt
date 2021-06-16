set (src
        archive.cc
        input_archive.cc)

set (hdr
        archive.hh
        input_archive.hh
        zip/miniz.h
        zip/ZipArchive.hpp
        zip/ZipEntry.hpp
        zip/ZipException.hpp
        zip/ZipUtilities.hpp)

add_library(sniffer-data STATIC ${src} ${hdr})
target_link_libraries(sniffer-data binadox-be)
target_include_directories(sniffer-data PRIVATE ${PROJECT_INCLUDE})
set_target_properties(sniffer-data PROPERTIES CXX_STANDARD 11)
