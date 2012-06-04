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


#ifndef BATCHRECORDWIDGET_H
#define BATCHRECORDWIDGET_H

#include <QCompleter>
#include <QSortFilterProxyModel>

#include "ui_BatchRecordWidget.h"
#include "AbstractRecordWidget.h"

class BatchRecordWidget : public AbstractRecordWidget, public Ui::BRWidget {
Q_OBJECT
public:
	BatchRecordWidget(QWidget * parent = NULL);
	virtual ~BatchRecordWidget();

public slots:
	void update_model();
	void prepareUpdate(const QModelIndex & idx);

private slots:
	void insertRecord();
	void clearForm();
	void validateAdd();
	void validateCB(int i);

private:
	QCompleter * completer_spec;
	QCompleter * completer_qty;
	QCompleter * completer_unit;
	QCompleter * completer_price;
	QCompleter * completer_invoice;
	QCompleter * completer_book;
	QCompleter * completer_expiry;

	QSortFilterProxyModel * pproxy;
};

#endif // BATCHRECORDWIDGET_H
