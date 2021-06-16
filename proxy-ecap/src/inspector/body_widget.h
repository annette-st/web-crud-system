//
// Created by igor on 26/12/2020.
//

#ifndef BINADOX_ECAP_BODY_WIDGET_H
#define BINADOX_ECAP_BODY_WIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtCore/QByteArray>
#include "Qsci/qsciscintilla.h"
#include "Qsci/qscilexerjson.h"
#include "Qsci/qscilexerhtml.h"

#include "sniffer-plug/sniffer_data/input_archive.hh"

class body_widget : public QWidget
{
    Q_OBJECT
public:
    body_widget(bool isRequest, QWidget* parent);
    ~body_widget();
    void  setData(const binadox::sniffer::transaction& tr);
private:
    bool m_is_request;
    QLabel *content_type_label;
    QLabel *content_length_label;
    QsciScintilla *textBrowser;

    QsciLexerJSON* jsonLexer;
    QsciLexerHTML* htmlLexer;
    QByteArray* buffer;
};


#endif //BINADOX_ECAP_BODY_WIDGET_H
