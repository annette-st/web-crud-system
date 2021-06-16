//
// Created by igor on 21/12/2020.
//

#ifndef BINADOX_ECAP_HEADERS_MODEL_H
#define BINADOX_ECAP_HEADERS_MODEL_H

#include <QtCore/QAbstractTableModel>
#include <QtCore/QString>

#include <vector>
#include <utility>

#include "sniffer-plug/sniffer_data/input_archive.hh"

class headers_model : public QAbstractTableModel
{
Q_OBJECT
public:
    headers_model(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setData(const binadox::sniffer::headers_t & headers);
    QString getData(int r, int c) const;
private:
    std::vector<std::pair<QString, QString>> m_data;
};



#endif //BINADOX_ECAP_HEADERS_MODEL_H
