//
// Created by igor on 21/12/2020.
//

#include "details_tab.h"

details_tab::details_tab(bool isRequest, QWidget* parent)
: QTabWidget(parent), is_request(isRequest)
{
    headersTab = new headers_widget(this);
    bodyTab = new body_widget(isRequest, this);
    addTab(headersTab, isRequest ? "Request Headers" : "Response Heades");
    addTab(bodyTab, isRequest ? "Request Body" : "Response Body");
}

void details_tab::transactionSelected(const binadox::sniffer::transaction& tr)
{
    if (is_request)
    {
        headersTab->setData(tr.m_request, tr.m_url);
    }
    else
    {
        headersTab->setData(tr.m_response);
    }
    bodyTab->setData(tr);
}
