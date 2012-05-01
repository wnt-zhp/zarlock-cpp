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

MealFoodList::MealFoodList(QWidget* parent): QListWidget(parent), isdirty(false), proxy(NULL), foodkey(-1), cached_parent((MealTabWidget *)parent) {
	CII();
	connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(doItemEdit(QListWidgetItem*)));
}

MealFoodList::~MealFoodList() {
	DII();
}

void MealFoodList::populateModel() {
	int num = proxy->rowCount();

	this->clear();

	insertHeaderSlot();

	for (int i = 0; i < num; ++i) {
		QListWidgetItem * qlwi = new QListWidgetItem(this);
		qlwi->setSizeHint(QSize(100, 32));

		MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this, qlwi);

		int distributor_id = proxy->index(i, DistributorTableModel::HId).data(Qt::EditRole).toInt();
		mflidw->setWidgetData(distributor_id);

		this->addItem(qlwi);
		this->setItemWidget(qlwi, (QWidget *)mflidw);

		connect(mflidw, SIGNAL(itemRemoved(QListWidgetItem*)), this, SLOT(doItemRemoved(QListWidgetItem*)));
	}

	insertEmptySlot();
}

void MealFoodList::refreshModel() {
	proxy->invalidate();

	int num = this->count();

	for (int i = 0; i < num; ++i) {
		((MealFoodListItemDataWidget *)this->itemWidget(this->item(i)))->resetWidgetData();
	}
}

void MealFoodList::insertEmptySlot() {
	QListWidgetItem * qlwi = new QListWidgetItem(this);
	qlwi->setSizeHint(QSize(100, 36));

	MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this, qlwi);

	this->addItem(qlwi);
	this->setItemWidget(qlwi, (QWidget *)mflidw);
	mflidw->convertToEmpty();
}

void MealFoodList::insertHeaderSlot() {
	QListWidgetItem * qlwi = new QListWidgetItem(this);
	qlwi->setSizeHint(QSize(100, 36));
	
	MealFoodListItemDataWidget * mflidw = new MealFoodListItemDataWidget(this, qlwi);
	
	this->insertItem(0, qlwi);
	this->setItemWidget(qlwi, (QWidget *)mflidw);
	mflidw->convertToHeader();

// 	((QVBoxLayout *)((QWidget *)parent())->layout())->insertWidget(0, mflidw);
}

void MealFoodList::setProxyModel(MealTableModelProxy* model) {
	proxy = model;
}

const MealTableModelProxy* MealFoodList::proxyModel() {
	return proxy;
}

void MealFoodList::doItemEdit(QListWidgetItem* item) {
	cached_parent->closeOpenItems();
	cached_parent->markOpenItem(item);
	((MealFoodListItemDataWidget *)itemWidget(item))->buttonUpdate();
	this->repaint();
}

void MealFoodList::doItemRemoved(QListWidgetItem* item) {
	disconnect(sender(), SIGNAL(itemRemoved(QListWidgetItem*)), this, SLOT(doItemRemoved(QListWidgetItem*)));
	to_remove.push_back(item);
}

void MealFoodList::paintEvent(QPaintEvent* e) {
	int s = to_remove.size();
	if (s) {
		for (int i = 0; i < s; ++i)
			this->takeItem(this->row(to_remove[i]));

		qDeleteAll(to_remove);
		to_remove.clear();
		populateModel();
	}

	QListView::paintEvent(e);
}

#include "MealFoodList.moc"
