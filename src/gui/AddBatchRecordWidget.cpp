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

#include "AddBatchRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

#include <QDate>
#include <QTextStream>

#include <iostream>
#define PR(x) cout << "++DEBUG: " << #x << " = |" << x << "|\n";

using namespace std;

AddBatchRecordWidget::AddBatchRecordWidget(QWidget * parent) : Ui::ABRWidget() {
	setupUi(parent);
	cout << "++ AddBatchRecordWidget::AddBatchRecordWidget\n";

	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clear_form()));
	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(cancel_form()));

	connect(combo_products, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(edit_spec, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(edit_qty, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_unit, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_price, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(check_uprice, SIGNAL(stateChanged(int)), this, SLOT(validateAdd()));
	connect(edit_invoice, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_book, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));
}

AddBatchRecordWidget::~AddBatchRecordWidget() {
	if (completer) delete completer;
}

bool AddBatchRecordWidget::insert_record() {
	cout << "++ AddBatchRecordWidget::insert_record()\n";
	Database & db = Database::Instance();
	ProductsTableModel * ptm = db.CachedProducts();
	BatchTableModel * btm = db.CachedBatch();

	int idx = combo_products->currentIndex();
	int prod_id = ptm->data(ptm->index(idx, 0)).toInt();
	double price, tax;
	DataParser::price(edit_price->text(), price, tax);
	QTextStream uprice;
	uprice << price/edit_qty->text().toDouble() << "+" << tax;
	QString unitprice = check_uprice->isChecked() ? edit_price->text() : uprice.readAll();
// 	edit_price->text().toDouble()/edit_qty->text().toDouble();

	int row = btm->rowCount();
	btm->insertRows(row, 1);
// 	btm->setData(btm->index(row, BatchTableModel::HId), row);
	btm->setData(btm->index(row, BatchTableModel::HProdId), prod_id);
	btm->setData(btm->index(row, BatchTableModel::HSpec), edit_spec->text());
	btm->setData(btm->index(row, BatchTableModel::HExpire), edit_expiry->text());
	btm->setData(btm->index(row, BatchTableModel::HStaQty), edit_qty->text());
	btm->setData(btm->index(row, BatchTableModel::HUnit), edit_unit->text());
	btm->setData(btm->index(row, BatchTableModel::HPrice), unitprice);
	btm->setData(btm->index(row, BatchTableModel::HCurQty), edit_qty->text());
	btm->setData(btm->index(row, BatchTableModel::HInvoice), edit_invoice->text());
	btm->setData(btm->index(row, BatchTableModel::HBook), edit_book->text());
	btm->setData(btm->index(row, BatchTableModel::HRegDate), QDate::currentDate().toString("dd/MM/yyyy"));
	btm->setData(btm->index(row, BatchTableModel::HDesc), QString(":)"));
	bool status = btm->submitAll();

// 	clear_form();
	edit_spec->setFocus();
	return status;
}

void AddBatchRecordWidget::clear_form() {
	cout << "++ AddBatchRecordWidget::clear_form()\n";
	edit_spec->clear();
	edit_unit->clear();
	edit_expiry->clear();
	edit_spec->clear();
	edit_expiry->clear();
	edit_qty->clear();
	edit_unit->clear();
	edit_price->clear();
	check_uprice->setCheckState(Qt::Checked);
	edit_invoice->clear();
	edit_book->clear();
	edit_expiry->clear();
}

void AddBatchRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddBatchRecordWidget::validateAdd() {
	if (!edit_spec->text().isEmpty() &&
		!edit_unit->text().isEmpty() &&
		!edit_expiry->text().isEmpty() &&
		!edit_spec->text().isEmpty() &&
		!edit_expiry->text().isEmpty() &&
		!edit_qty->text().isEmpty() &&
		!edit_unit->text().isEmpty() &&
		!edit_price->text().isEmpty() &&
		!edit_invoice->text().isEmpty() &&
		!edit_book->text().isEmpty() &&
		!edit_expiry->text().isEmpty()) {
		action_add->setEnabled(true);
	} else {
		action_add->setEnabled(false);
	}
}

void AddBatchRecordWidget::update_model() {
	combo_products->setModel(Database::Instance().CachedProducts());
	combo_products->setModelColumn(1);

// 	completer = new QCompleter(combo_products->model(), this);
// 	completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
// 	completer->setCompletionColumn(ProductsTableModel::HName);
// 	combo_products->setCompleter(completer);
}


#include "AddBatchRecordWidget.moc"