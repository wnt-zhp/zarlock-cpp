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


#ifndef DIMMINGWIDGET_H
#define DIMMINGWIDGET_H

#include "AbstractDimmingWidget.h"
#include <QStyle>
#include <QLabel>

class DimmingWidget : public AbstractDimmingWidget {
public:
	DimmingWidget(QWidget * parent = NULL);
	virtual ~DimmingWidget();

	void setWidget(QWidget * widget = NULL);

	virtual void showWidget(bool modal = false);
	virtual void hideWidget();

private:
	QWidget * overlay_widget;
	QWidget * widget;
	bool modal_mode;

	QString widget_stylesheet;
};

#endif // DIMMINGWIDGET_H
