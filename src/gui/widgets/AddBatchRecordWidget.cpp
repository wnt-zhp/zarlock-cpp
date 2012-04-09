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
#include "AddBatchRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

#include <QStringList>
#include <QSqlQuery>

AddBatchRecordWidget::AddBatchRecordWidget(QWidget * parent) : Ui::ABRWidget(),
	completer_spec(NULL), completer_qty(NULL), completer_unit(NULL), completer_price(NULL),
	completer_invoice(NULL), completer_book(NULL), completer_expiry(NULL), pproxy(NULL),
	idToUpdate(-1) {
	CI();
	setupUi(parent);

	action_addexit->setEnabled(false);
	action_addnext->setEnabled(false);

	connect(edit_book, SIGNAL(dataChanged()), edit_expiry, SLOT(doRefresh()));

	connect(action_addnext, SIGNAL(clicked(bool)), this, SLOT(insertRecord()));
	connect(action_addexit, SIGNAL(clicked(bool)), this, SLOT(insertRecordExit()));
	connect(action_cancel, SIGNAL(clicked(bool)), this, SLOT(cancelForm()));

// 	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateCB(int)));

// 	connect(edit_spec, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(spin_qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));
// 	connect(edit_unit, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_price, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(check_uprice, SIGNAL(stateChanged(int)), this, SLOT(validateAdd()));
	connect(edit_invoice, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_book, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));

	connect(check_inf, SIGNAL(toggled(bool)), edit_expiry, SLOT(setDisabled(bool)));
	connect(check_inf, SIGNAL(stateChanged(int)), this, SLOT(validateAdd()));

	connect(Database::Instance(), SIGNAL(batchWordListUpdated()), this, SLOT(update_model()));

	edit_spec->enableEmpty(true);
	edit_expiry->setDateReferenceObj(edit_book);
}

AddBatchRecordWidget::~AddBatchRecordWidget() {
	DI();

	if (completer_spec) delete completer_spec;
	if (completer_qty) delete completer_qty;
	if (completer_unit) delete completer_unit;
	if (completer_price) delete completer_price;
	if (completer_invoice) delete completer_invoice;
	if (completer_book) delete completer_book;
	if (completer_expiry) delete completer_expiry;
	if (pproxy) delete pproxy;
}

void AddBatchRecordWidget::insertRecord() {
	Database * db = Database::Instance();

	int idx = combo_products->currentIndex();
	unsigned int prod_id = pproxy->mapToSource(pproxy->index(idx, 0)).data().toUInt();

	// price
	double netto, vat;
	DataParser::price(edit_price->text(), netto, vat);
	int uprice = int(netto*(100+vat));
	// unit price
	int unitprice = check_uprice->isChecked() ? uprice : (uprice / spin_qty->value());
	QDate regdate, expdate;
	if (!check_inf->isChecked()) {
		expdate = edit_expiry->date();
	}
	regdate = edit_book->date();

	// FIXME: edit_date->book() jest potencjalnie miejscem bledow
	if (idToUpdate >= 0) {
		BatchTableModel * btm = db->CachedBatch();
		QModelIndexList bl = btm->match(btm->index(0, BatchTableModel::HId), Qt::EditRole, idToUpdate, 1, Qt::MatchExactly);
		if (bl.size() != 1)
			return;
		if (db->updateBatchRecord(bl.at(0).row(), prod_id, edit_spec->text(), unitprice, edit_unit->text(),
			spin_qty->value()*100, regdate, expdate, QDate::currentDate(), edit_invoice->text(), ":)")) {
			idToUpdate = -1;
			clearForm();
			combo_products->setFocus();
		}
	} else {
		if (db->addBatchRecord(prod_id, edit_spec->text(), unitprice, edit_unit->text(),
			spin_qty->value()*100, regdate, expdate, QDate::currentDate(), edit_invoice->text(), ":)")) {
			clearForm();
			combo_products->setFocus();
		}
	}
}

void AddBatchRecordWidget::insertRecordExit() {
	insertRecord();
// 	cancelForm();
	this->setVisible(false);
}

void AddBatchRecordWidget::clearForm() {
	edit_spec->clear();
// 	combo_spec->clear();
	edit_expiry->clear();
// 	edit_qty->clear();
	edit_unit->clear();
// 	combo_unit->clear();
	edit_price->clear();
// 	check_uprice->setCheckState(Qt::Checked);
// 	edit_invoice->clear();
// 	edit_book->clear();
}

void AddBatchRecordWidget::cancelForm() {
	prepareInsert(false);
	emit canceled(false);
}

void AddBatchRecordWidget::validateCB(int i) {
	Database * db = Database::Instance();

	ProductsTableModel * ptm = db->CachedProducts();
	QString defexp = ptm->index(i, ProductsTableModel::HExpire).data().toString();
	QString defunit = ptm->index(i, ProductsTableModel::HUnit).data().toString();

	if (edit_expiry->text(true).isEmpty()) {
		edit_expiry->setRaw(defexp);
// 		edit_expiry->doRefresh();
		edit_unit->setRaw(defunit);
// 		edit_unit->doRefresh();
	}

	validateAdd();
}

