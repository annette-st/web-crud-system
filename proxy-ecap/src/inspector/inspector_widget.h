//
// Created by igor on 20/12/2020.
//

#ifndef BINADOX_ECAP_INSPECTOR_WIDGET_H
#define BINADOX_ECAP_INSPECTOR_WIDGET_H

#include <QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QAbstractTableModel>

#include "details_tab.h"
#include "transactions_view.h"

class inspector_widget : public QWidget
{
    Q_OBJECT
public:
    inspector_widget(QWidget* parent = nullptr);
    virtual ~inspector_widget() = default;

    void setDataModel(QAbstractTableModel* model);
private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *searchField;
    QPushButton *searchPushButton;
    QHBoxLayout *transactionsDataLayout;
    transactions_view *transactionsWidget;
    QVBoxLayout *detailsLayout;
    details_tab* requestDetails;
    details_tab* responseDetails;
    QWidget *tab_5;
    QWidget *tab_6;

};


#endif //BINADOX_ECAP_INSPECTOR_WIDGET_H
