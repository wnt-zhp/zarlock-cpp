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
// 	amrw = new AddMealRecordWidget(bd, ed);
// 	afrw = new AddFoodRecordWidget();

// 	list_meal->setAutoExpandDelay(0);
// 	list_meal->setIndentation(0);

	activateUi(true);

	action_toggle->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
// 	action_addday->setEnabled(false);
// 	list_meal->setsetHeaderLabel("Day list");
// 	connect(, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(updateMealList(QListWidgetItem*)));

// 	connect(table_dist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));

	connect(action_insert, SIGNAL(clicked(bool)), this, SLOT(add_mealday()));
	connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(hightlight_day(QDate)));
	connect(action_toggle, SIGNAL(toggled(bool)), this, SLOT(toggle_calendar(bool)));

	action_toggle->setChecked(true);
	hightlight_day(QDate::currentDate());
}

TabMealWidget::~TabMealWidget() {
	FPR(__func__);
	activateUi(false);
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

		list_meal->setModel(db.CachedMeal());
		list_meal->setModelColumn(MealTableModel::HDistDate);
		list_meal->setEditTriggers(QAbstractItemView::NoEditTriggers);
		list_meal->setIconSize(QSize(100, 32));
		db.CachedMeal()->setSort(MealTableModel::HDistDate, Qt::AscendingOrder);
		list_meal->update();

// 		list_food->setModel(modelproxy_meal);
// 		list_food->setModelColumn(DistributorTableModel::HBatchId);
	}
}

void TabMealWidget::add_mealday() {
	db.addMealRecord(calendar->selectedDate().toString(Qt::ISODate), false, db.cs()->scoutsNo, db.cs()->leadersNo, 0, 0.0, "]:->");
	PR(db.cs()->scoutsNo);
	hightlight_day(calendar->selectedDate());
}

void TabMealWidget::toggle_calendar(bool show) {
	calendar->setVisible(show);
	if (show) {
		action_toggle->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
		hightlight_day(calendar->selectedDate());
	}
	else {
		action_toggle->setIcon(style()->standardIcon(QStyle::SP_TitleBarMinButton));
		action_insert->setEnabled(false);
	}
}

void TabMealWidget::hightlight_day(const QDate & date) {
	QModelIndexList ml = db.CachedMeal()->match(db.CachedMeal()->index(0, MealTableModel::HDistDate), Qt::EditRole, date.toString(Qt::ISODate));

	if (ml.count()) {
		list_meal->setCurrentIndex(ml.at(0));
		action_insert->setEnabled(false);
		action_insert->setIcon(style()->standardPixmap(QStyle::SP_DialogNoButton));
	} else {
		action_insert->setEnabled(true);
		action_insert->setIcon(style()->standardPixmap(QStyle::SP_DialogApplyButton));
	}
}

#include "TabMealWidget.moc"