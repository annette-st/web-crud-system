//
// Created by igor on 21/12/2020.
//

#ifndef BINADOX_ECAP_TRANSACTIONS_VIEW_H
#define BINADOX_ECAP_TRANSACTIONS_VIEW_H

#include <QtWidgets/QTableView>
#include "sniffer-plug/sniffer_data/input_archive.hh"

class transactions_view : public QTableView
{
    Q_OBJECT
public:
    explicit transactions_view(QWidget* parent);
public slots:
    void     onTableClicked(const QModelIndex &);
signals:
    void  transactionSelected(const binadox::sniffer::transaction&);
};


#endif //BINADOX_ECAP_TRANSACTIONS_VIEW_H
