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

#include "globals.h"

#include "MealFoodList.h"
#include <QPushButton>
#include <QLayout>

#include "MealFoodListItemDataWidget.h"
#include <Database.h>

// #include "MealFoodListItemData.h"
// #include "MealFoodListItemNew.h"

MealFoodList::MealFoodList(QWidget* parent): QListWidget(parent), isdirty(false), proxy(NULL), foodkey(-1) {
// 	this->setResizeMode(Adjust);
// 	this->
// 	this->res
// Lipken ?

// 	QListWidgetItem * qlwi = new QListWidgetItem();
// 	qlwi->setSizeHint(QSize(100, 48));
// 
// 	MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget;
// 	this->addItem(qlwi);
// 	this->setItemWidget(qlwi, (QWidget *)mflidw);
// 
// 	mflidw->convertToData();

// 	proxy = new MealTableModelProxy();
// 	proxy->setSourceModel((QAbstractItemModel *)Database::Instance().CachedDistributor());

// 	populateModel();
}

MealFoodList::~MealFoodList() {
}

void MealFoodList::populateModel() {
	int num = proxy->rowCount();

	this->clear();
	for (int i = 0; i < num; ++i) {
		QListWidgetItem * qlwi = new QListWidgetItem();
		qlwi->setSizeHint(QSize(100, 48));

		MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this);

		QModelIndex didx = proxy->mapToSource(proxy->index(i, DistributorTableModel::HBatchId));

		mflidw->setBatchData(didx);
		this->addItem(qlwi);
		this->setItemWidget(qlwi, (QWidget *)mflidw);

		mflidw->convertToData();
	}

	insertEmptySlot();
}

void MealFoodList::insertEmptySlot() {
	QListWidgetItem * qlwi = new QListWidgetItem();
	qlwi->setSizeHint(QSize(100, 48));

	MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this);

	this->addItem(qlwi);
	this->setItemWidget(qlwi, (QWidget *)mflidw);
}

void MealFoodList::setDirty(bool dirty) {
	isdirty = dirty;
}

bool MealFoodList::isDirty() {
	return isdirty;
}

void MealFoodList::setProxyModel(MealTableModelProxy* model) {
	proxy = model;
}

const MealTableModelProxy* MealFoodList::proxyModel() {
	return proxy;
}

#include "MealFoodList.moc"