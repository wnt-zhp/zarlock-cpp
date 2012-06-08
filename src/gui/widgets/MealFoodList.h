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


#ifndef MEALFOODLIST_H
#define MEALFOODLIST_H

#include <QListWidget>
#include <QSortFilterProxyModel>
#include "MealTableModelProxy.h"

class MealTabWidget;

class MealFoodList : public QListWidget {
Q_OBJECT
public:
	explicit MealFoodList(QWidget* parent = 0);
	virtual ~MealFoodList();

	virtual void setProxyModel(MealTableModelProxy * model);
	virtual const MealTableModelProxy * proxyModel();

	virtual void insertEmptySlot();
	virtual void insertHeaderSlot();
public slots:
	virtual void populateModel();
	virtual void refreshModel();

	virtual void doItemEdit(QListWidgetItem * item);
	virtual void doItemRemoved(QListWidgetItem * item);

protected:
	virtual void paintEvent(QPaintEvent* e);

private:
	bool isdirty;
	MealTableModelProxy * proxy;
	int foodkey;
	MealTabWidget * cached_parent;
	QVector<QListWidgetItem *> to_remove;
};

#endif // MEALFOODLIST_H
