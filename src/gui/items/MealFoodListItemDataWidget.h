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

#include "ui_MealFoodListItemDataWidget.h"
#include "MealFoodList.h"

class MealFoodListItemDataWidget : public QWidget, public Ui::MealFoodListItemDataWidget {
Q_OBJECT
public:
	explicit MealFoodListItemDataWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);

	virtual ~MealFoodListItemDataWidget();

	bool isEmpty();

public slots:
	void render(bool doRender);

	void buttonAdd();
	void buttonUpdate();
	void buttonRemove();

	void setBatchData(const QModelIndex & idx);
	void convertToEmpty();

protected slots:
	void validateBatchAdd();
	void validateAdd();

private:
	bool empty, lock;

	double quantity;
	QString batchlabel;

	QModelIndex batch_idx;
	QModelIndex dist_idx;

	MealFoodList * mfl;
};

#endif // MEALFOODLISTITEMDATAWIDGET_H
