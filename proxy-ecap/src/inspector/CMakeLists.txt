set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set (src main.cc
        inspector_widget.cc
        transactions_model.cc
        details_tab.cc
        transactions_view.cc
        headers_widget.cc
        headers_model.cc
        context_menu.cc
        label_with_menu.cc
        body_widget.cc)

set (hdr
        inspector_widget.h
        transactions_model.h
        details_tab.h
        transactions_view.h
        headers_widget.h
        headers_model.h
        context_menu.h
        label_with_menu.h
        body_widget.h)

add_executable(inspector ${src} ${hdr})
target_include_directories(inspector PRIVATE ${PROJECT_INCLUDE}
        ${CMAKE_CURRENT_SOURCE_DIR}/qscintilla)
set_target_properties(inspector PROPERTIES CXX_STANDARD 11)
target_link_libraries(inspector
        sniffer-data
        qscintilla
        Qt5::Widgets
        )

add_subdirectory(qscintilla)