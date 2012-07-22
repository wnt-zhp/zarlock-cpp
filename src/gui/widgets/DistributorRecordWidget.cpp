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

#include <QCompleter>
#include <QProxyModel>
#include <QDesktopWidget>

#include "DistributorRecordWidget.h"
#include "globals.h"

#include "Database.h"
#include "DataParser.h"
#include "EventFilter.h"
#include "BatchTableView.h"
#include "TextInput.h"

#include "BatchTableModel.h"
#include "DistributorTableModel.h"

const int new_width = 1000;

DistributorRecordWidget::DistributorRecordWidget(QWidget * parent) : AbstractRecordWidget(), Ui::DRWidget(),
	completer_qty(NULL), completer_date(NULL), completer_reason(NULL), completer_reason2(NULL),
	proxy(NULL), hideempty(NULL), disttype(0), tv(NULL) {
	setupUi(parent);

	combo_batch->setPopupExpandable(true);

	action_addnext->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_addexit->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_update->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_clear->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton) );
	action_cancel->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton) );

	action_cancel->setShortcut(Qt::Key_Escape);

	connect(action_addnext, SIGNAL(clicked(bool)), this, SLOT(insertRecord()));
	connect(action_addexit, SIGNAL(clicked(bool)), this, SLOT(insertRecordAndExit()));
	connect(action_update, SIGNAL(clicked(bool)), this, SLOT(insertRecordAndExit()));
	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(closeForm()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clearForm()));

	connect(edit_date, SIGNAL(textChanged(QString)), this,  SLOT(validateDistDate()));
	connect(combo_batch, SIGNAL(currentIndexChanged(int)), this, SLOT(validateBatchData()));
	connect(spin_qty, SIGNAL(valueChanged(int)), this, SLOT(validateData()));
	connect(edit_reason_a, SIGNAL(textChanged(QString)), this, SLOT(validateData()));
// 	connect(edit_reason_b, SIGNAL(textChanged(QString)), this,  SLOT(validateData()));

	connect(Database::Instance(), SIGNAL(distributorWordListUpdated()), this, SLOT(prepareWidget()));

// 	connect(db->CachedDistributor(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update_model()));
// 	edit_date->setDateReferenceObj(edit_book);
	edit_reason_b->setEmptyAllowed(true);
	// TODO: do it better
	combo_batch->setStyleSheet("color: black;");

	hideempty = new QAction(this);
	hideempty->setCheckable(true);
	hideempty->setChecked(true);

	proxy = new BatchTableModelProxy;
	proxy->setHideEmpty(true);

	combo_batch->setModel(proxy);
	combo_batch->setModelColumn(BatchTableModel::HSpec);

	combo_batch->setAutoCompletion(true);
	combo_batch->setAutoCompletionCaseSensitivity(Qt::CaseInsensitive);

	prepareWidget();
	prepareView();
}

DistributorRecordWidget::~DistributorRecordWidget() {
	DII();
	if (completer_qty) delete completer_qty;
	if (completer_date) delete completer_date;
	if (completer_reason) delete completer_reason;
	if (completer_reason2) delete completer_reason2;
	if (proxy) delete proxy;
	delete hideempty;

	delete tv;
}

