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


#ifndef MEALFOODLISTITEMDATAWIDGET_H
#define MEALFOODLISTITEMDATAWIDGET_H

#include <QDataWidgetMapper>

#include "ui_MealFoodListItemDataWidget.h"
#include "MealFoodList.h"
#include "BatchTableModelProxy.h"
#include "BatchTableView.h"

class MealFoodListItemDataWidget : public QWidget, public Ui::MealFoodListItemDataWidget {
Q_OBJECT
public:
	explicit MealFoodListItemDataWidget(QWidget* parent = 0, QListWidgetItem * item = 0, Qt::WindowFlags f = 0);

	virtual ~MealFoodListItemDataWidget();

	bool isEmpty();
	int distributorId();

public slots:
	void render();
	void update();

	void buttonAdd();
	void buttonUpdate();
	void buttonClose();
	void buttonRemove();

	void setWidgetData(int id);
	void resetWidgetData();
	void convertToEmpty();
	void convertToHeader();

	void invalidate();

protected slots:
	void validateBatchAdd();
	void validateAdd();

signals:
	void itemRemoved(QListWidgetItem * item);

private:
	void prepareView();
	void deleteView();

	int mergeBox(const QModelIndexList & list);

	bool empty, editable, lock;

	double quantity;

	int batch_row;
	int dist_id, dist_row;

	MealFoodList * mfl;
	QListWidgetItem * owner;
	BatchTableModelProxy * proxy;
	BatchTableView * tv;
};

#endif // MEALFOODLISTITEMDATAWIDGET_H
