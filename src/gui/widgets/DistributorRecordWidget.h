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


#ifndef DISTRIBUTERECORDWIDGET_H
#define DISTRIBUTERECORDWIDGET_H

#include "ui_DistributorRecordWidget.h"
#include "BatchTableModelProxy.h"
#include "AbstractRecordWidget.h"

class QCompleter;
class QProxyModel;
class BatchTableView;
class EventFilter;
class TextInput;

class DistributorRecordWidget : public AbstractRecordWidget, public Ui::DRWidget {
Q_OBJECT
public:
	DistributorRecordWidget(QWidget * parent = NULL);
	virtual ~DistributorRecordWidget();

private slots:
	void insertRecord();
	void clearForm();
	void validateDistDate();
	void validateData();
	void prepareWidget();

private:
	void prepareInsert();
	void prepareUpdate();
	void prepareView();

private:
	QCompleter * completer_qty;
	QCompleter * completer_date;
	QCompleter * completer_reason;
	QCompleter * completer_reason2;

	BatchTableModelProxy * proxy;
	QCheckBox * hideempty;

// 	const QModelIndex * indexToUpdate;
// 	QModelIndex copyOfIndexToUpdate;
// 	QModelIndex sourceIndex;
	int sourceRowToUpdate;
	int disttype;

	BatchTableView * tv;
};

#endif // DISTRIBUTERECORDWIDGET_H
