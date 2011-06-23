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
												 wmap(NULL) {
	setupUi(this);

	activateUi(true);

	group_meals->setTitle(tr("Select meal to activate this section"));
	group_meals->setEnabled(false);

	group_meals->setEnabled(true);

	spin_scouts->setMaximum(9999);
	spin_leadres->setMaximum(9999);
	spin_others->setMaximum(9999);

	action_toggle->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));

	connect(action_insert, SIGNAL(clicked(bool)), this, SLOT(add_mealday()));
	connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(hightlight_day(QDate)));
	connect(action_toggle, SIGNAL(toggled(bool)), this, SLOT(toggle_calendar(bool)));
	connect(list_meal, SIGNAL(clicked(QModelIndex)), this, SLOT(selectDay(QModelIndex)));

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
		db.CachedMeal()->setDirtyIcon(style()->standardIcon(QStyle::SP_BrowserReload));
// 		db.CachedMeal()->select();

		list_meal->setModel(db.CachedMeal());
		list_meal->hideColumn(MealTableModel::HId);
		list_meal->hideColumn(MealTableModel::HDirty);
		list_meal->hideColumn(MealTableModel::HNotes);

		list_meal->horizontalHeader()->setResizeMode(MealTableModel::HDistDate, QHeaderView::Stretch);
		list_meal->horizontalHeader()->setResizeMode(MealTableModel::HScouts, QHeaderView::ResizeToContents);
		list_meal->horizontalHeader()->setResizeMode(MealTableModel::HLeaders, QHeaderView::ResizeToContents);
		list_meal->horizontalHeader()->setResizeMode(MealTableModel::HOthers, QHeaderView::ResizeToContents);
		list_meal->horizontalHeader()->setResizeMode(MealTableModel::HAvgCosts, QHeaderView::ResizeToContents);

		list_meal->setEditTriggers(QAbstractItemView::NoEditTriggers);

// 		list_meal->setSortingEnabled(true);
		list_meal->setSelectionBehavior(QAbstractItemView::SelectRows);
		list_meal->setSelectionMode(QAbstractItemView::SingleSelection);

		list_meal->update();

		wmap = new QDataWidgetMapper;
		wmap->setModel(db.CachedMeal());
		wmap->addMapping(spin_scouts, MealTableModel::HScouts);
		wmap->addMapping(spin_leadres, MealTableModel::HLeaders);
		wmap->addMapping(spin_others, MealTableModel::HOthers);
// 		wmap->addMapping(label_data, MealTableModel::HAvgCosts);

		calculate->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
	}
}

void TabMealWidget::add_mealday() {
	db.addMealRecord(calendar->selectedDate().toString(Qt::ISODate), true, db.cs()->scoutsNo, db.cs()->leadersNo, 0, 0.0, "]:->");
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
		action_insert->setIcon(style()->standardIcon(QStyle::SP_DialogNoButton));
		selectDay(ml.at(0));
	} else {
		action_insert->setEnabled(true);
		action_insert->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
	}
}

void TabMealWidget::selectDay(const QModelIndex& idx) {
	wmap->setCurrentIndex(idx.row());
	tab_meals->setDateRef(db.CachedMeal()->index(idx.row(), MealTableModel::HDistDate).data(Qt::EditRole).toString());
}

#include "TabMealWidget.moc"