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

#include <QPushButton>
#include <QToolButton>

TabMealWidget::TabMealWidget(QWidget * parent) : QWidget(parent), db(Database::Instance()),
												 modelproxy_meal(NULL) {

	mealcode[0] = tr("Breakfast");
	mealcode[1] = tr("2nd breakfast");
	mealcode[2] = tr("Lunch");
	mealcode[3] = tr("Diner");

// 	QSqlQuery mq;

// 	mq.exec("SELECT value FROM settings WHERE key='BeginOfTheCamp';");

// 	if (mq.next())
// 		bd = QDate::fromString(mq.value(0).toString(), Qt::ISODate);

// 	mq.exec("SELECT value FROM settings WHERE key='EndOfTheCamp';");
// 	if (mq.next())
// 		ed = QDate::fromString(mq.value(0).toString(), Qt::ISODate);

	bd = Database::Instance().cs()->campDateBegin;
	ed = Database::Instance().cs()->campDateEnd;

	setupUi(this);

	group_meals->setTitle(tr("Select meal to activate this section"));
	group_meals->setEnabled(false);

// 	QPushButton * b = new QPushButton("test");
// // 	tab_meals->printTabRect();
// 	tab_meals->addTab(b, "tescik");
// // 	tab_meals->printTabRect();
// 	tab_meals->addTab(tb, "tescik2");
// // 	tab_meals->printTabRect();
// 	tab_meals->addTab(b, "tescik");
// // 	tab_meals->printTabRect();
// 	tab_meals->addTab(b, "tescik");
// // 	tab_meals->printTabRect();
// 	tab_meals->addTab(b, "tescik");
// // 	tab_meals->printTabRect();
// 	tab_meals->addTab(b, "tescik");


// 	group_meals->layout()->addWidget(tb);

	group_meals->setEnabled(true);
	amrw = new AddMealRecordWidget(bd, ed);
	afrw = new AddFoodRecordWidget();

// 	list_meal->setAutoExpandDelay(0);
// 	list_meal->setIndentation(0);

	activateUi(true);

// 	action_addday->setEnabled(false);
// 	list_meal->setsetHeaderLabel("Day list");
// 	connect(, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(updateMealList(QListWidgetItem*)));

// 	connect(table_dist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
	connect(&db, SIGNAL(dbSaved()), amrw, SLOT(update_model()));
// 	connect(amrw, SIGNAL(dbmealupdated(bool)), this, SLOT(activateUi(bool)));
	connect(amrw, SIGNAL(dbmealupdated(bool)), this, SLOT(doRefresh()));
	connect(action_addday, SIGNAL(clicked(bool)), this, SLOT(add_mealday()));
// 	table_dist->setEditTriggers(QAbstractItemView::DoubleClicked);

	connect(list_meal, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(updateMealList(QListWidgetItem*)));
// 	connect(list_meal, SIGNAL(itemDoubleClicked(QListWidgetItem*, int)), amrw, SLOT(showW(QListWidgetItem*,int)));

	doRefresh();
}

TabMealWidget::~TabMealWidget() {
	FPR(__func__);
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
// 		list_food->setModel(modelproxy_meal);
// 		list_food->setModelColumn(DistributorTableModel::HBatchId);

// 		int total_days = bd.daysTo(ed);
// 	
// 		list_meal->clear();
// 
// 		for (int i = 0; i <= total_days; ++i) {
// 			QString date = bd.addDays(i).toString(Qt::ISODate);
// 
// 			QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(date));
// 			QSqlQuery mq(QString("SELECT * FROM meal WHERE distdate='%1' ORDER BY mealtype ASC;").arg(date));
// 
// 			mq.exec();
// 			while (mq.next()) {
// 				QString date = mq.value(TabMealWidget::MDate).toString();
// 				int mealtype = mq.value(TabMealWidget::MType).toInt();
// 				int mealid = mq.value(TabMealWidget::MId).toInt();
// 				QString mealname;
// 				if (mealtype > DistributorTableModel::MDiner) {
// 					mealname = mq.value(TabMealWidget::MName).toString();
// 				} else {
// 					mealname = mealcode[mealtype];
// 				}
// 				QTreeWidgetItem * child = new QTreeWidgetItem(QStringList(mealname));
// 				child->setData(0, Qt::UserRole, mq.value(TabMealWidget::MId));
// // 				child->setData(0, Qt::UserRole+1, mealtype);
// 				item->insertChild(item->childCount(), child);
// 			}
// 			list_meal->addTopLevelItem(item);
// 		}
	}
}

void TabMealWidget::doRefresh() {
	QSqlQuery mq("SELECT * FROM meal ORDER BY distdate ASC, mealtype ASC;");

	QListWidgetItem * item;

	mq.exec();
	while (mq.next()) {
		QString date = mq.value(TabMealWidget::MDate).toDate().toString(Qt::DefaultLocaleShortDate);
		QString rdate = mq.value(TabMealWidget::MDate).toDate().toString(Qt::ISODate);

		QList<QListWidgetItem *> ml = list_meal->findItems(date, Qt::MatchExactly);
		if (ml.size() == 0){
			item = new QListWidgetItem(date);
			item->setData(Qt::UserRole, rdate);
			item->setTextAlignment(4);
			item->setSizeHint(QSize(0, 18));
			list_meal->addItem(item);
		} else {
			item = ml.at(0);
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

void TabMealWidget::updateMealList(QListWidgetItem * item) {
	if (item and item->data(Qt::DisplayRole).isValid()) {
		tab_meals->setDateRef(item->data(Qt::UserRole).toString());
		group_meals->setTitle(item->text());
	} else {
		tab_meals->setDateRef("");
	}
}

void TabMealWidget::add_mealday() {
	amrw->setWindowFlags(Qt::Tool);
	amrw->showW(NULL, 0);
}

#include "TabMealWidget.moc"