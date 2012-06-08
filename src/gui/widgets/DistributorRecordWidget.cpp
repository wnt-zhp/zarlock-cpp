/*
    This file is part of Zarlok.

    Copyright (C) 2012  Rafał Lalik <rafal.lalik@zhp.net.pl>

    Zarlok is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zarlok is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "globals.h"
#include "DistributorRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

DistributorRecordWidget::DistributorRecordWidget(QWidget * parent) : AbstractRecordWidget(), Ui::DRWidget(),
	completer_qty(NULL), completer_date(NULL), completer_reason(NULL), completer_reason2(NULL),
	pproxy(NULL), hideempty(NULL), disttype(0) {
	setupUi(parent);

	button_label_insert_and_next = action_addnext->text();
	button_label_insert_and_exit = action_addexit->text();
	button_label_close = action_cancel->text();

	action_addnext->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_addexit->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_clear->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton) );
	action_cancel->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton) );

	action_cancel->setShortcut(Qt::Key_Escape);

	connect(action_addnext, SIGNAL(clicked(bool)), this, SLOT(insertRecord()));
	connect(action_addexit, SIGNAL(clicked(bool)), this, SLOT(insertRecordAndExit()));
	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(closeForm()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clearForm()));

	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_products, SIGNAL(editTextChanged(QString)), this, SLOT(validateAdd()));
	connect(spin_qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));
	connect(edit_date, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(edit_reason_a, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
// 	connect(edit_reason_b, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

	connect(Database::Instance(), SIGNAL(distributorWordListUpdated()), this, SLOT(update_model()));

// 	connect(db->CachedDistributor(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update_model()));
// 	edit_date->setDateReferenceObj(edit_book);
	edit_reason_b->enableEmpty();
	// TODO: do it better
	combo_products->setStyleSheet("color: black;");

	hideempty = new QCheckBox;
	hideempty->setChecked(true);

	pproxy = new BatchTableModelProxy(hideempty);
	pproxy->setExtendedSpec(true);
	update_model();
}

DistributorRecordWidget::~DistributorRecordWidget() {
	FPR(__func__);
	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;
	if (pproxy) delete pproxy;
	delete hideempty;
}

void DistributorRecordWidget::insertRecord() {
	Database * db = Database::Instance();

	int idx = combo_products->currentIndex();
	int batch_id = pproxy->mapToSource(pproxy->index(idx, 0)).data(BatchTableModel::RRaw).toInt();	//HBatchId

	QDate df;
	if (!DataParser::date(edit_date->text(), df))
		return;

	if (idToUpdate >= 0) {
		DistributorTableModel * dtm = db->CachedDistributor();
		QModelIndexList dl = dtm->match(dtm->index(0, DistributorTableModel::HId), Qt::EditRole, idToUpdate, 1, Qt::MatchExactly);
		if (dl.size() != 1)
			return;
		if (db->updateDistributorRecord(dl.at(0).row(), batch_id, spin_qty->value()*100, df, QDate::currentDate(),
			disttype, edit_reason_a->text(), edit_reason_b->text()))
		{
			idToUpdate = 0;
			clearForm();
			edit_date->setFocus();
		}
	} else {
		if (db->addDistributorRecord(batch_id, spin_qty->value()*100, df, QDate::currentDate(),
			disttype, edit_reason_a->text(), edit_reason_b->text()))
		{
			clearForm();
			edit_date->setFocus();
		}
	}
}

void DistributorRecordWidget::clearForm() {
	spin_qty->setValue(0.0);
	spin_qty->setSuffix("");
	edit_date->clear();
	edit_reason_a->clear();
	edit_reason_b->clear();
}

void DistributorRecordWidget::validateAdd() {
	QModelIndex idx = pproxy->mapToSource(pproxy->index(combo_products->currentIndex(), 0));

	Database * db = Database::Instance();

	unsigned int fake = 0;
	unsigned int qtyused = db->CachedBatch()->index(idx.row(), BatchTableModel::HUsedQty).data(Qt::EditRole).toUInt();
	unsigned int qtytotal = db->CachedBatch()->index(idx.row(), BatchTableModel::HStaQty).data(Qt::EditRole).toUInt();
	QString qunit = db->CachedBatch()->index(idx.row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();

// 	if (indexToUpdate and (idx.row() == copyOfIndexToUpdate.row())) {
	if (idToUpdate and (idx.row() == sourceRowToUpdate)) {
		fake = db->CachedDistributor()->getIndexById(idToUpdate, DistributorTableModel::HQty).data(DistributorTableModel::RRaw).toUInt();
	}

	unsigned int totalmax = qtytotal - qtyused + fake;

	label_unit->setText(tr("x %1").arg(qunit));
	spin_qty->setSuffix(tr(" of %1").arg(totalmax/100.0, 0, 'f', 2));
	spin_qty->setMaximum(totalmax/100.0);

	bool is_date_ok = edit_date->ok();
	if (is_date_ok) {
		pproxy->setDateKey(edit_date->date());
		pproxy->invalidate();
	}

	combo_products->setEnabled(is_date_ok);
	spin_qty->setEnabled(is_date_ok);
	edit_reason_a->setEnabled(is_date_ok);
	edit_reason_b->setEnabled(is_date_ok);

	if ((spin_qty->value() > 0.0) and is_date_ok and edit_reason_a->ok()) {
		action_addnext->setEnabled(true);
	} else {
		action_addnext->setEnabled(false);
	}
}

void DistributorRecordWidget::update_model() {
	Database * db = Database::Instance();

	pproxy->setSourceModel(db->CachedBatch());
	pproxy->setDynamicSortFilter(true);
	pproxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	pproxy->sort(2, Qt::AscendingOrder);

	combo_products->setModel(pproxy);
	combo_products->setModelColumn(2);

// 	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;

// 	completer_qty = new QCompleter(db->DistributorWordList().at(Database::DWqty), edit_qty);
	completer_date = new QCompleter(db->DistributorWordList().at(Database::DWdist), edit_date);
	completer_reason = new QCompleter(db->DistributorWordList().at(Database::DWreason), edit_reason_a);
	completer_reason2 = new QCompleter(db->DistributorWordList().at(Database::DWoptional), edit_reason_b);

// 	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_date->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason2->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

// 	edit_qty->setCompleter(completer_qty);
	edit_date->setCompleter(completer_date);
	edit_reason_a->setCompleter(completer_reason);
	edit_reason_b->setCompleter(completer_reason2);
}

void DistributorRecordWidget::prepareUpdate(const QModelIndex & idx) {
	int bid;
	int qty;

	QString spec, price, disttype_a, disttype_b;
	QDate reg, entry;
	int disttype;
	
	clearForm();
	update_model();

	Database * db = Database::Instance();

	BatchTableModel * bm = db->CachedBatch();

	QModelIndex sidx = idx.model()->index(idx.row(), DistributorTableModel::HBatchId);

	idToUpdate = sidx.model()->index(sidx.row(), DistributorTableModel::HId).data(Qt::EditRole).toUInt();

	// Find batch record index for selected distributor index
	QModelIndexList batchl = bm->match(bm->index(0, BatchTableModel::HId), Qt::DisplayRole, sidx.data(DistributorTableModel::RRaw).toUInt(), -1, Qt::MatchExactly);

	if (batchl.size() != 1)
		return;

	db->getDistributorRecord(sidx.row(), bid, qty, reg, entry, disttype, disttype_a, disttype_b);	// get record

	edit_date->setRaw(reg.toString("dd/MM/yyyy"));

	sourceRowToUpdate = batchl.at(0).row();		// source row number

	pproxy->setItemNum(&sourceRowToUpdate);		// update proxy
	validateAdd();								// revalidate proxy model

	combo_products->setCurrentIndex(pproxy->mapFromSource(batchl.at(0)).row());
	spin_qty->setValue(qty/100.0);
	this->disttype = disttype;
	edit_reason_a->setRaw(disttype_a);
	edit_reason_b->setRaw(disttype_b);
	
	action_addnext->setText(tr("Update record and exit"));
	validateAdd();								// revalidate proxy model

	action_addexit->setText(tr("Update record"));
	action_addnext->hide();
}

#include "DistributorRecordWidget.moc"