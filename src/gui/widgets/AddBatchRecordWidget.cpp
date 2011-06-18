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

AddBatchRecordWidget::AddBatchRecordWidget(QWidget * parent) : Ui::ABRWidget(),
	completer_spec(NULL), completer_qty(NULL), completer_unit(NULL), completer_price(NULL),
	completer_invoice(NULL), completer_book(NULL), completer_expiry(NULL) {
	setupUi(parent);

	action_addexit->setEnabled(false);
	action_addnext->setEnabled(false);

	connect(edit_book, SIGNAL(dateChanged()), edit_expiry, SLOT(doRefresh()));

	connect(action_addnext, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_addexit, SIGNAL(clicked(bool)), this, SLOT(insert_record_and_exit()));
	connect(action_cancel, SIGNAL(clicked(bool)), this, SLOT(cancel_form()));

// 	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateCB(int)));
	connect(edit_spec, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_qty, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_unit, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_price, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(check_uprice, SIGNAL(stateChanged(int)), this, SLOT(validateAdd()));
	connect(edit_invoice, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_book, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));

	edit_expiry->setDateReferenceObj(edit_book);
}

AddBatchRecordWidget::~AddBatchRecordWidget() {
	FPR(__func__);
	if (completer_spec) delete completer_spec;
	if (completer_qty) delete completer_qty;
	if (completer_unit) delete completer_unit;
	if (completer_price) delete completer_price;
	if (completer_invoice) delete completer_invoice;
	if (completer_book) delete completer_book;
	if (completer_expiry) delete completer_expiry;
}

void AddBatchRecordWidget::insert_record() {
	Database & db = Database::Instance();
	ProductsTableModel * ptm = db.CachedProducts();
	BatchTableModel * btm = db.CachedBatch();

	int idx = combo_products->currentIndex();
	int prod_id = ptm->data(ptm->index(idx, 0)).toInt();

	// proce
	double price, tax;
	DataParser::price(edit_price->text(), price, tax);
	QString uprice;
	uprice.sprintf("%.2f+%d", price/edit_qty->text().toDouble(), int(tax));
	// unit price
	QString unitprice = check_uprice->isChecked() ? edit_price->text() : uprice;

// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< prod_id << ","
// 		<< edit_spec->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << ","
// 		<< edit_qty->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< unitprice.toStdString() << ","
// 		<< edit_qty->text().toStdString() << ","
// 		<< edit_invoice->text().toStdString() << ","
// 		<< edit_book->text(true).toStdString() << ","
// 		<< QDate::currentDate().toString(Qt::ISODate).toStdString() << ","
// 		<< QString(":)").toStdString() << " );" << std::endl;

	db.addBatchRecord(prod_id, edit_spec->text(), edit_book->text(true), QDate::currentDate().toString(Qt::ISODate),
		edit_expiry->text(), edit_qty->text().toFloat(), 0.0, edit_unit->text(), unitprice, edit_invoice->text(), ":)");

	clear_form();
	edit_spec->setFocus();
}

void AddBatchRecordWidget::insert_record_and_exit() {
	insert_record();
	cancel_form();
}

void AddBatchRecordWidget::clear_form() {
	edit_spec->clear();
	edit_unit->clear();
	edit_expiry->clear();
	edit_spec->clear();
// 	edit_expiry->clear();
	edit_qty->clear();
	edit_unit->clear();
	edit_price->clear();
// 	check_uprice->setCheckState(Qt::Checked);
// 	edit_invoice->clear();
// 	edit_book->clear();
}

void AddBatchRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddBatchRecordWidget::validateCB(int i) {
	ProductsTableModel * ptm = Database::Instance().CachedProducts();
	BatchTableModel * btm = Database::Instance().CachedBatch();
	QString defexp = ptm->index(i, ProductsTableModel::HExpire).data().toString();

	if (edit_expiry->text().isEmpty()) {
		edit_expiry->setText(defexp);
		edit_expiry->doRefresh();
	}

	validateAdd();
}


void AddBatchRecordWidget::validateAdd() {
// 	PR(combo_products->currentIndex());

	if (edit_spec->ok() and edit_unit->ok() and
		edit_book->ok() and edit_price->ok() and
		edit_qty->ok() and edit_expiry->ok() and
		edit_invoice->ok()) {
		action_addnext->setEnabled(true);
		action_addexit->setEnabled(true);
	} else {
		action_addnext->setEnabled(false);
		action_addexit->setEnabled(false);
	}
}

void AddBatchRecordWidget::update_model() {
	combo_products->setModel(Database::Instance().CachedProducts());
	combo_products->setModelColumn(1);
// 	combo_products->setEditable(true);

// 	completer = new QCompleter(combo_products->model(), this);
// 	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
// 	completer->setCompletionColumn(ProductsTableModel::HName);
// 	combo_products->setCompleter(completer);

	if (completer_spec) delete completer_spec;
	if (completer_qty) delete completer_qty;
	if (completer_unit) delete completer_unit;
	if (completer_price) delete completer_price;
	if (completer_invoice) delete completer_invoice;
	if (completer_book) delete completer_book;
	if (completer_expiry) delete completer_expiry;

	completer_spec = new QCompleter(edit_spec);
	completer_qty = new QCompleter(edit_qty);
	completer_unit = new QCompleter(edit_unit);
	completer_price = new QCompleter(edit_price);
	completer_invoice = new QCompleter(edit_invoice);
	completer_book = new QCompleter(edit_book);
	completer_expiry = new QCompleter(edit_expiry);

	Database & db = Database::Instance();
	completer_spec->setModel(db.CachedBatch());
	completer_qty->setModel(db.CachedBatch());
	completer_unit->setModel(db.CachedBatch());
	completer_price->setModel(db.CachedBatch());
	completer_invoice->setModel(db.CachedBatch());
	completer_book->setModel(db.CachedBatch());
	completer_expiry->setModel(db.CachedBatch());

	completer_spec->setCompletionColumn(BatchTableModel::HSpec);
	completer_qty->setCompletionColumn(BatchTableModel::HUsedQty);
	completer_unit->setCompletionColumn(BatchTableModel::HUnit);
	completer_price->setCompletionColumn(BatchTableModel::HPrice);
	completer_invoice->setCompletionColumn(BatchTableModel::HInvoice);
	completer_book->setCompletionColumn(BatchTableModel::HBook);
	completer_expiry->setCompletionColumn(BatchTableModel::HExpire);

	completer_spec->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_qty->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_unit->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_price->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_invoice->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_book->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_expiry->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	edit_spec->setCompleter(completer_spec);
	edit_qty->setCompleter(completer_qty);
	edit_unit->setCompleter(completer_unit);
	edit_price->setCompleter(completer_price);
	edit_invoice->setCompleter(completer_invoice);
	edit_book->setCompleter(completer_book);
	edit_expiry->setCompleter(completer_expiry);

	validateCB(combo_products->currentIndex());
}


#include "AddBatchRecordWidget.moc"