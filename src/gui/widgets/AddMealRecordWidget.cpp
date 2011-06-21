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
#include "AddMealRecordWidget.h"
#include "Database.h"
#include "DataParser.h"
#include "TabMealWidget.h"

#include <QSqlQuery>
#include <QMessageBox>

AddMealRecordWidget::AddMealRecordWidget(const QDate & bd, const QDate & ed, QWidget * parent) :
										QWidget(parent) {
	setupUi(this);

	calendar->setDateRange(bd, ed);
	action_save->setIcon( action_save->style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_close->setIcon( action_close->style()->standardIcon(QStyle::SP_DialogCancelButton) );

	connect(action_save, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_close, SIGNAL(clicked(bool)), this, SLOT(setShown(bool)));

	connect(calendar, SIGNAL(selectionChanged()), this, SLOT(calDayChanged()));
}

AddMealRecordWidget::~AddMealRecordWidget() {
	FPR(__func__);
}

void AddMealRecordWidget::showW(QTreeWidgetItem * item, int /*column*/) {
	if (item and item->parent())
		return;

	if (item) {
		calendar->setSelectedDate(QDate::fromString(item->text(0), Qt::ISODate));
	} else {
		calendar->setSelectedDate(QDate::currentDate());
	}

	this->show();
}

bool AddMealRecordWidget::insert_record() {

	return true;
}

void AddMealRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddMealRecordWidget::update_model() {
// 	combo_batch->setModel(Database::Instance().CachedBatch());
// 	combo_batch->setModelColumn(2);

// 	Database & db = Database::Instance();
// 	completer_qty->setModel(db.CachedBatch());
// 	completer_qty->setCompletionColumn(MealTableModel::HQty);

}

#include "AddMealRecordWidget.moc"