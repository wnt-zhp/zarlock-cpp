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
#include "DistributorTableModelProxy.h"

#include "DimmingWidget.h"

class DistributorTableModel;
class DistributorRecordWidget;

class TabDistributorWidget : public QWidget, public Ui::TabDistributorWidget {
Q_OBJECT
public:
	TabDistributorWidget(QWidget * parent = NULL);
	virtual ~TabDistributorWidget();

public slots:
	virtual void markSourceRowActive(int row);

private:
	void activateUi(bool activate = true);

private slots:
	void addRecord(bool newrec = true);

	void editRecord(const QModelIndex & idx);
	void editRecord(const QVector<int> & ids);

	void setFilter();
	void setFilterString(const QString & string);

private:
	Database * db;
	DistributorTableModel * model_dist;

	DistributorTableModelProxy * proxy_model;

	DistributorRecordWidget * drw;

	DimmingWidget * dwbox;
};

#endif // TABDISTRIBUTORWIDGET_H
