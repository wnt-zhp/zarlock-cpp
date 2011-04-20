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


#ifndef ADDFOODRECORDWIDGET_H
#define ADDFOODRECORDWIDGET_H

#include <QCompleter>

#include "ui_AddFoodRecordWidget.h"

class AddFoodRecordWidget : public QWidget, public Ui::AFRWidget {
Q_OBJECT
public:
	AddFoodRecordWidget(QWidget * parent = NULL);
	virtual ~AddFoodRecordWidget();

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
	QCompleter * completer_qty;
};

#endif // ADDFOODRECORDWIDGET_H
