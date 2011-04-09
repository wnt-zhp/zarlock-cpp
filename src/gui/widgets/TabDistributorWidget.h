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


#ifndef TABDISTRIBUTORWIDGET_H
#define TABDISTRIBUTORWIDGET_H

#include "ui_TabDistributorWidget.h"

#include "Database.h"
#include "ProductsTableModel.h"
#include "AddDistributorRecordWidget.h"

#include <QtSql>
#include <QCompleter>

class TabDistributorWidget : public QWidget, public Ui::TabDistributorWidget {
Q_OBJECT
public:
	TabDistributorWidget(QWidget * parent = NULL);
	virtual ~TabDistributorWidget();

public slots:
	void activateUi(bool activate = true);

private slots:
// 	void add_batch_record(bool newrec = true);
	void edit_record(const QModelIndex & idx);

private:
	Database & db;
	DistributorTableModel * model_dist;
	QSqlRelationalDelegate * model_dist_delegate;

	AddDistributorRecordWidget * adrw;
};

#endif // TABDISTRIBUTORWIDGET_H
