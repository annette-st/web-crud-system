//
// Created by igor on 20/12/2020.
//

#include "inspector_widget.h"

inspector_widget::inspector_widget(QWidget* parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    horizontalLayout = new QHBoxLayout();


    searchField = new QLineEdit(this);

    horizontalLayout->addWidget(searchField);

    searchPushButton = new QPushButton(this);
    searchPushButton->setText("Search");

    horizontalLayout->addWidget(searchPushButton);


    mainLayout->addLayout(horizontalLayout);

    transactionsDataLayout = new QHBoxLayout();


    transactionsWidget = new transactions_view(this);


    transactionsDataLayout->addWidget(transactionsWidget);

    detailsLayout = new QVBoxLayout();
    requestDetails = new details_tab(true, this);
    detailsLayout->addWidget(requestDetails);

    responseDetails = new details_tab(false, this);
    detailsLayout->addWidget(responseDetails);
    transactionsDataLayout->addLayout(detailsLayout);


    mainLayout->addLayout(transactionsDataLayout);

    connect(transactionsWidget, SIGNAL(transactionSelected(const binadox::sniffer::transaction&)),
            requestDetails, SLOT(transactionSelected(const binadox::sniffer::transaction&)));

    connect(transactionsWidget, SIGNAL(transactionSelected(const binadox::sniffer::transaction&)),
            responseDetails, SLOT(transactionSelected(const binadox::sniffer::transaction&)));
}
// --------------------------------------------------------------------------------------------
void inspector_widget::setDataModel(QAbstractTableModel* model)
{
    transactionsWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    transactionsWidget->setModel(model);
}
