//
// Created by igor on 20/12/2020.
//

#include "transactions_model.h"

transactions_model::transactions_model(const std::string& fname, QObject* parent)
        : QAbstractTableModel(parent),
          transactions(binadox::sniffer::load_transactions(fname))
{

}
// ----------------------------------------------------------------------------------------------------
int transactions_model::rowCount(const QModelIndex& parent) const
{
    return transactions.size();
}
// ----------------------------------------------------------------------------------------------------
int transactions_model::columnCount(const QModelIndex& parent) const
{
    return 4;
}
QVariant transactions_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
            case 0:
                return QString("Status");
            case 1:
                return QString("Proto");
            case 2:
                return QString("Domain");
            case 3:
                return QString("Resource");
        }
    }
    return QVariant();
}
// ----------------------------------------------------------------------------------------------------
QVariant transactions_model::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const auto& tr = transactions[index.row()];
        switch (index.column())
        {
            case 0:
                return tr.m_response.m_code;
            case 1:
                return (tr.m_is_http ? "HTTP" : "HTTPS");
            case 2:
                return tr.m_domain.c_str();
            case 3:
                return tr.m_resource.c_str();
        }
    }
    return QVariant();
}
// ----------------------------------------------------------------------------------------------------
const binadox::sniffer::transaction& transactions_model::transaction(std::size_t idx)
{
    return transactions[idx];
}