/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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
#include "Database.h"
#include "DataParser.h"
#include "TabMealWidget.h"

TabMealWidget::TabMealWidget(QWidget * parent) : QWidget(parent), db(Database::Instance()),
												 modelproxy_meal(NULL) {

	mealcode[0] = tr("Breakfast");
	mealcode[1] = tr("2nd breakfast");
	mealcode[2] = tr("Lunch");
	mealcode[3] = tr("Diner");

	QSqlQuery mq;

	mq.exec("SELECT value FROM settings WHERE key='BeginOfTheCamp';");

	if (mq.next())
		bd = QDate::fromString(mq.value(0).toString(), "yyyy-MM-dd");

	mq.exec("SELECT value FROM settings WHERE key='EndOfTheCamp';");
	if (mq.next())
		ed = QDate::fromString(mq.value(0).toString(), "yyyy-MM-dd");

	setupUi(this);

	groupBox->setTitle(tr("Select meal to activate this section"));
	groupBox->setEnabled(false);

	amrw = new AddMealRecordWidget(bd, ed);
	afrw = new AddFoodRecordWidget();

	tree_meal->setAutoExpandDelay(0);
// 	tree_meal->setIndentation(0);

	activateUi(true);

	action_addday->setEnabled(false);
	tree_meal->setHeaderLabel("Day list");
// 	connect(list_meal, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(updateMealList(QListWidgetItem*)));

// 	connect(table_dist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
	connect(&db, SIGNAL(dbSaved()), amrw, SLOT(update_model()));
	connect(amrw, SIGNAL(dbmealupdated(bool)), this, SLOT(activateUi(bool)));
	connect(action_addday, SIGNAL(clicked(bool)), this, SLOT(add_mealday()));
// 	table_dist->setEditTriggers(QAbstractItemView::DoubleClicked);

	connect(tree_meal, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(expand_single_item(QTreeWidgetItem*, int)));
	connect(tree_meal, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), amrw, SLOT(showW(QTreeWidgetItem*,int)));
}

TabMealWidget::~TabMealWidget() {
	activateUi(false);
	if (amrw) delete amrw;
	if (afrw) delete afrw;
}

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void TabMealWidget::activateUi(bool activate) {
	if (activate) {
		if (modelproxy_meal) delete modelproxy_meal;
		modelproxy_meal = new MealTableModelProxy();
		modelproxy_meal->setSourceModel(db.CachedDistributor());
		list_food->setModel(modelproxy_meal);
		list_food->setModelColumn(DistributorTableModel::HBatchId);

		int total_days = bd.daysTo(ed);
	
		tree_meal->clear();
		for (int i = 0; i <= total_days; ++i) {
			QString date = bd.addDays(i).toString("yyyy-MM-dd");

			QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(date));
			QSqlQuery mq(QString("SELECT * FROM meal WHERE distdate='%1' ORDER BY mealtype ASC;").arg(date));

			mq.exec();
			while (mq.next()) {
				QString date = mq.value(TabMealWidget::MDate).toString();
				int mealtype = mq.value(TabMealWidget::MType).toInt();
				int mealid = mq.value(TabMealWidget::MId).toInt();
				QString mealname;
				if (mealtype > DistributorTableModel::MDiner) {
					mealname = mq.value(TabMealWidget::MName).toString();
				} else {
					mealname = mealcode[mealtype];
				}
				QTreeWidgetItem * child = new QTreeWidgetItem(QStringList(mealname));
				child->setData(0, Qt::UserRole, mq.value(TabMealWidget::MId));
// 				child->setData(0, Qt::UserRole+1, mealtype);
				item->insertChild(item->childCount(), child);
			}
			tree_meal->addTopLevelItem(item);
		}
	}
}

void TabMealWidget::edit_record(const QModelIndex& /*idx*/) {
// 	if (model_dist->isDirty(idx)) {
// 		table_dist->setEditTriggers(QAbstractItemView::DoubleClicked);
// 	} else {
// 		table_dist->setEditTriggers(QAbstractItemView::NoEditTriggers);
// 	}
}

void TabMealWidget::updateMealList(QTreeWidgetItem * item) {
	
	if (item and item->data(0, Qt::UserRole).isValid()) {
		modelproxy_meal->setRef(item->data(0, Qt::UserRole).toInt());
	} else {
		modelproxy_meal->setRef(-1);
	}

	modelproxy_meal->invalidate();
}

void TabMealWidget::add_mealday() {
	amrw->setWindowFlags(Qt::Tool);
	amrw->showW(NULL, 0);
}

void TabMealWidget::expand_single_item(QTreeWidgetItem * item, int /*column*/) {
	if (!item->parent()) {
		tree_meal->collapseAll();
		tree_meal->expandItem(item);
		updateMealList(NULL);
		groupBox->setTitle(tr("Select meal to activate this section"));
		groupBox->setEnabled(false);
	} else {
		updateMealList(item);
		groupBox->setTitle(item->parent()->text(0));
		groupBox->setEnabled(true);
	}
	
}

#include "TabMealWidget.moc"