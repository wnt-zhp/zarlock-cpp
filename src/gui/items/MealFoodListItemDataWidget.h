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


#ifndef MEALFOODLISTITEMDATAWIDGET_H
#define MEALFOODLISTITEMDATAWIDGET_H

#include "ui_MealFoodListItemDataWidget.h"

class QListWidgetItem;
class MealFoodList;
class BatchTableModelProxy;
class BatchTableView;
class EventFilter;
class TextInput;

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

	void invalidateProxy();

protected slots:
	void validateBatchAdd();
	void validateAdd();

	void eventCaptured(QEvent * evt);
	void setFilter();
	void setFilterString(const QString & string);

signals:
	void itemRemoved(QListWidgetItem * item);
	void itemMerged();

private:
	void resetWidget();
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

	EventFilter * evf;
	QString filter_string;
	TextInput * ledit;
};

#endif // MEALFOODLISTITEMDATAWIDGET_H