void AddBatchRecordWidget::validateAdd() {
// 	PR(combo_products->currentIndex());

	if (edit_spec->ok() and edit_unit->ok() and
		edit_book->ok() and edit_price->ok() and
		(spin_qty->value() > 0) and edit_invoice->ok() and
		(edit_expiry->ok() or check_inf->isChecked())) {
		action_addnext->setEnabled(true);
		action_addexit->setEnabled(true);
	} else {
		action_addnext->setEnabled(false);
		action_addexit->setEnabled(false);
	}
}

void AddBatchRecordWidget::update_model() {
	Database * db = Database::Instance();

	if (pproxy) delete pproxy;
	pproxy = new QSortFilterProxyModel();
	pproxy->setSourceModel(db->CachedProducts());
	pproxy->sort(1, Qt::AscendingOrder);
	pproxy->setSortCaseSensitivity(Qt::CaseInsensitive);
	combo_products->setModel(pproxy);
	combo_products->setModelColumn(1);
	
	if (completer_spec) delete completer_spec;
// 	if (completer_qty) delete completer_qty;
	if (completer_unit) delete completer_unit;
	if (completer_price) delete completer_price;
	if (completer_invoice) delete completer_invoice;
	if (completer_book) delete completer_book;
	if (completer_expiry) delete completer_expiry;

	completer_spec = new QCompleter(db->BatchWordList().at(Database::BWspec), edit_spec);
// 	completer_qty = new QCompleter(db->BatchWordList().at(Database::BWqty), edit_qty);
	completer_unit = new QCompleter(db->BatchWordList().at(Database::BWunit), edit_unit);
	completer_price = new QCompleter(db->BatchWordList().at(Database::BWprice), edit_price);
	completer_invoice = new QCompleter(db->BatchWordList().at(Database::BWinvoice), edit_invoice);
	completer_book = new QCompleter(db->BatchWordList().at(Database::BWbooking), edit_book);
	completer_expiry = new QCompleter(db->BatchWordList().at(Database::BWexpire), edit_expiry);

	completer_spec->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
// 	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_unit->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_price->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_invoice->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_book->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_expiry->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	edit_spec->setCompleter(completer_spec);
// 	edit_qty->setCompleter(completer_qty);
	edit_unit->setCompleter(completer_unit);
	edit_price->setCompleter(completer_price);
	edit_invoice->setCompleter(completer_invoice);
	edit_book->setCompleter(completer_book);
	edit_expiry->setCompleter(completer_expiry);

	validateCB(combo_products->currentIndex());
}

void AddBatchRecordWidget::prepareInsert(bool visible) {
	action_addexit->setText(tr("Insert record and exit"));
	action_addnext->setText(tr("Insert record and add next"));
	idToUpdate = -1;
	if (visible) {
		clearForm();
	}
}

void AddBatchRecordWidget::prepareUpdate(const QModelIndex & idx) {
	int pid;
	QString spec, unit, invoice, notes;
	QDate reg, expiry, entry;
	int price, qty, used;

	clearForm();
// 	update_model();

	Database * db = Database::Instance();

	BatchTableModel * btm = db->CachedBatch();
	QModelIndexList bl = btm->match(btm->index(0, BatchTableModel::HId), Qt::EditRole, idx.model()->index(idx.row(), BatchTableModel::HId).data(Qt::EditRole), -1, Qt::MatchExactly);
	if (bl.size() != 1)
		return;
	idToUpdate = bl.at(0).data(Qt::EditRole).toInt();
	db->getBatchRecord(bl.at(0).row(), pid, spec, price, unit, qty, used, reg, expiry, entry, invoice, notes);

	ProductsTableModel * pm = db->CachedProducts();
	QModelIndexList pl = pm->match(pm->index(0, ProductsTableModel::HId), Qt::DisplayRole, idx.model()->index(idx.row(), BatchTableModel::HProdId).data(Qt::EditRole).toUInt(), -1, Qt::MatchExactly);
	if (pl.size() != 1)
		return;

	combo_products->setCurrentIndex(pproxy->mapFromSource(pl.at(0)).row());
	edit_spec->setRaw(spec);
	edit_price->setRaw(QString("%1").arg(0.01*price));
	check_uprice->setChecked(true);
	edit_unit->setRaw(unit);
	spin_qty->setMinimum(0.01*used);
	spin_qty->setValue(0.01*qty);
	edit_book->setRaw(reg.toString("dd/MM/yyyy"));
	edit_expiry->setRaw(expiry.toString("dd/MM/yyyy"));
	check_inf->setChecked(!expiry.isValid());
	edit_invoice->setRaw(invoice);

	action_addexit->setText(tr("Update record and exit"));
	action_addnext->setText(tr("Update record and add next"));
}

#include "AddBatchRecordWidget.moc"