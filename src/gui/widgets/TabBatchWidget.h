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


#ifndef TABBATCHWIDGET_H
#define TABBATCHWIDGET_H

#include "ui_TabBatchWidget.h"

#include "Database.h"
#include "BatchTableModel.h"
#include "BatchTableModelProxy.h"
#include "AddBatchRecordWidget.h"

#include <QtSql>
#include <QCompleter>

class TabBatchWidget : public QWidget, public Ui::TabBatchWidget {
Q_OBJECT
public:
	TabBatchWidget(QWidget * parent = NULL);
	virtual ~TabBatchWidget();

public slots:
	virtual void doCreateSMreports();
	virtual void doCreateKMreports();

private:
	void activateUi(bool activate = true);

private slots:
	void add_batch_record(bool newrec = true);
	void edit_record(const QModelIndex & idx);
	void set_filter();

	virtual void syncDB();

private:
	Database & db;
	BatchTableModel * model_batch;
	BatchTableModelProxy * modelproxy_batch;
	QSqlRelationalDelegate * model_batch_delegate;

	AddBatchRecordWidget * abrw;

	QAction * syncdb;
	QAction * createSMrep;
	QAction * createKMrep;
};

#endif // TABBATCHWIDGET_H
