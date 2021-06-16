//
// Created by igor on 20/12/2020.
//

#ifndef BINADOX_ECAP_TRANSACTIONS_MODEL_H
#define BINADOX_ECAP_TRANSACTIONS_MODEL_H

#include <QAbstractTableModel>
#include "sniffer-plug/sniffer_data/input_archive.hh"

class transactions_model : public QAbstractTableModel
{
    Q_OBJECT
public:
    transactions_model(const std::string& fname, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    const binadox::sniffer::transaction& transaction(std::size_t idx);
private:
    std::vector <binadox::sniffer::transaction> transactions;
};


#endif //BINADOX_ECAP_TRANSACTIONS_MODEL_H
