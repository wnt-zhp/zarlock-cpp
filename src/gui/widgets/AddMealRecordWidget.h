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


#ifndef ADDMEALRECORDWIDGET_H
#define ADDMEALRECORDWIDGET_H

#include <QCompleter>
#include <QTreeWidgetItem>

#include "ui_AddMealRecordWidget.h"

class AddMealRecordWidget : public QWidget, public Ui::AMRWidget {
Q_OBJECT
public:
	AddMealRecordWidget(const QDate & bd, const QDate & ed, QWidget * parent = NULL);
	virtual ~AddMealRecordWidget();

signals:
	void canceled(bool);
	void dbmealupdated(bool);

public slots:
	void showW(QTreeWidgetItem * item, int column);
	void update_model();

private slots:
	bool insert_record();
	void cancel_form();

private:

};

#endif // ADDMEALRECORDWIDGET_H
