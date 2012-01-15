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

#include "MealTabWidget.h"

#include "DistributorTableModel.h"
#include "Database.h"
#include "MealFoodListItemDataWidget.h"

#include <QListWidgetItem>
#include <QStyle>
#include <QAction>

MealTabWidget::MealTabWidget(QWidget* parent): QTabWidget(parent), openeditem(NULL), mtiw(NULL) {
	this->setDocumentMode(true);

	mtiw = new MealTabInsertWidget(this);
	connect(mtiw, SIGNAL(mealInserted(int)), this, SLOT(reloadMeals()));

	che = new QCheckBox;
	che->setChecked(true);
	cexp = new QCheckBox;
	cexp->setChecked(false);

	btmp = new BatchTableModelProxy(cexp, NULL, NULL, che);
	btmp->setSourceModel(Database::Instance().CachedBatch());
	btmp->setDynamicSortFilter(true);
	btmp->setSortCaseSensitivity(Qt::CaseInsensitive);
	btmp->sort(2, Qt::AscendingOrder);
	btmp->invalidate();

// 	this->addTab(foodlist[0], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Breakfast"));
// 	this->addTab(foodlist[1], tr("2nd Breakfast"));
// 	this->addTab(foodlist[2], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Lunch"));
// 	this->addTab(foodlist[3], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Tea"));
// 	this->addTab(foodlist[4], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Diner"));
// 	this->addTab(foodlist[5], tr("Other 1"));
// 	this->addTab(foodlist[6], tr("Other 2"));
}

MealTabWidget::~MealTabWidget() {
	delete btmp;
	delete che;
	delete cexp;
	FPR(__func__);
}

void MealTabWidget::setIndex(const QModelIndex& index) {
	index_backup = index;

	MealDayTableModel * mdt = Database::Instance().CachedMealDay();
	MealTableModel * mt = Database::Instance().CachedMeal();
	
	QDate sel_meal_date = mdt->index(index.row(), MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();
	QVariant sel_meal_id = mdt->index(index.row(), MealDayTableModel::HId).data(Qt::EditRole);

	btmp->setDateKey(sel_meal_date);
	btmp->invalidate();

	mt->sort(MealTableModel::HMealKind, Qt::AscendingOrder);
	QModelIndexList meals = mt->match(mt->index(0, MealTableModel::HMealDay), Qt::EditRole, sel_meal_id.toInt(), -1, Qt::MatchExactly);

	this->clear();
	for (int i = 0; i < meals.size(); ++i) {
		int mid = mt->index(meals.at(i).row(), MealTableModel::HId).data().toInt();
		QString mn = mt->index(meals.at(i).row(), MealTableModel::HMealName).data().toString();

		MealTableModelProxy * proxy = new MealTableModelProxy;
		proxy->setSourceModel((QAbstractItemModel *)Database::Instance().CachedDistributor());

		MealFoodList * foodlist = new MealFoodList(this);
		this->addTab(foodlist, mn);
		
		foodlist->setIndex(index);
		foodlist->setProxyModel(proxy);

		proxy->setKey(mid);
		proxy->setRefDate(sel_meal_date);
		proxy->invalidate();
		foodlist->populateModel();

	}
	mtiw->setKey(sel_meal_id.toInt());
	this->addTab(mtiw, style()->standardIcon(QStyle::SP_FileDialogStart), tr("Actions"));
}

BatchTableModelProxy* MealTabWidget::getBatchProxyModel() {
	return btmp;
}

void MealTabWidget::markOpenedItems(QListWidgetItem* item) {
	openeditem = item;
}

void MealTabWidget::closeOpenedItems() {
	if (openeditem)
		((MealFoodListItemDataWidget *)openeditem->listWidget()->itemWidget(openeditem))->buttonClose();
	openeditem = NULL;
}

void MealTabWidget::reloadMeals() {
	setIndex(index_backup);
	this->setCurrentIndex(this->count()-1);
}

#include "MealTabWidget.moc"