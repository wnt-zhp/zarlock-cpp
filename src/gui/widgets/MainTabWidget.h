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


#ifndef MAINTABWIDGET_H
#define MAINTABWIDGET_H

#include <QTabWidget>


class MainTabWidget : public QTabWidget {
Q_OBJECT
public:
    explicit MainTabWidget(QWidget* parent = 0);
    virtual ~MainTabWidget();

private slots:
	void changeEvent(QEvent &);
private:
};

#endif // MAINTABWIDGET_H
