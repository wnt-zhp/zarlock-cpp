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
#include "AbstractRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

#include <QStringList>
#include <QSqlQuery>

AbstractRecordWidget::AbstractRecordWidget(QWidget * parent) : idToUpdate(0) {
	CI();
	button_label_update = tr("Update record");
}

AbstractRecordWidget::~AbstractRecordWidget() {
	DI();
}

void AbstractRecordWidget::insertRecordAndExit() {
	insertRecord();
	this->setVisible(false);
	closeForm();
}

void AbstractRecordWidget::clearForm() {
}

void AbstractRecordWidget::closeForm() {
	emit closed(false);
}

void AbstractRecordWidget::update_model() {
}

void AbstractRecordWidget::prepareInsert(bool visible) {
// 	action_addexit->setText(button_label_insert_and_exit);	//!
// 	action_addnext->show();									//!
	idToUpdate = -1;
	if (visible) {
		clearForm();
	}
}

void AbstractRecordWidget::prepareUpdate(const QModelIndex & idx) {
	idToUpdate = -1;

// 	action_addexit->setText(button_label_update);		//!
// 	action_addnext->hide();									//!
}

#include "AbstractRecordWidget.moc"