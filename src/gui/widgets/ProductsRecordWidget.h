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


#ifndef PRODUCTSRECORDWIDGET_H
#define PRODUCTSRECORDWIDGET_H

#include "ui_ProductsRecordWidget.h"

#include <QCompleter>

class ProductsRecordWidget : public QWidget, public Ui::PRWidget {
Q_OBJECT
public:
	ProductsRecordWidget(QWidget * parent = NULL);
	virtual ~ProductsRecordWidget();

	virtual void setVisible(bool visible);

signals:
	void canceled(bool);

public slots:
	void update_model();
	void prepareInsert(bool visible);
	void prepareUpdate(const QModelIndex & idx);

private slots:
	void insertRecord();
	void insertRecordExit();
	void clearForm();
	void cancelForm();
	void validateAdd();

private:
	QCompleter * completer_name;
	QCompleter * completer_unit;
	QCompleter * completer_expiry;

	int idToUpdate;
	QString button_label_insert;
	QString button_label_insert_and_exit;
	QString button_label_exit;
};

#endif // PRODUCTSRECORDWIDGET_H
