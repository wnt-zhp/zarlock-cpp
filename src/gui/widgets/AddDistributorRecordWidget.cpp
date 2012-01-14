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
	completer_qty(NULL), completer_date(NULL), completer_reason(NULL), completer_reason2(NULL),
	pproxy(NULL), hideempty(NULL), indexToUpdate(NULL) {
	setupUi(parent);

	hideempty = new QCheckBox;
	hideempty->setChecked(true);

	action_add->setIcon( action_add->style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_clear->setIcon( action_add->style()->standardIcon(QStyle::SP_DialogDiscardButton) );

	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insertRecord()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clearForm()));
// 	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(cancel_form()));

	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_products, SIGNAL(editTextChanged(QString)), this, SLOT(validateAdd()));
	connect(spin_qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));
	connect(edit_date, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(edit_reason_a, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
// 	connect(edit_reason_b, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

	connect(&Database::Instance(), SIGNAL(distributorWordListUpdated()), this, SLOT(update_model()));

// 	connect(Database::Instance().CachedDistributor(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update_model()));
// 	edit_date->setDateReferenceObj(edit_book);
	edit_reason_b->enableEmpty();
	// TODO: do it better
	combo_products->setStyleSheet("color: black;");
}

AddDistributorRecordWidget::~AddDistributorRecordWidget() {
	FPR(__func__);
	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;
	if (pproxy) delete pproxy;
	delete hideempty;
}

bool AddDistributorRecordWidget::insertRecord() {
	Database & db = Database::Instance();

	int idx = combo_products->currentIndex();
	int batch_id = pproxy->mapToSource(pproxy->index(idx, 0)).data(BatchTableModel::RRaw).toInt();	//HBatchId

	QDate df;
	if (!DataParser::date(edit_date->text(), df))
		return false;

	if (indexToUpdate) {
		if (db.updateDistributorRecord(copyOfIndexToUpdate, batch_id, spin_qty->value(), df, QDate::currentDate(),
			DistributorTableModel::RGeneral, edit_reason_a->text(), edit_reason_b->text())) {
				indexToUpdate = NULL;
				prepareInsert(true);
				edit_date->setFocus();
			}
	} else {
		if (db.addDistributorRecord(batch_id, spin_qty->value(), df, QDate::currentDate(),
			DistributorTableModel::RGeneral, edit_reason_a->text(), edit_reason_b->text())) {
				prepareInsert(true);
				edit_date->setFocus();
			}
	}

	update_model();

	combo_products->setFocus();
	return true;
}

void AddDistributorRecordWidget::clearForm() {
	spin_qty->setValue(0.0);
	spin_qty->setSuffix("");
	edit_date->clear();
	edit_reason_a->clear();
	edit_reason_b->clear();
}

void AddDistributorRecordWidget::cancelForm() {
	prepareInsert(true);
	emit canceled(false);
}

void AddDistributorRecordWidget::validateAdd() {
	QModelIndex idx = pproxy->mapToSource(pproxy->index(combo_products->currentIndex(), 0));

	unsigned int fake = 0;
	unsigned int qtyused = Database::Instance().CachedBatch()->index(idx.row(), BatchTableModel::HUsedQty).data().toUInt();
	unsigned int qtytotal = Database::Instance().CachedBatch()->index(idx.row(), BatchTableModel::HStaQty).data(Qt::EditRole).toUInt();
	QString qunit = Database::Instance().CachedBatch()->index(idx.row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();

// 	if (indexToUpdate and (idx.row() == copyOfIndexToUpdate.row())) {
	if (indexToUpdate and (idx.row() == sourceRowToUpdate)) {
		fake = indexToUpdate->model()->index(indexToUpdate->row(), DistributorTableModel::HQty).data(DistributorTableModel::RRaw).toUInt();
	}

	unsigned int totalmax = qtytotal - qtyused + fake;

	label_unit->setText(tr("x %1").arg(qunit));
	spin_qty->setSuffix(tr(" of %1").arg(totalmax/100.0, 0, 'f', 2));
	spin_qty->setMaximum(totalmax/100.0);

	if (edit_date->ok()) {
		combo_products->setEnabled(true);
		spin_qty->setEnabled(true);
		edit_reason_a->setEnabled(true);
		edit_reason_b->setEnabled(true);
		pproxy->setDateKey(edit_date->date());
		pproxy->invalidate();
	} else {
		combo_products->setEnabled(false);
		spin_qty->setEnabled(false);
		edit_reason_a->setEnabled(false);
		edit_reason_b->setEnabled(false);
	}
	
	if ((spin_qty->value() > 0.0) and edit_date->ok() and edit_reason_a->ok()) {
		action_add->setEnabled(true);
	} else {
		action_add->setEnabled(false);
	}
}

void AddDistributorRecordWidget::update_model() {
	if (pproxy) delete pproxy;
	pproxy = new BatchTableModelProxy(hideempty);
 
	pproxy->setSourceModel(Database::Instance().CachedBatch());
	pproxy->setDynamicSortFilter(true);
	pproxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	pproxy->sort(2, Qt::AscendingOrder);
	combo_products->setModel(pproxy);
	combo_products->setModelColumn(2);

// 	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;

// 	completer_qty = new QCompleter(Database::Instance().DistributorWordList().at(Database::DWqty), edit_qty);
	completer_date = new QCompleter(Database::Instance().DistributorWordList().at(Database::DWdist), edit_date);
	completer_reason = new QCompleter(Database::Instance().DistributorWordList().at(Database::DWreason), edit_reason_a);
	completer_reason2 = new QCompleter(Database::Instance().DistributorWordList().at(Database::DWoptional), edit_reason_b);

// 	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_date->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason2->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

// 	edit_qty->setCompleter(completer_qty);
	edit_date->setCompleter(completer_date);
	edit_reason_a->setCompleter(completer_reason);
	edit_reason_b->setCompleter(completer_reason2);
}

void AddDistributorRecordWidget::prepareInsert(bool visible) {
	action_add->setText(tr("Insert record and exit"));
	indexToUpdate = NULL;
	if (visible) {
	} else {
		clearForm();
	}
}

void AddDistributorRecordWidget::prepareUpdate(const QModelIndex & idx) {
	unsigned int bid;
	unsigned int qty;

	QString spec, price, disttype_a, disttype_b;
	QDate reg, entry;
	DistributorTableModel::Reasons disttype;
	
	clearForm();
	update_model();

	copyOfIndexToUpdate = idx;
	indexToUpdate = &copyOfIndexToUpdate;	// save index to update

	BatchTableModel * bm = Database::Instance().CachedBatch();

	QModelIndex sidx = idx.model()->index(idx.row(), DistributorTableModel::HBatchId);	// source index for proxy fpr column HBatchID

	// Find batch record index for selected distributor index
	QModelIndexList batchl = bm->match(bm->index(0, BatchTableModel::HId), Qt::DisplayRole, sidx.data(DistributorTableModel::RRaw).toUInt());

	if (batchl.size() != 1)
		return;

	Database::Instance().getDistributorRecord(sidx, bid, qty, reg, entry, disttype, disttype_a, disttype_b);	// get record

	edit_date->setRaw(reg.toString("dd/MM/yyyy"));

	sourceRowToUpdate = batchl.at(0).row();		// source row number

	pproxy->setItemNum(&sourceRowToUpdate);		// update proxy
	validateAdd();								// revalidate proxy model

	combo_products->setCurrentIndex(pproxy->mapFromSource(batchl.at(0)).row());
	spin_qty->setValue(qty/100.0);
	edit_reason_a->setRaw(disttype_a);
	edit_reason_b->setRaw(disttype_b);
	
	action_add->setText(tr("Update record and exit"));
	validateAdd();								// revalidate proxy model
}

#include "AddDistributorRecordWidget.moc"