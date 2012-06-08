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
#include "ProductsRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

ProductsRecordWidget::ProductsRecordWidget(QWidget * parent) : AbstractRecordWidget(), Ui::PRWidget(),
	completer_name(NULL), completer_unit(NULL), completer_expiry(NULL) {
	setupUi(parent);

	button_label_insert_and_next = action_addnext->text();
	button_label_insert_and_exit = action_addexit->text();
	button_label_close = action_cancel->text();
	
	action_addnext->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_addexit->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) );
	action_clear->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogDiscardButton) );
	action_cancel->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton) );

	action_addexit->setEnabled(false);
	action_addnext->setEnabled(false);

	connect(action_addnext, SIGNAL(clicked(bool)), this, SLOT(insertRecord()));
	connect(action_addexit, SIGNAL(clicked(bool)), this, SLOT(insertRecordAndExit()));
	connect(action_cancel, SIGNAL(clicked(bool)), this,  SLOT(closeForm()));
	connect(action_clear, SIGNAL(clicked(bool)), this,  SLOT(clearForm()));

	connect(edit_name, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_unit, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));
	connect(edit_expiry, SIGNAL(textChanged(QString)), this,  SLOT(validateAdd()));

	connect(Database::Instance(), SIGNAL(productsWordListUpdated()), this, SLOT(update_model()));

	button_label_insert_and_next = action_addnext->text();
	button_label_insert_and_exit = action_addexit->text();
	button_label_close = action_addexit->text();
}

ProductsRecordWidget::~ProductsRecordWidget() {
	FI();
	if (completer_name) delete completer_name;
	if (completer_unit) delete completer_unit;
	if (completer_expiry) delete completer_expiry;
}

void ProductsRecordWidget::setVisible(bool visible) {
	edit_name->setFocus();
    QWidget::setVisible(visible);
}

void ProductsRecordWidget::insertRecord() {
	Database * db = Database::Instance();

	if (idToUpdate >= 0) {
		ProductsTableModel * ptm = db->CachedProducts();
		QModelIndexList pl = ptm->match(ptm->index(0, ProductsTableModel::HId), Qt::EditRole, idToUpdate, 1, Qt::MatchExactly);
		if (pl.size() != 1)
			return;

		if (db->updateProductRecord(pl.at(0).row(), edit_name->text(), edit_unit->text(), edit_expiry->text(), ":)")) {
			idToUpdate = 0;
			clearForm();
		}
	} else {
		if (db->addProductRecord(edit_name->text(), edit_unit->text(), edit_expiry->text(), ";)")) {
			clearForm();
		}
	}
}

void ProductsRecordWidget::prepareUpdate(const QModelIndex& idx) {
	int pid;
	QString name, unit, expiry, notes;

	clearForm();

	Database * db = Database::Instance();

	ProductsTableModel * ptm = db->CachedProducts();
	QModelIndexList pl = ptm->match(ptm->index(0, BatchTableModel::HId), Qt::EditRole, idx.model()->index(idx.row(), ProductsTableModel::HId).data(Qt::EditRole), -1, Qt::MatchExactly);
	if (pl.size() != 1)
		return;
	idToUpdate = pl.at(0).data(Qt::EditRole).toInt();
	db->getProductRecord(pl.at(0).row(), pid, name, unit, expiry, notes);

	edit_name->setRaw(name);
	edit_unit->setRaw(unit);
	edit_expiry->setRaw(expiry);
// 	edit_notes->setRaw(notes);

	action_addexit->setText(tr("Update record"));
	action_addnext->hide();
}


void ProductsRecordWidget::clearForm() {
	edit_name->clear();
	edit_unit->clear();
	edit_expiry->clear();
}

void ProductsRecordWidget::validateAdd() {
	if (edit_name->ok() and edit_unit->ok() and edit_expiry->ok()) {
		action_addnext->setEnabled(true);
		action_addexit->setEnabled(true);
	} else {
		action_addnext->setEnabled(false);
		action_addexit->setEnabled(false);
	}
}

void ProductsRecordWidget::update_model() {
	if (completer_name) delete completer_name;
	if (completer_unit) delete completer_unit;
	if (completer_expiry) delete completer_expiry;

	completer_name = new QCompleter(Database::Instance()->ProductsWordList().at(Database::PWname), edit_name);
	completer_unit = new QCompleter(Database::Instance()->ProductsWordList().at(Database::PWunit), edit_unit);
	completer_expiry = new QCompleter(Database::Instance()->ProductsWordList().at(Database::PWexpire), edit_expiry);

	completer_name->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_unit->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	completer_expiry->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

	edit_name->setCompleter(completer_name);
	edit_unit->setCompleter(completer_unit);
	edit_expiry->setCompleter(completer_expiry);
}

#include "ProductsRecordWidget.moc"