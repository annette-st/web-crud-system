//
// Created by igor on 22/12/2020.
//

#ifndef BINADOX_ECAP_LABEL_WITH_MENU_H
#define BINADOX_ECAP_LABEL_WITH_MENU_H

#include <QtWidgets/QLabel>

class label_with_menu : public QLabel
{
    Q_OBJECT
public:
    explicit label_with_menu(QWidget* parent);
    void mousePressEvent(QMouseEvent *ev) override;
signals:
    void popUpMenu();
};


#endif //BINADOX_ECAP_LABEL_WITH_MENU_H
