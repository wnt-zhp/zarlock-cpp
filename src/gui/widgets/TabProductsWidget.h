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


#ifndef TABPRODUCTSWIDGET_H
#define TABPRODUCTSWIDGET_H

#include "ui_TabProductsWidget.h"

#include "Database.h"
#include "ProductsTableModel.h"

#include <QCompleter>
#include <BatchTableModelProxyP.h>
#include <DistributorTableModelProxyP.h>

#include "DimmingWidget.h"

class ProductsRecordWidget;

class TabProductsWidget : public QWidget, public Ui::TabProductsWidget {
Q_OBJECT
public:
	TabProductsWidget(QWidget * parent = NULL);
	virtual ~TabProductsWidget();

private:
	void activateUi(bool activate = true);

private slots:
	void setFilter();
	void setFilterString(const QString & string);

// 	void add_prod_record(bool newrec = true);
	void doFilterBatches();
	void doFilterDistributions();

	void addRecord(bool newrec = true);
	void editRecord(const QVector<int> & ids);
	void editRecord(const QModelIndex & idx);

private:
	Database * db;
	ProductsTableModel * model_prod;
	QSortFilterProxyModel * proxy_model;
	QSortFilterProxyModel * proxy_model_batch;
	QSortFilterProxyModel * proxy_model_distributor;

	ProductsRecordWidget * prw;
	QVariant pid;

	DimmingWidget * dwbox;
};

#endif // TABPRODUCTSWIDGET_H
