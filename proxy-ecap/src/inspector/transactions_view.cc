//
// Created by igor on 21/12/2020.
//

#include "transactions_view.h"
#include "transactions_model.h"

transactions_view::transactions_view(QWidget* parent)
:QTableView(parent)
{
    setAutoScroll(false);
    connect(this, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onTableClicked(const QModelIndex &)));
}

void transactions_view::onTableClicked(const QModelIndex& idx)
{
    transactions_model* mdl = (transactions_model*)model();
    emit transactionSelected(mdl->transaction(idx.row()));
}