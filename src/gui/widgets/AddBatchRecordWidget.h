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


#ifndef ADDBATCHRECORDWIDGET_H
#define ADDBATCHRECORDWIDGET_H

#include <QCompleter>
#include <QProxyModel>

#include "ui_AddBatchRecordWidget.h"

class AddBatchRecordWidget : public QWidget, public Ui::ABRWidget {
Q_OBJECT
public:
	AddBatchRecordWidget(QWidget * parent = NULL);
	virtual ~AddBatchRecordWidget();

signals:
	void canceled(bool);

public slots:
	void update_model();

private slots:
	void insert_record();
	void insert_record_and_exit();
	void clear_form();
	void cancel_form();
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

	QProxyModel * pproxy;
};

#endif // ADDBATCHRECORDWIDGET_H
