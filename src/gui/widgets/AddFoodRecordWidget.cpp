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
#include "AddFoodRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

AddFoodRecordWidget::AddFoodRecordWidget(QWidget * parent) : QWidget(parent), completer_qty(NULL) {
	setupUi(this);

	action_add->setIcon( action_add->style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_clear->setIcon( action_add->style()->standardIcon(QStyle::SP_DialogDiscardButton) );

	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clear_form()));

	connect(combo_batch, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_batch, SIGNAL(editTextChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_qty, SIGNAL(textEdited(QString)), this, SLOT(validateAdd()));
}

AddFoodRecordWidget::~AddFoodRecordWidget() {
	FPR(__func__);
	if (completer_qty) delete completer_qty;
}

bool AddFoodRecordWidget::insert_record() {
	Database & db = Database::Instance();
	BatchTableModel * btm = db.CachedBatch();
	DistributorTableModel * dtm = db.CachedDistributor();

	int idx = combo_batch->currentIndex();
	int batch_id = btm->index(idx, 0).data().toInt();

	int row = dtm->rowCount();
	dtm->insertRows(row, 1);
// 	btm->setData(btm->index(row, DistributorTableModel::HId), row);
	dtm->setData(dtm->index(row, DistributorTableModel::HBatchId), batch_id);
	dtm->setData(dtm->index(row, DistributorTableModel::HQty), edit_qty->text());
	dtm->setData(dtm->index(row, DistributorTableModel::HDistDate), QDate::currentDate().toString(Qt::ISODate));
	dtm->setData(dtm->index(row, DistributorTableModel::HRegDate), QDate::currentDate().toString(Qt::ISODate));
	dtm->setData(dtm->index(row, DistributorTableModel::HReason), "meal");
	dtm->setData(dtm->index(row, DistributorTableModel::HReason2), "");
	dtm->setData(dtm->index(row, DistributorTableModel::HReason3), DistributorTableModel::RMeal);

	dtm->submitAll();
	db.updateBatchQty(dtm->index(row, 1).data(Qt::EditRole).toInt());
	btm->submitAll();
	update_model();	

	combo_batch->setFocus();
	return /*status*/ true;
}

void AddFoodRecordWidget::clear_form() {
	edit_qty->clear();
}

void AddFoodRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddFoodRecordWidget::validateAdd() {
	double qtyused = Database::Instance().CachedBatch()->index(combo_batch->currentIndex(), BatchTableModel::HUsedQty).data().toDouble();
	double qtytotal = Database::Instance().CachedBatch()->index(combo_batch->currentIndex(), BatchTableModel::HStaQty).data(Qt::EditRole).toDouble();

	QString max;
	label_maxvalue->setText(max.sprintf("%.2f", qtytotal-qtyused));

	bool qtyvalid = edit_qty->text().toDouble() > (qtytotal-qtyused) ? false : true;
	if (edit_qty->ok() and qtyvalid /*and edit_reason2->ok()*/) {
		action_add->setEnabled(true);
	} else {
		action_add->setEnabled(false);
	}
}

void AddFoodRecordWidget::update_model() {
	combo_batch->setModel(Database::Instance().CachedBatch());
	combo_batch->setModelColumn(2);
	PR(__LINE__);

	if (completer_qty) delete completer_qty;

	completer_qty = new QCompleter(edit_qty);

// 	Database & db = Database::Instance();
// 	completer_qty->setModel(db.CachedBatch());
// 	completer_qty->setCompletionColumn(FoodTableModel::HQty);

	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	edit_qty->setCompleter(completer_qty);
}


#include "AddFoodRecordWidget.moc"