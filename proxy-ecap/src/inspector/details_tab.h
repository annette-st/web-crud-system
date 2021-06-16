//
// Created by igor on 21/12/2020.
//

#ifndef BINADOX_ECAP_DETAILS_TAB_H
#define BINADOX_ECAP_DETAILS_TAB_H

#include <QtWidgets/QTabWidget>
#include "headers_widget.h"
#include "body_widget.h"
#include "sniffer-plug/sniffer_data/input_archive.hh"


class details_tab : public QTabWidget
{
    Q_OBJECT
public:
    details_tab(bool isRequest, QWidget* parent);
public slots:
    void  transactionSelected(const binadox::sniffer::transaction&);
private:
    bool is_request;
    headers_widget* headersTab;
    body_widget* bodyTab;
};


#endif //BINADOX_ECAP_DETAILS_TAB_H
