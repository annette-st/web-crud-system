//
// Created by igor on 22/12/2020.
//

#ifndef BINADOX_ECAP_CONTEXT_MENU_H
#define BINADOX_ECAP_CONTEXT_MENU_H

#include <QtWidgets/QMenu>


class context_menu : public QMenu
{
    Q_OBJECT
public:
    explicit context_menu(QWidget* parent);

    static void setClipboard(const QString& text);
signals:
    void copyToClipboard();
private slots:
    void copyToClipboardSlot();
private:

};


#endif //BINADOX_ECAP_CONTEXT_MENU_H
