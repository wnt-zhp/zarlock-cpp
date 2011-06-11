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
										QWidget(parent), completer_other(NULL) {
	setupUi(this);

	calendar->setDateRange(bd, ed);
	action_save->setIcon( action_save->style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_close->setIcon( action_close->style()->standardIcon(QStyle::SP_DialogCancelButton) );

	connect(action_save, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_close, SIGNAL(clicked(bool)), this, SLOT(setShown(bool)));

	connect(calendar, SIGNAL(selectionChanged()), this, SLOT(calDayChanged()));
	connect(edit_other1, SIGNAL(textEdited(QString)), this, SLOT(validateAdd()));
	connect(edit_other2, SIGNAL(textEdited(QString)), this, SLOT(validateAdd()));
}

AddMealRecordWidget::~AddMealRecordWidget() {
	FPR(__func__);
	if (completer_other) delete completer_other;
}

void AddMealRecordWidget::showW(QTreeWidgetItem * item, int /*column*/) {
	if (item and item->parent())
		return;

	if (item) {
		calendar->setSelectedDate(QDate::fromString(item->text(0), "yyyy-MM-dd"));
	} else {
		calendar->setSelectedDate(QDate::currentDate());
	}
	calDayChanged();

	this->show();
}

bool AddMealRecordWidget::insert_record() {
	Database & db = Database::Instance();
	BatchTableModel * btm = db.CachedBatch();
	DistributorTableModel * dtm = db.CachedDistributor();

	QMessageBox::warning(this, tr("Deleting meals"), tr("When you remove meal from the list all distributor records associated to this meal will be removed as w ell."));
	QString qadd("INSERT INTO meal VALUES (NULL, '%1', %2, '%3', %4, %5);");
	QString qdel("DELETE FROM meal WHERE id=%1;");
	QString qsel("SELECT * FROM meal WHERE distdate='%1';");
	QSqlQuery qsql;

	qsql.exec(qsel.arg(calendar->selectedDate().toString("yyyy-MM-dd")));
	bool recexisting[DistributorTableModel::MOther2+1] = { false, false, false, false, false, false };
	int recexistingid[DistributorTableModel::MOther2+1];
	while (qsql.next()) {
		recexisting[qsql.value(TabMealWidget::MType).toInt()] = true;
		recexistingid[qsql.value(TabMealWidget::MType).toInt()] = qsql.value(TabMealWidget::MId).toInt();
	}

	for (int i = DistributorTableModel::MBreakfast; i <= DistributorTableModel::MOther2; ++i) {
		switch (i) {
			case DistributorTableModel::MBreakfast:
				PR(recexisting[DistributorTableModel::MBreakfast]);
				PR(cb_breakfast->isChecked());
				PR(qdel.arg(recexistingid[DistributorTableModel::MBreakfast]).toStdString());
				if (!recexisting[DistributorTableModel::MBreakfast] and cb_breakfast->isChecked()) {
					qsql.exec(qadd.arg(calendar->selectedDate().toString("yyyy-MM-dd")).arg(DistributorTableModel::MBreakfast).arg("").arg(0).arg(0.0));
				}
				if (recexisting[DistributorTableModel::MBreakfast] and !cb_breakfast->isChecked()) {
					qsql.exec(qdel.arg(recexistingid[DistributorTableModel::MBreakfast]));
				}
				PR(qsql.executedQuery().toStdString());
				break;
			case DistributorTableModel::M2Breakfast:
				if (!recexisting[DistributorTableModel::M2Breakfast] and cb_breakfast2nd->isChecked()) {
					qsql.exec(qadd.arg(calendar->selectedDate().toString("yyyy-MM-dd")).arg(DistributorTableModel::M2Breakfast).arg("").arg(0).arg(0.0));
				}
				if (recexisting[DistributorTableModel::M2Breakfast] and !cb_breakfast2nd->isChecked()) {
					qsql.exec(qdel.arg(recexistingid[DistributorTableModel::M2Breakfast]));
				}
				break;
			case DistributorTableModel::MLunch:
				if (!recexisting[DistributorTableModel::MLunch] and cb_lunch->isChecked()) {
					qsql.exec(qadd.arg(calendar->selectedDate().toString("yyyy-MM-dd")).arg(DistributorTableModel::MLunch).arg("").arg(0).arg(0.0));
				}
				if (recexisting[DistributorTableModel::MLunch] and !cb_lunch->isChecked()) {
					qsql.exec(qdel.arg(recexistingid[DistributorTableModel::MLunch]));
				}
				break;
			case DistributorTableModel::MDiner:
				if (!recexisting[DistributorTableModel::MDiner] and cb_dinner->isChecked()) {
					qsql.exec(qadd.arg(calendar->selectedDate().toString("yyyy-MM-dd")).arg(DistributorTableModel::MDiner).arg("").arg(0).arg(0.0));
				}
				if (recexisting[DistributorTableModel::MDiner] and !cb_dinner->isChecked()) {
					qsql.exec(qdel.arg(recexistingid[DistributorTableModel::MDiner]));
				}
				break;
			case DistributorTableModel::MOther1:
				if (!recexisting[DistributorTableModel::MOther1] and cb_other1->isChecked()) {
					qsql.exec(qadd.arg(calendar->selectedDate().toString("yyyy-MM-dd")).arg(DistributorTableModel::MOther1).arg("").arg(0).arg(0.0));
				}
				if (recexisting[DistributorTableModel::MOther1] and !cb_other1->isChecked()) {
					qsql.exec(qdel.arg(recexistingid[DistributorTableModel::MOther1]));
				}
				break;
			case DistributorTableModel::MOther2:
				if (!recexisting[DistributorTableModel::MOther2] and cb_other2->isChecked()) {
					qsql.exec(qadd.arg(calendar->selectedDate().toString("yyyy-MM-dd")).arg(DistributorTableModel::MOther2).arg("").arg(0).arg(0.0));
				}
				if (recexisting[DistributorTableModel::MOther2] and !cb_other2->isChecked()) {
					qsql.exec(qdel.arg(recexistingid[DistributorTableModel::MOther2]));
				}
				break;
		}
	}

	emit dbmealupdated(true);

	return true;
}

void AddMealRecordWidget::clear_form() {
	edit_other1->clear();
	edit_other2->clear();
}

void AddMealRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddMealRecordWidget::validateAdd() {
// 	float qtyused = Database::Instance().CachedBatch()->index(combo_batch->currentIndex(), BatchTableModel::HUsedQty).data().toFloat();
// 	float qtytotal = Database::Instance().CachedBatch()->index(combo_batch->currentIndex(), BatchTableModel::HStaQty).data(Qt::EditRole).toFloat();

	QString max;
// 	label_maxvalue->setText(max.sprintf("%.2f", qtytotal-qtyused));

// 	bool qtyvalid = edit_qty->text().toFloat() > (qtytotal-qtyused) ? false : true;
// 	if (edit_qty->ok() and qtyvalid /*and edit_reason2->ok()*/) {
// 		action_add->setEnabled(true);
// 	} else {
// 		action_add->setEnabled(false);
// 	}
}

void AddMealRecordWidget::update_model() {
// 	combo_batch->setModel(Database::Instance().CachedBatch());
// 	combo_batch->setModelColumn(2);
	if (completer_other) delete completer_other;
	completer_other = new QCompleter();

// 	Database & db = Database::Instance();
// 	completer_qty->setModel(db.CachedBatch());
// 	completer_qty->setCompletionColumn(MealTableModel::HQty);

	completer_other->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	edit_other1->setCompleter(completer_other);
	edit_other2->setCompleter(completer_other);
}

void AddMealRecordWidget::set_caldate(const QDate& /*date*/) {
}

void AddMealRecordWidget::calDayChanged() {
	QString date = calendar->selectedDate().toString("yyyy-MM-dd");

	QSqlQuery q(QString("SELECT * FROM meal WHERE distdate='%1';").arg(date));
	q.exec();

	int records = 0;

	cb_breakfast->setChecked(false);
	cb_breakfast2nd->setChecked(false);
	cb_lunch->setChecked(false);
	cb_dinner->setChecked(false);
	cb_other1->setChecked(false);
	cb_other2->setChecked(false);

	while(q.next()) {
		++records;
		switch (q.value(2).toInt()) {
			case 0:
				cb_breakfast->setChecked(true);
				break;
			case 1:
				cb_breakfast2nd->setChecked(true);
				break;
			case 2:
				cb_lunch->setChecked(true);
				break;
			case 3:
				cb_dinner->setChecked(true);
				break;
			case 4:
				cb_other1->setChecked(true);
				edit_other1->setText(q.value(3).toString());
				break;
			case 5:
				cb_other2->setChecked(true);
				edit_other2->setText(q.value(3).toString());
				break;
		}
	}

	if (records == 0) {
		cb_breakfast->setChecked(true);
		cb_breakfast2nd->setChecked(true);
		cb_lunch->setChecked(true);
		cb_dinner->setChecked(true);
	}
}


#include "AddMealRecordWidget.moc"