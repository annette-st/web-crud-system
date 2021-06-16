//
// Created by igor on 26/12/2020.
//

#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QJsonDocument>
#include <QtWidgets/QFrame>

#include "body_widget.h"

body_widget::body_widget(bool isRequest, QWidget* parent)
        : QWidget(parent),
        m_is_request(isRequest)
{
    auto* verticalLayout = new QVBoxLayout(this);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    auto* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    auto label = new QLabel(this);
    label->setText("Content-Type:");

    horizontalLayout->addWidget(label);

    content_type_label = new QLabel(this);


    horizontalLayout->addWidget(content_type_label);

    auto* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    auto label_3 = new QLabel(this);
    label_3->setText("Content-Length:");

    horizontalLayout->addWidget(label_3);

    content_length_label = new QLabel(this);


    horizontalLayout->addWidget(content_length_label);


    verticalLayout->addLayout(horizontalLayout);

    auto* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    verticalLayout->addWidget(line);

    textBrowser = new QsciScintilla(this);
    textBrowser->setReadOnly(true);
    QFont f1 = textBrowser->font();
    QFontMetrics fm(f1);
    f1.setPointSize(15);
    textBrowser->setFont(f1);

    verticalLayout->addWidget(textBrowser);

    jsonLexer = new QsciLexerJSON(this);
    jsonLexer->setFont(f1);

    htmlLexer = new QsciLexerHTML(this);
    htmlLexer->setFont(f1);

    buffer = new QByteArray();
}
// -----------------------------------------------------------------------------------------------------
body_widget::~body_widget() noexcept
{
    delete buffer;
}
// -----------------------------------------------------------------------------------------------------
enum content_type_t
{
    eNONE,
    eJSON,
    eHTML,
    eRAW
};
// -----------------------------------------------------------------------------------------------------
void  body_widget::setData(const binadox::sniffer::transaction& tr)
{
    const binadox::sniffer::headers_t* headers = nullptr;
    if (m_is_request)
    {
        headers = &tr.m_request.m_headers;
    }
    else
    {
        headers = &tr.m_response.m_headers;
    }

    bool has_cl = false;
    bool has_ct = false;
    QString raw_content_type;
    for (const auto& kv : *headers)
    {
        QString name(kv.first.c_str());
        if (name.compare("content-length", Qt::CaseInsensitive) == 0)
        {
            QString value(kv.second.c_str());
            content_length_label->setText(value);
            has_cl = true;
        }
        if (name.compare("content-type", Qt::CaseInsensitive) == 0)
        {
            QString value(kv.second.c_str());
            auto idx = kv.second.find(";");
            if (idx != std::string::npos)
            {
                raw_content_type = QString(kv.second.substr(0, idx).c_str()).trimmed().toLower();
            }
            else
            {
                raw_content_type = QString(kv.second.c_str()).trimmed().toLower();
            }
            content_type_label->setText(value);
            has_ct = true;
        }
    }
    if (!has_cl)
    {
        content_length_label->setText("N/A");
    }
    if (!has_ct)
    {
        content_type_label->setText("N/A");
    }
    content_type_t ct = eNONE;
    if (raw_content_type == "application/json")
    {
        textBrowser->setLexer(jsonLexer);
        ct = eJSON;
    }
    if (raw_content_type == "text/html")
    {
        textBrowser->setLexer(htmlLexer);
        ct = eHTML;
    }
    if (raw_content_type == "application/x-www-form-urlencoded")
    {
        ct = eRAW;
    }
    if (ct != eNONE)
    {
        buffer->clear();
        const std::vector<std::vector<unsigned char>>* chunks = nullptr;
        if (m_is_request)
        {
            chunks = &tr.m_request.m_chunks;
        }
        else
        {
            chunks = &tr.m_response.m_chunks;
        }
        for (const auto& chunk : *chunks)
        {
            buffer->append((const char*)chunk.data(), (int)chunk.size());
        }
        if (ct == eJSON)
        {
            QJsonDocument json = QJsonDocument::fromJson(*buffer);
            QString txt = json.toJson(QJsonDocument::Indented);
            textBrowser->setText(txt);
        }
        else
        {
            textBrowser->setText(*buffer);
        }
    }
    else
    {
        textBrowser->clear();
        textBrowser->setLexer();
    }

}
