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


#ifndef TABBATCHWIDGET_H
#define TABBATCHWIDGET_H

#include "ui_TabBatchWidget.h"

#include "BatchTableModel.h"
#include "BatchTableModelProxy.h"

#include <QtSql>
#include <QCompleter>

#include "DimmingWidget.h"
// #include "DimmingMessage.h"

class BatchRecordWidget;

class TabBatchWidget : public QWidget, public Ui::TabBatchWidget {
Q_OBJECT
public:
	TabBatchWidget(QWidget * parent = NULL);
	virtual ~TabBatchWidget();

private:
	void activateUi(bool activate = true);

private slots:
	void addRecord(bool newrec = true);
	void editRecord(const QVector<int> & ids);
	void setFilter();
	void setFilterString(const QString & string);

private:
	Database * db;
	BatchTableModel * model_batch;
	BatchTableModelProxy * proxy_model;
	QSqlRelationalDelegate * model_batch_delegate;

	BatchRecordWidget * brw;

	DimmingWidget * dwbox;
};

#endif // TABBATCHWIDGET_H
