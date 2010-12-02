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


#include "AddProductsRecordWidget.h"
#include "Database.h"

#include <iostream>
#define PR(x) cout << "++DEBUG: " << #x << " = |" << x << "|\n";

using namespace std;

AddProductsRecordWidget::AddProductsRecordWidget(QWidget * parent) : Ui::APRWidget() {
	setupUi(parent);
	cout << "++ AddProductsRecordWidget::AddProductsRecordWidget\n";
	connect(action_add, SIGNAL(clicked(bool)), this, SLOT(insert_record()));
	connect(action_clear, SIGNAL(clicked(bool)), this, SLOT(clear_form()));
	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(cancel_form()));
	connect(edit_name, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_unit, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

	edit_name->setText("cukier");
	edit_unit->setText("1kg");
	edit_expiry->setText("100");
}

bool AddProductsRecordWidget::insert_record() {
	cout << "++ AddProductsRecordWidget::insert_record()\n";
	Database & db = Database::Instance();
	QSqlTableModel * tm = db.CachedProducts();
	PR(&db);
	PR(tm);

	PR(edit_name->text().toStdString());
	PR(edit_unit->text().toStdString());
	PR(edit_expiry->text().toStdString());

	int row = tm->rowCount();
	tm->insertRows(row, 1);
	tm->setData(tm->index(row, 1), edit_name->text());
	tm->setData(tm->index(row, 2), edit_unit->text());
	tm->setData(tm->index(row, 3), edit_expiry->text().toInt());
	tm->submitAll();

	clear_form();
	edit_name->setFocus();
}

void AddProductsRecordWidget::clear_form() {
	cout << "++ AddProductsRecordWidget::clear_form()\n";
	edit_name->clear();
	edit_unit->clear();
	edit_expiry->clear();
}

void AddProductsRecordWidget::cancel_form() {
	emit canceled(false);

}

void AddProductsRecordWidget::validateAdd() {
	if (!edit_name->text().isEmpty() &&
		!edit_unit->text().isEmpty() &&
		!edit_unit->text().isEmpty()) {
		action_add->setEnabled(true);
	} else {
		action_add->setEnabled(false);
	}
}

#include "AddProductsRecordWidget.moc"