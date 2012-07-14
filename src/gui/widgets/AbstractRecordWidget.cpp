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
#include "AbstractRecordWidget.h"
#include "Database.h"
#include "DataParser.h"

#include <QStringList>
#include <QSqlQuery>

AbstractRecordWidget::AbstractRecordWidget(QWidget * parent) : idToUpdate(0), widget_mode(INSERT_MODE) {
	CI();
}

AbstractRecordWidget::~AbstractRecordWidget() {
	DI();
}

void AbstractRecordWidget::insertRecordAndExit() {
	insertRecord();
	setVisible(false);
	closeForm();
}

void AbstractRecordWidget::clearForm() {
}

void AbstractRecordWidget::closeForm() {
	emit closed(false);
}

void AbstractRecordWidget::prepareWidget() {
}

void AbstractRecordWidget::setInsertMode() {
	widget_mode = INSERT_MODE;
	idToUpdate = -1;
	prepareWidget();
	clearForm();
	prepareInsert();
}

void AbstractRecordWidget::setUpdateMode(const QModelIndex & idx) {
	widget_mode = UPDATE_MODE;
	idxToUpdate = idx;
	idToUpdate = idx.model()->index(idx.row(), 0).data(Qt::EditRole).toUInt();
	prepareWidget();
	clearForm();
	prepareUpdate();
}

#include "AbstractRecordWidget.moc"