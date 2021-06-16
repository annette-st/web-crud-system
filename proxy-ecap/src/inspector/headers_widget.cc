//
// Created by igor on 21/12/2020.
//
#include <sstream>
#include <QtCore/QUrl>

#include <QtWidgets/QHeaderView>
#include "headers_widget.h"

// ======================================================================================================
headers_widget::headers_widget(QWidget* parent)
        : QWidget(parent)
{
    verticalLayout = new QVBoxLayout(this);

    verticalLayout->setContentsMargins(0, 0, 0, 0);
    label = new label_with_menu(this);
    verticalLayout->addWidget(label);

    line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    verticalLayout->addWidget(line);

    tableView = new QTableView(this);

    tableView->setModel(&model);
    tableView->setAutoScroll(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    verticalLayout->addWidget(tableView);

    menu = new context_menu(this);
    tableMenu = new context_menu(this);

    connect(label, SIGNAL(popUpMenu()), this, SLOT(popUpMenu()));
    connect(menu, SIGNAL( copyToClipboard()), this, SLOT(copyToClipBoard()));
    connect(tableMenu, SIGNAL( copyToClipboard()), this, SLOT(copyToClipBoardFromTable()));
    connect(tableView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(tablePopUpMenu(const QPoint&)));
}
// ------------------------------------------------------------------------------------
void headers_widget::setData(const binadox::sniffer::request& r, const std::string& url)
{
    QUrl qurl(url.c_str());
    QString qs(QString("<b>") + r.m_method.c_str() + QString("</b> ") );
    qs += qurl.scheme() + "://" + qurl.host()  + "<u><b>" + qurl.path() + "</b></u>";
    if (qurl.hasQuery())
    {
        qs += "<font color=\"yellow\">?" + qurl.query() + "</font>";
    }
    QFontMetrics metrix(label->font());
    int width = label->width() - 2;
    QString clippedText = metrix.elidedText(qs, Qt::ElideRight, width);
    label->setText(clippedText);
    label->setToolTip(url.c_str());
    model.setData(r.m_headers);
    m_url = url.c_str();

}
// ------------------------------------------------------------------------------------
void headers_widget::setData(const binadox::sniffer::response& r)
{
    std::ostringstream os;
    os << "STATUS: " << r.m_code;
    label->setText(os.str().c_str());
    model.setData(r.m_headers);
}
// ------------------------------------------------------------------------------------
void headers_widget::popUpMenu()
{
    menu->exec(QCursor::pos());
}
// ------------------------------------------------------------------------------------
void headers_widget::copyToClipBoard()
{
    context_menu::setClipboard(m_url);
}
// ------------------------------------------------------------------------------------
void headers_widget::tablePopUpMenu(const QPoint &pos)
{
    const QModelIndex index = tableView->indexAt(pos);
    if (index.isValid())
    {
        const QPoint point = tableView->viewport()->mapToGlobal(pos);
        tableMenu->exec(point);
    }
}
// ------------------------------------------------------------------------------------
void headers_widget::copyToClipBoardFromTable()
{
    auto idx = tableView->selectionModel()->currentIndex();
    QString text = model.getData(idx.row(), idx.column());
    context_menu::setClipboard(text);
}