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
#include <QInputDialog>

MealTabWidget::MealTabWidget(QWidget* parent): QTabWidget(parent), open_item(NULL), mtiw(NULL) {
	CI();
	this->setDocumentMode(true);

	// Popup menu dla akcji usuwania rekordu z bazy.
	tab_action_rename = new QAction(tr("&Rename"), this);
	// 	removeRec->setShortcut(QKeySequence::Delete);
	tab_action_rename->setToolTip(tr("Remove record from database"));
	
	//  Łączymy akcję kliknięcia w menu "Remove" z funkcją (slotem), która to wykona.
	// 	connect(removeRec, SIGNAL(triggered()), this, SLOT(removeRecord()));
	tab_context_menu.addAction(tab_action_rename);


	mtiw = new MealTabInsertWidget(this);

	che = new QCheckBox;
	che->setChecked(true);
	cexp = new QCheckBox;
	cexp->setChecked(false);

	batch_proxy = new BatchTableModelProxy(cexp, NULL, NULL, che);
	batch_proxy->setSourceModel(Database::Instance()->CachedBatch());
	batch_proxy->setDynamicSortFilter(true);
	batch_proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	batch_proxy->sort(2, Qt::AscendingOrder);
	batch_proxy->invalidate();

	this->setTabsClosable(true);

	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(mtiw, SIGNAL(mealInserted(int)), this, SLOT(reloadTabs(int)));

	tab_handler.reserve(20);

	this->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->tabBar(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuEvent(QPoint)));
}

MealTabWidget::~MealTabWidget() {
	DI();
	delete batch_proxy;
	delete che;
	delete cexp;
}

void MealTabWidget::setMealDayId(int mdid) {
	if (meal_day_id == mdid)
		return;

	open_item = 0;
	meal_day_id = mdid;

	MealDayTableModel * mdt = Database::Instance()->CachedMealDay();
	MealTableModel * mt = Database::Instance()->CachedMeal();

	QModelIndexList mdl = mdt->match(mdt->index(0, MealDayTableModel::HId), Qt::EditRole, QVariant(mdid), -1, Qt::MatchExactly);
	// if there is no days then return
	if (mdl.count() != 1)
		return;

	QDate sel_meal_date = mdt->index(mdl.at(0).row(), MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();

	batch_proxy->setDateKey(sel_meal_date);
	batch_proxy->invalidate();

	mt->sort(MealTableModel::HMealKind, Qt::AscendingOrder);
	QModelIndexList meals = mt->match(mt->index(0, MealTableModel::HMealDay), Qt::EditRole, mdid, -1, Qt::MatchExactly);

	int last_selected_meal = this->currentIndex();

	this->clear();

	qDeleteAll(tab_handler);
	tab_handler.clear();

	mtiw->setKey(mdid);
	this->addTab(mtiw, style()->standardIcon(QStyle::SP_FileDialogStart), tr("Actions"));

	for (int i = meals.size()-1; i >=0; --i) {
		int mid = mt->index(meals.at(i).row(), MealTableModel::HId).data().toInt();
		QString mn = mt->index(meals.at(i).row(), MealTableModel::HMealName).data().toString();

		MealTableModelProxy * proxy = new MealTableModelProxy;
		proxy->setSourceModel((QAbstractItemModel *)Database::Instance()->CachedDistributor());

		MealFoodList * foodlist = new MealFoodList(this);
		this->insertTab(0, foodlist, mn);
		tab_handler.push_back(foodlist);
		
		foodlist->setProxyModel(proxy);

		proxy->setKey(mid);
		proxy->setRefDate(sel_meal_date);
		proxy->invalidate();
		foodlist->populateModel();
	}

	if (last_selected_meal < this->count()) {
		this->setCurrentIndex(last_selected_meal);
	}
}

BatchTableModelProxy* MealTabWidget::getBatchProxyModel() const {
	return batch_proxy;
}

void MealTabWidget::markOpenItem(QListWidgetItem* item) {
	open_item = item;
}

void MealTabWidget::closeOpenItems() {
	if (open_item)
		((MealFoodListItemDataWidget *)open_item->listWidget()->itemWidget(open_item))->buttonClose();
	open_item = NULL;
}

void MealTabWidget::reloadTabs(int mealDayId) {
	setMealDayId(mealDayId);
	this->setCurrentIndex(this->count()-1);
}

void MealTabWidget::closeTab(int index) {
	// Nie usuwamy zakłaki z dodawaniem posiłków
	if (index == this->count()-1) {
		return;
	}

	const MealTableModelProxy * p = ((MealFoodList * )this->widget(index))->proxyModel();

	int entries = p->rowCount();
	if (entries) {
		QMessageBox mbox(QMessageBox::Information, tr("Remove meal"),
						 tr("This meal contain %1 distributed parties. Remove them all first before you delete meal.").arg(entries),
						 QMessageBox::Ok);
		mbox.exec();
	} else {
		QMessageBox mbox(QMessageBox::Question, tr("Remove meal"),
						 tr("Are you sure to remove this meal?"),
						 QMessageBox::Yes | QMessageBox::No);
		if (mbox.exec() == QMessageBox::Yes) {
			int mid = ((MealFoodList * )this->widget(index))->proxyModel()->key();

			QVector<int> v;
			v.push_back(mid);
			if (Database::Instance()->removeMealRecord(v))
				this->removeTab(index);
		}
	}
}

void MealTabWidget::customContextMenuEvent(const QPoint & point) {

	QTabBar * tb = tabBar();
	QPoint gpoint = tb->mapToGlobal(point);

	int t = tb->tabAt(point);

	if (t == -1 or t == (this->count()-1))
		return;

	QAction * ac = tab_context_menu.exec(gpoint);

	bool is_ok = false;
	if (ac == tab_action_rename) {
		QString newname = QInputDialog::getText(this, tr("Meal name rename"), tr("Set new meal name"), QLineEdit::Normal, tb->tabText(t), &is_ok);
		if (is_ok && !newname.isEmpty()) {
			MealTableModel * mt = Database::Instance()->CachedMeal();

			mt->sort(MealTableModel::HMealKind, Qt::AscendingOrder);
			QModelIndexList meals = mt->match(mt->index(0, MealTableModel::HMealDay), Qt::EditRole, meal_day_id, -1, Qt::MatchExactly);

			mt->setData(mt->index(meals.at(t).row(), MealTableModel::HMealName), newname, Qt::EditRole);
			if (!mt->submit())
				mt->revert();
			else
				tb->setTabText(t, newname);
		}
	}
}


#include "MealTabWidget.moc"