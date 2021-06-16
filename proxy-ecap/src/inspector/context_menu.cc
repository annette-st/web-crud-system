//
// Created by igor on 22/12/2020.
//

#include "context_menu.h"
#include <QApplication>
#include <QClipboard>
#include <QThread>

context_menu::context_menu(QWidget* parent)
: QMenu(parent)
{
    this->addAction("Copy to clipboard", this, SLOT(copyToClipboardSlot()));
}
// --------------------------------------------------------------------------------------------------------------
void context_menu::copyToClipboardSlot()
{
    emit copyToClipboard();
}
// --------------------------------------------------------------------------------------------------------------
void context_menu::setClipboard(const QString& text)
{
    QClipboard* clipboard = QApplication::clipboard();

    clipboard->setText(text, QClipboard::Clipboard);

    if (clipboard->supportsSelection())
    {
        clipboard->setText(text, QClipboard::Selection);
    }

#if defined(Q_OS_LINUX)
    QThread::msleep(1); //workaround for copied text not being available...
#endif
}