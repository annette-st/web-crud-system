//
// Created by igor on 21/12/2020.
//

#ifndef BINADOX_ECAP_HEADERS_WIDGET_H
#define BINADOX_ECAP_HEADERS_WIDGET_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

#include "sniffer-plug/sniffer_data/input_archive.hh"
#include "headers_model.h"
#include "label_with_menu.h"
#include "context_menu.h"

class headers_widget : public QWidget
{
    Q_OBJECT
public:
    explicit headers_widget(QWidget* parent);

    void setData(const binadox::sniffer::request& r, const std::string& url);
    void setData(const binadox::sniffer::response& r);
private slots:
    void popUpMenu();
    void copyToClipBoard();
    void copyToClipBoardFromTable();
    void tablePopUpMenu(const QPoint &pos);
private:
    QVBoxLayout *verticalLayout;
    label_with_menu *label;
    QFrame *line;
    QTableView *tableView;
    context_menu* menu;
    context_menu* tableMenu;
    headers_model model;
    QString m_url;
};


#endif //BINADOX_ECAP_HEADERS_WIDGET_H