void DistributorRecordWidget::insertRecord() {
	Database * db = Database::Instance();

	int idx = combo_batch->currentIndex();
	int batch_id = proxy->mapToSource(proxy->index(idx, 0)).data(BatchTableModel::RRaw).toInt();	//HBatchId

	QDate df;
	if (!DataParser::date(edit_date->text(), df))
		return;

	if (widget_mode == UPDATE_MODE) {
		DistributorTableModel * dtm = db->CachedDistributor();
		QModelIndexList dl = dtm->match(dtm->index(0, DistributorTableModel::HId), Qt::EditRole, idToUpdate, 1, Qt::MatchExactly);
		if (dl.size() != 1)
			return;
		if (db->updateDistributorRecord(dl.at(0).row(), batch_id, spin_qty->value(), df, QDate::currentDate(),
			disttype, edit_reason_a->text(), edit_reason_b->text()))
		{
			clearForm();
			edit_date->setFocus();
		}
	} else if (widget_mode == INSERT_MODE) {
		if (db->addDistributorRecord(batch_id, spin_qty->value(), df, QDate::currentDate(),
			disttype, edit_reason_a->text(), edit_reason_b->text()))
		{
			clearForm();
			edit_date->setFocus();
			proxy->invalidate();
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

void DistributorRecordWidget::validateDistDate() {
	bool is_date_ok = edit_date->ok();
	if (is_date_ok) {
		proxy->setDateKey(edit_date->date());
		proxy->invalidate();
	}

	combo_batch->setEnabled(is_date_ok);
	spin_qty->setEnabled(is_date_ok);
	edit_reason_a->setEnabled(is_date_ok);
	edit_reason_b->setEnabled(is_date_ok);

	validateBatchData();
}

void DistributorRecordWidget::validateBatchData() {
	QModelIndex idx = proxy->mapToSource(proxy->index(combo_batch->currentIndex(), 0));

	Database * db = Database::Instance();

	unsigned int fake = 0;
	unsigned int qtyused = db->CachedBatch()->index(idx.row(), BatchTableModel::HUsedQty).data(Qt::EditRole).toUInt();
	unsigned int qtytotal = db->CachedBatch()->index(idx.row(), BatchTableModel::HStaQty).data(Qt::EditRole).toUInt();
	QString qunit = db->CachedBatch()->index(idx.row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();

	if ((widget_mode == UPDATE_MODE) and (idx.row() == sourceRowToUpdate)) {
		fake = db->CachedDistributor()->getIndexById(idToUpdate, DistributorTableModel::HQty).data(DistributorTableModel::RRaw).toUInt();
	}

	unsigned int totalmax = qtytotal - qtyused + fake;

	label_unit->setText(tr("x %1").arg(qunit));
	spin_qty->setSuffix(tr(" of %1").arg(totalmax/100.0, 0, 'f', 2));
	spin_qty->setMaximum(totalmax);

	validateData();
}


void DistributorRecordWidget::validateData() {
	if ((spin_qty->value() > 0.0) and edit_date->ok() and edit_reason_a->ok()) {
		action_addnext->setEnabled(true);
	} else {
		action_addnext->setEnabled(false);
	}
}

void DistributorRecordWidget::prepareWidget() {
	Database * db = Database::Instance();

	proxy->setSourceModel(db->CachedBatch());
	proxy->setDynamicSortFilter(true);
	proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	proxy->sort(2, Qt::AscendingOrder);

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

void DistributorRecordWidget::prepareUpdate() {
	int bid;
	int qty;

	QString spec, price, disttype_a, disttype_b;
	QDate reg, entry;
	int disttype;

	clearForm();
	prepareWidget();

	Database * db = Database::Instance();

	BatchTableModel * bm = db->CachedBatch();

	const QModelIndex & idx = idxToUpdate;
	QModelIndex sidx = idx.model()->index(idx.row(), DistributorTableModel::HBatchId);

	idToUpdate = sidx.model()->index(sidx.row(), DistributorTableModel::HId).data(Qt::EditRole).toUInt();

	// Find batch record index for selected distributor index
	QModelIndexList batchl = bm->match(bm->index(0, BatchTableModel::HId), Qt::DisplayRole, sidx.data(DistributorTableModel::RRaw).toUInt(), -1, Qt::MatchExactly);

	if (batchl.size() != 1)
		return;

	db->getDistributorRecord(sidx.row(), bid, qty, reg, entry, disttype, disttype_a, disttype_b);	// get record

	edit_date->setRaw(reg.toString("dd/MM/yyyy"));

	sourceRowToUpdate = batchl.at(0).row();		// source row number

	proxy->setItemNum(&sourceRowToUpdate);		// update proxy
	proxy->invalidate();

	combo_batch->setCurrentIndex(proxy->mapFromSource(batchl.at(0)).row());
	spin_qty->setValue(qty);
	this->disttype = disttype;
	edit_reason_a->setRaw(disttype_a);
	edit_reason_b->setRaw(disttype_b);

	validateDistDate();								// revalidate proxy model

	action_update->show();
	action_addexit->hide();
	action_addnext->hide();
}

void DistributorRecordWidget::prepareInsert() {
	action_update->hide();
	action_addexit->show();
	action_addnext->show();
}

void DistributorRecordWidget::prepareView() {
	FII();

	delete tv;
	tv = new BatchTableView;

	tv->verticalHeader()->setDefaultSectionSize(20);
	tv->horizontalHeader()->setVisible(true);
	tv->verticalHeader()->setVisible(false);

	// 	tv->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	// 	tv->resizeColumnsToContents();
	// 	tv->resizeColumnToContents(BatchTableModel::HSpec);
	tv->setSelectionBehavior(QAbstractItemView::SelectRows);

	combo_batch->setView(tv);

	tv->selectRow(proxy->mapFromSource(proxy->sourceModel()->index(sourceRowToUpdate, BatchTableModel::HSpec)).row());
	tv->sortByColumn(BatchTableModel::HSpec, Qt::AscendingOrder);
}

#include "DistributorRecordWidget.moc"