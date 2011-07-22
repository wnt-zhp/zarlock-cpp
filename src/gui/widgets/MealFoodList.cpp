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
#include "Database.h"
#include "DistributorTableModel.h"
#include "MealTabWidget.h"

MealFoodList::MealFoodList(QWidget* parent): QListWidget(parent), isdirty(false), proxy(NULL), foodkey(-1) {
	connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doItemEdit(QListWidgetItem*)));
}

MealFoodList::~MealFoodList() {
}

void MealFoodList::populateModel() {
	int num = proxy->rowCount();

	this->clear();
	for (int i = 0; i < num; ++i) {
		QListWidgetItem * qlwi = new QListWidgetItem(this);
		qlwi->setSizeHint(QSize(100, 48));

		MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this, qlwi);

		QModelIndex didx = proxy->mapToSource(proxy->index(i, DistributorTableModel::HBatchId));

		mflidw->setBatchData(didx);
		this->addItem(qlwi);
		this->setItemWidget(qlwi, (QWidget *)mflidw);
	}

	insertEmptySlot();
}

void MealFoodList::insertEmptySlot() {
	QListWidgetItem * qlwi = new QListWidgetItem(this);
	qlwi->setSizeHint(QSize(100, 48));

	MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this, qlwi);

	this->addItem(qlwi);
	this->setItemWidget(qlwi, (QWidget *)mflidw);
}

void MealFoodList::setProxyModel(MealTableModelProxy* model) {
	proxy = model;
}

const MealTableModelProxy* MealFoodList::proxyModel() {
	return proxy;
}

void MealFoodList::doItemEdit(QListWidgetItem* item) {
	((MealTabWidget *)parent())->closeOpenedItems();
	((MealTabWidget *)parent())->markOpenedItems(item);
	((MealFoodListItemDataWidget *)itemWidget(item))->buttonUpdate();
}

void MealFoodList::markDirty() {
	// this is for autoupdate
	Database::Instance().updateMealCosts(idx);
	//this is for delayed update
//	Database::Instance().CachedMeal()->setData(Database::Instance().CachedMeal()->index(idx.row(), MealTableModel::HDirty), 1, Qt::EditRole);
//	Database::Instance().CachedMeal()->submitAll();
}

void MealFoodList::setIndex(const QModelIndex& index) {
	idx = index;
}

const QModelIndex& MealFoodList::getIndex() {
	return idx;
}

#include "MealFoodList.moc"