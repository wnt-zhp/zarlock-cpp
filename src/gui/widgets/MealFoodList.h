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


#ifndef MEALFOODLIST_H
#define MEALFOODLIST_H

#include <QListWidget>
#include <QSortFilterProxyModel>
#include "MealTableModelProxy.h"


class MealFoodList : public QListWidget {
Q_OBJECT
public:
	explicit MealFoodList(QWidget* parent = 0);
	virtual ~MealFoodList();

	void setDirty(bool dirty);
	bool isDirty();

	virtual void setProxyModel(MealTableModelProxy * model);
	virtual const MealTableModelProxy * proxyModel();

	virtual void insertEmptySlot();
	virtual void removeEmptySlot();

	virtual void setIndex(const QModelIndex& index);

public slots:
	virtual void populateModel();
	virtual void removeItem(int row);
	virtual void markDirty();

private slots:
	virtual void doItemEdit(QListWidgetItem * item);

private:
	bool isdirty;
	MealTableModelProxy * proxy;
	int foodkey;
	QModelIndex idx;
};

#endif // MEALFOODLIST_H
