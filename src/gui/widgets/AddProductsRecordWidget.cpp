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
#include "AddProductsRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

AddProductsRecordWidget::AddProductsRecordWidget(QWidget * parent) : Ui::APRWidget(),
	completer_name(NULL), completer_unit(NULL), completer_expiry(NULL) {
	setupUi(parent);
	std::cout << "++ AddProductsRecordWidget::AddProductsRecordWidget\n";

	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clear_form()));
	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(cancel_form()));

	connect(edit_name, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_unit, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

// 	vvv = new Validat01;
// 	edit_expiry->setValidator(vvv);
}

AddProductsRecordWidget::~AddProductsRecordWidget() {
	if (completer_name) delete completer_name;
	if (completer_unit) delete completer_unit;
	if (completer_expiry) delete completer_expiry;
}

void AddProductsRecordWidget::setVisible(bool visible) {
	edit_name->setFocus();
    QWidget::setVisible(visible);
}

bool AddProductsRecordWidget::insert_record() {
	std::cout << "++ AddProductsRecordWidget::insert_record()\n";
	Database & db = Database::Instance();
	QSqlTableModel * tm = db.CachedProducts();
// 	PR(&db);
// 	PR(tm);

	PR(edit_name->text().toStdString());
	PR(edit_unit->text().toStdString());
	PR(edit_expiry->text().toStdString());

	int row = tm->rowCount();
	tm->insertRows(row, 1);
	tm->setData(tm->index(row, 1), edit_name->text());
	tm->setData(tm->index(row, 2), edit_unit->text());
	tm->setData(tm->index(row, 3), edit_expiry->text());
	tm->submitAll();

	clear_form();
	edit_name->setFocus();

	return true;
}

void AddProductsRecordWidget::clear_form() {
	std::cout << "++ AddProductsRecordWidget::clear_form()\n";
	edit_name->clear();
	edit_unit->clear();
	edit_expiry->clear();
}

void AddProductsRecordWidget::cancel_form() {
	emit canceled(false);
}

void AddProductsRecordWidget::validateAdd() {
	if (edit_name->ok() and edit_unit->ok() and edit_expiry->ok()) {
		action_add->setEnabled(true);
	} else {
		action_add->setEnabled(false);
	}
}

void AddProductsRecordWidget::update_model() {
	if (completer_name) delete completer_name;
	if (completer_unit) delete completer_unit;
	if (completer_expiry) delete completer_expiry;

	completer_name = new QCompleter(edit_name);
	completer_unit = new QCompleter(edit_unit);
	completer_expiry = new QCompleter(edit_expiry);

	Database & db = Database::Instance();
	completer_name->setModel(db.CachedProducts());
	completer_unit->setModel(db.CachedProducts());
	completer_expiry->setModel(db.CachedProducts());

	completer_name->setCompletionColumn(ProductsTableModel::HName);
	completer_unit->setCompletionColumn(ProductsTableModel::HUnit);
	completer_expiry->setCompletionColumn(ProductsTableModel::HExpire);

	edit_name->setCompleter(completer_name);
	edit_unit->setCompleter(completer_unit);
	edit_expiry->setCompleter(completer_expiry);
}

#include "AddProductsRecordWidget.moc"