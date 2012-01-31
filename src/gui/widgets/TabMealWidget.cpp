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
#include "config.h"
#include "Database.h"
#include "DataParser.h"
#include "TabMealWidget.h"
#include "TableDelegates.h"
#include "DBReports.h"

#include <QPushButton>
#include <QToolButton>
#include <QDesktopServices>
#include <QProgressDialog>

TabMealWidget::TabMealWidget(QWidget * parent) : QWidget(parent), db(Database::Instance()),
												 wmap(NULL), lock(false) {
	setupUi(this);

	activateUi(true);

	group_meals->setTitle(tr("Select meal to activate this section"));
	group_meals->setEnabled(false);

	group_meals->setEnabled(true);

	spin_scouts->setMaximum(9999);
	spin_leadres->setMaximum(9999);
	spin_others->setMaximum(9999);

	action_toggle->setFlat(true);
	action_toggle->setText("");
	action_toggle->setIcon (QIcon(":/resources/icons/view-resource-calendar.png"));
	action_toggle->setIconSize(QSize(48, 48));

	connect(calendar, SIGNAL(clicked(QDate)), this, SLOT(hightlight_day(QDate)));
	connect(action_insert, SIGNAL(clicked(bool)), this, SLOT(add_mealday()));
	connect(action_toggle, SIGNAL(toggled(bool)), this, SLOT(toggle_calendar(bool)));
	connect(list_meal, SIGNAL(clicked(QModelIndex)), this, SLOT(selectDay(QModelIndex)));

	connect(spin_scouts, SIGNAL(valueChanged(int)), this, SLOT(validateSpins()));
	connect(spin_leadres, SIGNAL(valueChanged(int)), this, SLOT(validateSpins()));
	connect(spin_others, SIGNAL(valueChanged(int)), this, SLOT(validateSpins()));

	connect(push_update, SIGNAL(clicked(bool)), this, SLOT(doUpdate()));
	connect(tab_meals, SIGNAL(currentTabChanged(int)), this, SLOT(mealTabChanged(int)));

	createPDF = new QAction(QIcon(":/resources/icons/application-pdf.png"), tr("Create && view PDF report"), this);
	createPDFAll = new QAction(QIcon(":/resources/icons/application-pdf.png"), tr("Create all PDF reports"), this);
	browsePDF = new QAction(style()->standardIcon(QStyle::SP_DirHomeIcon), tr("Browse reports directory"), this);

	tools->setPopupMode(QToolButton::InstantPopup);
	tools->setIcon(QIcon(":/resources/icons/tools-wizard.png"));
	tools->setText(tr("Tools"));
	tools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	tools->addAction(createPDF);
	tools->addAction(createPDFAll);
	tools->addAction(browsePDF);

	createPDF->setEnabled(false);

	connect(createPDF, SIGNAL(triggered(bool)), this, SLOT(doPrepareReport()));
	connect(createPDFAll, SIGNAL(triggered(bool)), this, SLOT(doPrepareReports()));
	connect(browsePDF, SIGNAL(triggered(bool)), this, SLOT(doBrowseReports()));

	seldate = QDate::currentDate().toString(Qt::ISODate);

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
		list_meal->setModel(db.CachedMealDay());
		list_meal->hideColumn(MealDayTableModel::HId);

		list_meal->horizontalHeader()->setResizeMode(MealDayTableModel::HMealDate, QHeaderView::Stretch);

		list_meal->setEditTriggers(QAbstractItemView::NoEditTriggers);

// 		list_meal->setSortingEnabled(true);
		list_meal->setSelectionBehavior(QAbstractItemView::SelectRows);
		list_meal->setSelectionMode(QAbstractItemView::SingleSelection);

		PriceDelegate * price_delegate = new PriceDelegate;
		list_meal->setItemDelegateForColumn(MealDayTableModel::HAvgCost, price_delegate);
		list_meal->update();

		label_data->setIndent(14);
		label_data->setText(QObject::tr("No day selected yet"));

		wmap = new QDataWidgetMapper;
		wmap->setModel(db.CachedMeal());
		wmap->addMapping(spin_scouts, MealTableModel::HScouts);
		wmap->addMapping(spin_leadres, MealTableModel::HLeaders);
		wmap->addMapping(spin_others, MealTableModel::HOthers);
		wmap->addMapping(label_data, MealTableModel::HAvgCosts);

		push_update->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
		push_update->setEnabled(false);
	}
}

/** @brief Add day of meal to database
 **/
// TODO: wyczysc kod z komentarza
void TabMealWidget::add_mealday() {
	db.addMealDayRecord(calendar->selectedDate(), 0);
	
	hightlight_day(calendar->selectedDate());
// 	, true, db.cs()->scoutsNo, db.cs()->leadersNo, 0, 0.0, "]:->"
	db.CachedMealDay()->select();
}

void TabMealWidget::toggle_calendar(bool show) {
	calendar->setVisible(show);
	action_insert->setVisible(show);
}

