//
// Created by igor on 21/12/2020.
//

#include "headers_model.h"


headers_model::headers_model(QObject *parent)
: QAbstractTableModel(parent)
{

}

int headers_model::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}

int headers_model::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant headers_model::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto& tr = m_data[index.row()];
        switch (index.column())
        {
            case 0:
                return tr.first;
            case 1:
                return tr.second;
        }
    }
    return QVariant();
}

QVariant headers_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                return QString("Name");
            case 1:
                return QString("Value");
        }
    }
    return QVariant();
}

void headers_model::setData(const binadox::sniffer::headers_t & headers)
{
    this->beginResetModel();
    m_data.clear();
    for (const auto& kv : headers)
    {
        m_data.emplace_back(QString(kv.first.c_str()), QString(kv.second.c_str()));
    }
    this->endResetModel();
}

QString headers_model::getData(int r, int c) const
{
    const auto d = m_data[r];
    if (c == 0)
    {
        return d.first;
    }
    return d.second;
}