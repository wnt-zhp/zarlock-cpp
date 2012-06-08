/*
    This file is part of Zarlok.

    Copyright (C) 2012  Rafał Lalik <rafal.lalik@zhp.net.pl>

    Zarlok is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zarlok is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DBITEMWIDGET_H
#define DBITEMWIDGET_H

#include "CampProperties.h"

#include <QWidget>

#include "ui_DBItem.h"

class DBItemWidget : public QWidget, public Ui::DBItem {
public:
    explicit DBItemWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);
    virtual ~DBItemWidget();

	void update(const CampProperties * camp);
};

#endif // DBITEMWIDGET_H
