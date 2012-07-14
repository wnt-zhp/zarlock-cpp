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

#include "globals.h"
#include "TabProductsWidget.h"
#include "Database.h"
#include "DataParser.h"

#include "ProductsTableModel.h"
#include "ProductsRecordWidget.h"

#include <QStringBuilder>

TabProductsWidget::TabProductsWidget(QWidget *) :
	Ui::TabProductsWidget(), db(Database::Instance()),
	proxy_model(NULL), proxy_model_batch(NULL), proxy_model_distributor(NULL) {

	setupUi(this);

// 	widget_add_products->setVisible(false);
	prw = new ProductsRecordWidget(widget_add_products);

	table_batchbyid->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table_distributorbyid->setEditTriggers(QAbstractItemView::NoEditTriggers);

	activateUi(true);

#warning FIXME: double addRecord for closed() -> InsertAction, but not for UpdateAction, Add button must be disabled
	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(addRecord(bool)));
	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(prw, SIGNAL(closed(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(prw, SIGNAL(closed(bool)), this, SLOT(addRecord(bool)));

// 	connect(table_products, SIGNAL(recordsFilter(QString)), this, SLOT(set_filter(QString)));

	connect(table_products, SIGNAL(activated(QModelIndex)), this, SLOT(doFilterBatches()));
	connect(table_batchbyid, SIGNAL(activated(QModelIndex)), this, SLOT(doFilterDistributions()));

	connect(table_products, SIGNAL(editRecordRequested(const QVector<int> &)), this, SLOT(editRecord(const QVector<int> &)));
	connect(table_products, SIGNAL(removeRecordRequested(const QVector<int> &, bool)), Database::Instance(), SLOT(removeProductRecord(const QVector<int> &, bool)));

	table_products->setAlternatingRowColors(true);
	table_distributorbyid->setAlternatingRowColors(true);
	table_batchbyid->setSelectionMode(QAbstractItemView::ExtendedSelection);

	globals::appSettings->beginGroup("TabProducts");
	splitter_H->restoreState(globals::appSettings->value("splitterHoriz", QByteArray()).toByteArray());
	splitter_V->restoreState(globals::appSettings->value("splitterVert", QByteArray()).toByteArray());
	globals::appSettings->endGroup();

	dwbox = new DimmingWidget(this);

	dwbox->setOverlayAnimated(true);
	dwbox->setOverlayStyled(true);
	dwbox->setOverlayDefaultOpacity(100);
	dwbox->setWidget(widget_add_products);
}

TabProductsWidget::~TabProductsWidget() {
	DI();

	globals::appSettings->beginGroup("TabProducts");
	globals::appSettings->setValue("splitterHoriz", splitter_H->saveState());
	globals::appSettings->setValue("splitterVert", splitter_V->saveState());
	globals::appSettings->endGroup();

	activateUi(false);
	if (prw) delete prw;
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

void TabProductsWidget::addRecord(bool newrec) {
	if (newrec) {
		prw->setInsertMode();

		dwbox->setEventTransparent(false);
		dwbox->showWidget(true);
	} else {
		dwbox->hideWidget();
	}
}

void TabProductsWidget::editRecord(const QVector< int >& ids) {
	for (QVector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		editRecord(db->CachedProducts()->match(db->CachedProducts()->index(0, ProductsTableModel::HId), Qt::EditRole, *it, 1, Qt::MatchExactly).at(0));
	}
}

void TabProductsWidget::editRecord(const QModelIndex& idx) {
	prw->setUpdateMode(idx);
	dwbox->setEventTransparent(true);
	dwbox->showWidget();
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