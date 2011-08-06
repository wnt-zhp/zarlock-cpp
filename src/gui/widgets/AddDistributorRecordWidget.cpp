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
	completer_qty(NULL), completer_date(NULL), completer_reason(NULL), completer_reason2(NULL), pproxy(NULL), hideempty(NULL) {
	setupUi(parent);

	hideempty = new QCheckBox;
	hideempty->setChecked(true);

	action_add->setIcon( action_add->style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_clear->setIcon( action_add->style()->standardIcon(QStyle::SP_DialogDiscardButton) );

	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clear_form()));
// 	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(cancel_form()));

	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_products, SIGNAL(editTextChanged(QString)), this, SLOT(validateAdd()));
	connect(spin_qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));
	connect(edit_date, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(edit_reason, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_reason2, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

	connect(&Database::Instance(), SIGNAL(distributorWordListUpdated()), this, SLOT(update_model()));

// 	connect(Database::Instance().CachedDistributor(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(update_model()));
// 	edit_date->setDateReferenceObj(edit_book);
	edit_reason2->enableEmpty();
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

bool AddDistributorRecordWidget::insert_record() {
	Database & db = Database::Instance();

	int idx = combo_products->currentIndex();
	int batch_id = pproxy->mapToSource(pproxy->index(idx, 0)).data().toInt();

	db.addDistributorRecord(batch_id, spin_qty->value(), edit_date->text(true),
							QDate::currentDate().toString(Qt::DefaultLocaleShortDate), edit_reason->text(),
							edit_reason2->text(), DistributorTableModel::RGeneral);


	combo_products->setFocus();
	return /*status*/ true;
}

void AddDistributorRecordWidget::clear_form() {
	spin_qty->setValue(0.0);;
	spin_qty->setSuffix("");
	edit_date->clear();
	edit_reason->clear();
	edit_reason2->clear();
}

void AddDistributorRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddDistributorRecordWidget::validateAdd() {
	QModelIndex idx = pproxy->mapToSource(pproxy->index(combo_products->currentIndex(), 0));

	double qtyused = Database::Instance().CachedBatch()->index(idx.row(), BatchTableModel::HUsedQty).data().toDouble();
	double qtytotal = Database::Instance().CachedBatch()->index(idx.row(), BatchTableModel::HStaQty).data(Qt::EditRole).toDouble();
	QString qunit = Database::Instance().CachedBatch()->index(idx.row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();

	label_unit->setText(tr("x %1").arg(qunit));

	QString max;
	max.sprintf("%.2f", qtytotal-qtyused);
	spin_qty->setSuffix(tr(" of %1").arg(max));
	spin_qty->setMaximum(qtytotal-qtyused);

// 	bool qtyvalid = spin_qty->value() > (qtytotal-qtyused) ? false : true;

	if ((spin_qty->value() > 0.0) and edit_date->ok() and
		edit_reason->ok() /*and edit_reason2->ok()*/) {
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
	completer_reason = new QCompleter(Database::Instance().DistributorWordList().at(Database::DWreason), edit_reason);
	completer_reason2 = new QCompleter(Database::Instance().DistributorWordList().at(Database::DWoptional), edit_reason2);

// 	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_date->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_reason2->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

// 	edit_qty->setCompleter(completer_qty);
	edit_date->setCompleter(completer_date);
	edit_reason->setCompleter(completer_reason);
	edit_reason2->setCompleter(completer_reason2);
}

#include "AddDistributorRecordWidget.moc"