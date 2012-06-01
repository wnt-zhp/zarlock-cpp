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

#include "globals.h"
#include "TabProductsWidget.h"
#include "Database.h"
#include "DataParser.h"

#include <QStringBuilder>

TabProductsWidget::TabProductsWidget(QWidget *) :
	Ui::TabProductsWidget(), db(Database::Instance()),
	proxy_model(NULL), proxy_model_batch(NULL), proxy_model_distributor(NULL) {

	setupUi(this);

	widget_add_products->setVisible(false);
	aprw = new AddProductsRecordWidget(widget_add_products);

	table_batchbyid->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table_distributorbyid->setEditTriggers(QAbstractItemView::NoEditTriggers);

	activateUi(true);

	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(add_prod_record(bool)));
	connect(aprw, SIGNAL(canceled(bool)), button_add_prod, SLOT(setChecked(bool)));
// 	connect(table_products, SIGNAL(recordsFilter(QString)), this, SLOT(set_filter(QString)));

	connect(table_products, SIGNAL(activated(QModelIndex)), this, SLOT(doFilterBatches()));
	connect(table_batchbyid, SIGNAL(activated(QModelIndex)), this, SLOT(doFilterDistributions()));

	connect(table_products, SIGNAL(removeRecordRequested(const QVector<int> &, bool)), Database::Instance(), SLOT(removeProductsRecord(const QVector<int> &, bool)));

	table_products->setAlternatingRowColors(true);
	table_distributorbyid->setAlternatingRowColors(true);
	table_batchbyid->setSelectionMode(QAbstractItemView::ExtendedSelection);

	globals::appSettings->beginGroup("TabProducts");
	splitter_H->restoreState(globals::appSettings->value("splitterHoriz", QByteArray()).toByteArray());
	splitter_V->restoreState(globals::appSettings->value("splitterVert", QByteArray()).toByteArray());
	globals::appSettings->endGroup();
}

TabProductsWidget::~TabProductsWidget() {
	DI();
	
	globals::appSettings->beginGroup("TabProducts");
	globals::appSettings->setValue("splitterHoriz", splitter_H->saveState());
	globals::appSettings->setValue("splitterVert", splitter_V->saveState());
	globals::appSettings->endGroup();

	activateUi(false);
	if (aprw) delete aprw;
}

void TabProductsWidget::setFilter() {
	proxy_model->invalidate();
}

void TabProductsWidget::setFilterString(const QString & string) {
	QString f = string;
	f.replace(' ', '*');
	// 	proxy_model->setFilterRegExp(QRegExp(f, Qt::CaseInsensitive, QRegExp::FixedString));
	proxy_model->setFilterWildcard(f);
	proxy_model->setFilterKeyColumn(DistributorTableModel::HBatchId);

	setFilter();
}

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void TabProductsWidget::activateUi(bool activate) {
// 	this->setVisible(activate);

	if (activate) {
// 		// products
		if (!proxy_model) {
			proxy_model = new QSortFilterProxyModel();
			proxy_model->setDynamicSortFilter(true);
			proxy_model->setSortCaseSensitivity(Qt::CaseInsensitive);
		}

		if ((model_prod = db->CachedProducts())) {
			proxy_model->setSourceModel(model_prod);
			table_products->setModel(proxy_model);
			table_products->show();
			connect(edit_filter_prod, SIGNAL(textChanged(QString)), this, SLOT(setFilterString(QString)));
			aprw->update_model();
		}

		// batch proxy
		proxy_model_batch = new QSortFilterProxyModel;
		proxy_model_batch->setSourceModel(db->CachedBatch());
		proxy_model_batch->setDynamicSortFilter(true);
		proxy_model_batch->setFilterKeyColumn(BatchTableModel::HProdId);
		proxy_model_batch->setFilterRole(Qt::EditRole);

		proxy_model_distributor = new QSortFilterProxyModel;
		proxy_model_distributor->setSourceModel(db->CachedDistributor());
		proxy_model_distributor->setDynamicSortFilter(true);
		proxy_model_distributor->setFilterKeyColumn(DistributorTableModel::HBatchId);
		proxy_model_distributor->setFilterRole(Qt::EditRole);
	}
}

void TabProductsWidget::add_prod_record(bool newrec) {
	if (newrec) {
// 		table_products->setVisible(false);
		widget_add_products->setVisible(true);
	} else {
// 		table_products->setVisible(true);
		widget_add_products->setVisible(false);
	}
}

void TabProductsWidget::edit_record(const QModelIndex& idx) {
	if (model_prod->isDirty(idx)) {
		table_products->setEditTriggers(QAbstractItemView::DoubleClicked);
	} else {
		table_products->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
}

void TabProductsWidget::doFilterBatches() {
	QModelIndexList l = table_products->selectionModel()->selectedRows();
	QString tstr;
	for (int b = 0; b < l.size(); ++b) {
		QString str = proxy_model->index(l.at(b).row(), ProductsTableModel::HId).data(Qt::EditRole).toString();
		tstr = tstr % "^" % str % "$";
		if (b < l.size()-1)
			tstr = tstr % "|";
	}

	QString filter = tstr;
	proxy_model_batch->setFilterRegExp(QRegExp(filter, Qt::CaseInsensitive));
	proxy_model_batch->invalidate();
	table_batchbyid->setModel(proxy_model_batch);
}

void TabProductsWidget::doFilterDistributions() {
	QModelIndexList l = table_batchbyid->selectionModel()->selectedRows();
	QString tstr;
	for (int b = 0; b < l.size(); ++b) {
		QString str = proxy_model_batch->index(l.at(b).row(), BatchTableModel::HId).data(Qt::EditRole).toString();
		tstr = tstr % "^" % str % "$";
		if (b < l.size()-1)
			tstr = tstr % "|";
	}

	QString filter = tstr;
	proxy_model_distributor->setFilterRegExp(QRegExp(filter, Qt::CaseInsensitive));
	proxy_model_distributor->invalidate();
	table_distributorbyid->setModel(proxy_model_distributor);
}

#include "TabProductsWidget.moc"