void TabMealWidget::hightlight_day(const QDate & date) {
	QModelIndexList ml = db.CachedMealDay()->match(db.CachedMealDay()->index(0, MealDayTableModel::HMealDate), Qt::EditRole, date.toString(Qt::ISODate));

	if (ml.count()) {
		list_meal->setCurrentIndex(ml.at(0));
		int mdid = db.CachedMealDay()->data(db.CachedMealDay()->index(ml.at(0).row(), MealDayTableModel::HId)).toInt();
		PR(mdid);
		action_insert->setEnabled(false);
		action_insert->setIcon(style()->standardIcon(QStyle::SP_DialogNoButton));
		selectDay(ml.at(0));
	} else {
		action_insert->setEnabled(true);
		action_insert->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
	}
}

void TabMealWidget::selectDay(const QModelIndex& idx) {
// 	wmap->setCurrentIndex(idx.row());
// 	tab_meals->setIndex(idx);
// 	QDate sd = db.CachedMealDay()->index(idx.row(), MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();
// 	seldate = sd.toString(Qt::ISODate);
// 	label_data->setText(QObject::tr("Selected day: <b>%1</b>").arg(sd.toString(Qt::DefaultLocaleLongDate)));
// 	createPDF->setEnabled(true);

	// TODO: Sprawdzic czy to co ponizej (lub powyzej) jest poprawne.

	// do not reload meal tabs while filling tab
	lock = true;

	// get meal day id
	int mdid = idx.model()->data(idx.model()->index(idx.row(), MealDayTableModel::HId), Qt::EditRole).toInt();

	// query meal tab for meals from this day
	tab_meals->setMealDayId(mdid);

	QDate sd = db.CachedMealDay()->index(idx.row(), MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();

	label_data->setText(QObject::tr("Selected day: <b>%1</b>").arg(sd.toString(Qt::DefaultLocaleLongDate)));
	createPDF->setEnabled(true);

	lock = false;
}

void TabMealWidget::validateSpins() {
// 	if (lock)
// 		return;

	if (!lock and ( spin_scouts->value() or spin_leadres->value() or spin_others->value() )) {
		push_update->setEnabled(true);
	} else {
		push_update->setEnabled(false);
	}
}

void TabMealWidget::mealTabChanged(int tab) {
	int tabsqty = tab_meals->count();

	if (tab == (tabsqty-1)) {
// 		((QSpinBox *)(mapper->mappedWidgetAt(MealTableModel::HScouts)))->setValue(100);
// 		((QSpinBox *)(mapper->mappedWidgetAt(MealTableModel::HScouts)))->setValue(100);
// 		((QSpinBox *)(mapper->mappedWidgetAt(MealTableModel::HScouts)))->setValue(100);
		spin_scouts->setEnabled(false);
		spin_leadres->setEnabled(false);
		spin_others->setEnabled(false);
		return;
	}

	spin_scouts->setEnabled(true);
	spin_leadres->setEnabled(true);
	spin_others->setEnabled(true);

	int i = tab_meals->currentIndex();
	int mid = ((MealFoodList *)tab_meals->widget(i))->proxyModel()->key();

	MealTableModel * mt = Database::Instance().CachedMeal();
	QModelIndexList meals = mt->match(mt->index(0, MealTableModel::HId), Qt::EditRole, mid, -1, Qt::MatchExactly);
	
	if (meals.count() != 1)
		return;

	lock = true;
	wmap->setCurrentIndex(meals.at(0).row());
	lock = false;
}

void TabMealWidget::doUpdate() {
	int mid = ((MealFoodList *)tab_meals->widget(tab_meals->currentIndex()))->proxyModel()->key();

	QSqlQuery q;
	q.prepare("UPDATE meal SET scouts=?, leaders=?, others=? WHERE id=?;");
	q.bindValue(0, spin_scouts->value());
	q.bindValue(1, spin_leadres->value());
	q.bindValue(2, spin_others->value());
	q.bindValue(3, mid);
	q.exec();
	Database::Instance().CachedMeal()->select();
}

void TabMealWidget::doPrepareReport() {
	QString fn;
	DBReports::printDailyMealReport(seldate, &fn);
	QDesktopServices::openUrl(QUrl("file://" % fn));
}

void TabMealWidget::doPrepareReports() {
	QString fn;
	int num = db.CachedMeal()->rowCount();

	QProgressDialog progress(tr("Printing reports..."), tr("&Cancel"), 0, num);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setValue(0);

	for (int i = 0; i < num; ++i) {
		QDate sd = db.CachedMealDay()->index(i, MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();

		progress.setValue(i);
		progress.setLabelText(tr("Creating report for day: ") % sd.toString(Qt::DefaultLocaleShortDate));
		if (progress.wasCanceled())
			break;

		DBReports::printDailyMealReport(sd.toString(Qt::ISODate), &fn);
	}
	progress.setValue(num);
}

void TabMealWidget::doBrowseReports() {
	Database & db = Database::Instance();

// 	QDesktopServices::openUrl(QUrl("file:///home"));
	QDesktopServices::openUrl(QUrl("file://" % QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % db.openedDatabase()));
}

#include "TabMealWidget.moc"