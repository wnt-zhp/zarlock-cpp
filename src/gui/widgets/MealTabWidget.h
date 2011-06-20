/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef MEALTABWIDGET_H
#define MEALTABWIDGET_H

#include <QTabWidget>
#include <QToolButton>
#include <QMessageBox>

#include "MealFoodList.h"

class MealTabWidget : public QTabWidget {
Q_OBJECT
public:
	explicit MealTabWidget(QWidget* parent = 0);
	virtual ~MealTabWidget();

public slots:
	virtual void setDateRef(const QString & ref);
// 	void insertNewTab();
// 	void removeNewTab(int index);

protected:
//     virtual void tabInserted(int index);
//     virtual void tabRemoved(int index);

private:
// 	QToolButton * b;

// 	QAction * aBreakfast;
// 	QAction * a2ndBreakfast;
// 	QAction * aLunch;
// 	QAction * aDiner;
// 	QAction * aOther1;
// 	QAction * aOther2;
// 
// 	QIcon fakeIcon;
// 	QString fakeLabel;
// 	QWidget * fakeWidget;
// 
// 	QMessageBox msgb;

	static const int meals = 7;

	MealFoodList * foodlist[meals];
	MealTableModelProxy * proxy[meals];
};

#endif // MEALTABWIDGET_H
