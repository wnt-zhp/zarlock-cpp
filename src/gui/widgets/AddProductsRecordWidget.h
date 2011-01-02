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


#ifndef ADDPRODUCTSRECORDWIDGET_H
#define ADDPRODUCTSRECORDWIDGET_H

#include "ui_AddProductsRecordWidget.h"

#include <QCompleter>

class AddProductsRecordWidget : public QWidget, public Ui::APRWidget {
Q_OBJECT
public:
	AddProductsRecordWidget(QWidget * parent = NULL);
	virtual ~AddProductsRecordWidget();

    virtual void setVisible(bool visible);

signals:
	void canceled(bool);

public slots:
	void update_model();

private slots:
	bool insert_record();
	void clear_form();
	void cancel_form();
	void validateAdd();

private:
	QCompleter * completer_name;
	QCompleter * completer_unit;
	QCompleter * completer_expiry;
};

#endif // ADDPRODUCTSRECORDWIDGET_H
