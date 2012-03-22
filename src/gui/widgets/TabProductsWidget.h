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


#ifndef TABPRODUCTSWIDGET_H
#define TABPRODUCTSWIDGET_H

#include "ui_TabProductsWidget.h"

#include "Database.h"
#include "ProductsTableModel.h"
#include "AddProductsRecordWidget.h"

#include <QCompleter>
#include <BatchTableModelProxyP.h>
#include <DistributorTableModelProxyP.h>

class TabProductsWidget : public QWidget, public Ui::TabProductsWidget {
Q_OBJECT
public:
	TabProductsWidget(QWidget * parent = NULL);
	virtual ~TabProductsWidget();

private:
	void activateUi(bool activate = true);

private slots:
	void set_filter(const QString & str);
	void add_prod_record(bool newrec = true);
	void edit_record(const QModelIndex & idx);
	void doFilterBatches(const QModelIndex & idx);
	void doFilterDistributions(const QModelIndex & idx);

private:
	Database * db;
	ProductsTableModel * model_prod;
	BatchTableModelProxyP * model_batch_proxyP;
	DistributorTableModelProxyP * model_distributor_proxyP;

	AddProductsRecordWidget * aprw;
	QVariant pid;
};

#endif // TABPRODUCTSWIDGET_H
