//
// Created by igor on 22/12/2020.
//

#include <QMouseEvent>
#include "label_with_menu.h"

label_with_menu::label_with_menu(QWidget* parent)
: QLabel(parent)
{

}

void label_with_menu::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::RightButton)
    {
        emit popUpMenu();
    }
}
