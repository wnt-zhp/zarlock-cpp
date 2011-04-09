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
#include "AddDistributorRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

AddDistributorRecordWidget::AddDistributorRecordWidget(QWidget * parent) : Ui::ADRWidget(),
	completer_qty(NULL), completer_date(NULL), completer_reason(NULL), completer_reason2(NULL) {
	setupUi(parent);

	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clear_form()));
// 	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(cancel_form()));

// 	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_products, SIGNAL(editTextChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_qty, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_date, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(edit_reason, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_reason2, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

// 	edit_date->setDateReferenceObj(edit_book);
	edit_reason2->enableEmpty();
}

AddDistributorRecordWidget::~AddDistributorRecordWidget() {
	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;
}

bool AddDistributorRecordWidget::insert_record() {
	Database & db = Database::Instance();
	BatchTableModel * btm = db.CachedBatch();
	DistributorTableModel * dtm = db.CachedDistributor();

	int idx = combo_products->currentIndex();
	int batch_id = btm->index(idx, 0).data().toInt();

	int row = dtm->rowCount();
	dtm->insertRows(row, 1);
// 	btm->setData(btm->index(row, DistributorTableModel::HId), row);
	dtm->setData(dtm->index(row, DistributorTableModel::HBatchId), batch_id);
	dtm->setData(dtm->index(row, DistributorTableModel::HQty), edit_qty->text());
	dtm->setData(dtm->index(row, DistributorTableModel::HDistDate), edit_date->text(true));
	dtm->setData(dtm->index(row, DistributorTableModel::HRegDate), QDate::currentDate().toString("dd/MM/yyyy"));
	dtm->setData(dtm->index(row, DistributorTableModel::HReason), edit_reason->text());
	dtm->setData(dtm->index(row, DistributorTableModel::HReason2), edit_reason2->text());
	dtm->setData(dtm->index(row, DistributorTableModel::HReason3), 0);
// 	bool status = dtm->submitAll();

	db.updateBatchQty(batch_id);

// 	clear_form();
	edit_qty->setFocus();
	return false /*status*/;
}

void AddDistributorRecordWidget::clear_form() {
	edit_qty->clear();
	edit_date->clear();
	edit_reason->clear();
	edit_reason2->clear();
}

void AddDistributorRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddDistributorRecordWidget::validateAdd() {
// 	PR(combo_products->currentIndex());

	if (edit_qty->ok() and edit_date->ok() and
		edit_reason->ok() /*and edit_reason2->ok()*/) {
		action_add->setEnabled(true);
	} else {
		action_add->setEnabled(false);
	}
}

void AddDistributorRecordWidget::update_model() {
	combo_products->setModel(Database::Instance().CachedBatch());
	combo_products->setModelColumn(2);
// 	combo_products->setEditable(true);

// 	completer = new QCompleter(combo_products->model(), this);
// 	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
// 	completer->setCompletionColumn(ProductsTableModel::HName);
// 	combo_products->setCompleter(completer);

	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;

	completer_qty = new QCompleter(edit_qty);
	completer_date = new QCompleter(edit_date);
	completer_reason = new QCompleter(edit_reason);
	completer_reason2 = new QCompleter(edit_reason2);

	Database & db = Database::Instance();
	completer_qty->setModel(db.CachedDistributor());
	completer_date->setModel(db.CachedDistributor());
	completer_reason->setModel(db.CachedDistributor());
	completer_reason2->setModel(db.CachedDistributor());

	completer_qty->setCompletionColumn(DistributorTableModel::HQty);
	completer_date->setCompletionColumn(DistributorTableModel::HDistDate);
	completer_reason->setCompletionColumn(DistributorTableModel::HReason);
	completer_reason2->setCompletionColumn(DistributorTableModel::HReason2);

	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_date->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason2->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	edit_qty->setCompleter(completer_qty);
	edit_date->setCompleter(completer_date);
	edit_reason->setCompleter(completer_reason);
	edit_reason2->setCompleter(completer_reason2);
}


#include "AddDistributorRecordWidget.moc"