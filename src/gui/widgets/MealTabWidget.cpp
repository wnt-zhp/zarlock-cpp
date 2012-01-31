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

	batch_proxy = new BatchTableModelProxy(cexp, NULL, NULL, che);
	batch_proxy->setSourceModel(Database::Instance().CachedBatch());
	batch_proxy->setDynamicSortFilter(true);
	batch_proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	batch_proxy->sort(2, Qt::AscendingOrder);
	batch_proxy->invalidate();

	lock = false;
	connect(this, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

MealTabWidget::~MealTabWidget() {
	delete batch_proxy;
	delete che;
	delete cexp;
	FPR(__func__);
}

void MealTabWidget::setMealDayId(int mdid) {
	MealDayTableModel * mdt = Database::Instance().CachedMealDay();
	MealTableModel * mt = Database::Instance().CachedMeal();

	QModelIndexList mdl = mdt->match(mdt->index(0, MealDayTableModel::HId), Qt::EditRole, mdid, -1);
	// if there is no days then return
	if (mdl.count() != 1)
		return;

	QDate sel_meal_date = mdt->index(mdl.at(0).row(), MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();

	meal_day_id = mdid;
	
	batch_proxy->setDateKey(sel_meal_date);
	batch_proxy->invalidate();

	mt->sort(MealTableModel::HMealKind, Qt::AscendingOrder);
	QModelIndexList meals = mt->match(mt->index(0, MealTableModel::HMealDay), Qt::EditRole, mdid, -1, Qt::MatchExactly);

	lock = true;
	this->clear();
	for (int i = 0; i < meals.size(); ++i) {
		int mid = mt->index(meals.at(i).row(), MealTableModel::HId).data().toInt();
		QString mn = mt->index(meals.at(i).row(), MealTableModel::HMealName).data().toString();

		MealTableModelProxy * proxy = new MealTableModelProxy;
		proxy->setSourceModel((QAbstractItemModel *)Database::Instance().CachedDistributor());

		MealFoodList * foodlist = new MealFoodList(this);
		this->addTab(foodlist, mn);
		
		foodlist->setProxyModel(proxy);

		proxy->setKey(mid);
		proxy->setRefDate(sel_meal_date);
		proxy->invalidate();
		foodlist->populateModel();
	}
	mtiw->setKey(mdid);
	this->addTab(mtiw, style()->standardIcon(QStyle::SP_FileDialogStart), tr("Actions"));
	lock = false;
	// by default it is tab 0 so no setCurrentIndex(0) required
// 	this->setCurrentIndex(0);
	this->tabChanged(0);
}

BatchTableModelProxy* MealTabWidget::getBatchProxyModel() const {
	return batch_proxy;
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
	this->setCurrentIndex(this->count()-1);
}

int MealTabWidget::getMealDayId() const {
	return meal_day_id;
}

void MealTabWidget::tabChanged(int tab) const {
	if (!lock)
		emit currentTabChanged(tab);
}


#include "MealTabWidget.moc"