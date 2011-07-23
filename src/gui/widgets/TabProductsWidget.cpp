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

TabProductsWidget::TabProductsWidget(QWidget *) :
	Ui::TabProductsWidget(), db(Database::Instance()), model_batch_proxyP(NULL) {

	setupUi(this);

	widget_add_products->setVisible(false);
	aprw = new AddProductsRecordWidget(widget_add_products);

	activateUi(true);

	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(add_prod_record(bool)));
	connect(aprw, SIGNAL(canceled(bool)), button_add_prod, SLOT(setChecked(bool)));
// 	connect(table_products, SIGNAL(recordsFilter(QString)), this, SLOT(set_filter(QString)));

// TODO check this
// 	connect(&db, SIGNAL(dbSaved()), aprw, SLOT(update_model()));

// 	connect(table_products, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
	connect(table_products, SIGNAL(clicked(QModelIndex)), this, SLOT(doFilterBatches(QModelIndex)));
// 	connect(table_products, SIGNAL(clicked(QModelIndex)), this, SLOT(doFilterBatches(QModelIndex)));

	table_products->setAlternatingRowColors(true);
}

TabProductsWidget::~TabProductsWidget() {
	FPR(__func__);
	activateUi(false);
	if (aprw) delete aprw;
}

void TabProductsWidget::set_filter(const QString & /*str*/) {

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
		if ((model_prod = db.CachedProducts())) {
			table_products->setModel(model_prod);
			table_products->show();
			connect(edit_filter_prod, SIGNAL(textChanged(QString)), model_prod, SLOT(filterDB(QString)));
			aprw->update_model();
		}

		// batch proxy
		model_batch_proxyP = new BatchTableModelProxyP(&pid);
		model_batch_proxyP->setSourceModel(db.CachedBatch());
		model_batch_proxyP->setDynamicSortFilter(true);
		table_batchbyid->setModel(model_batch_proxyP);
		table_batchbyid->hideColumn(BatchTableModel::HProdId);
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
	PR(idx.row());
	PR(model_prod->isDirty(idx));
	if (model_prod->isDirty(idx)) {
		table_products->setEditTriggers(QAbstractItemView::DoubleClicked);
	} else {
		table_products->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
}

void TabProductsWidget::doFilterBatches(const QModelIndex& idx) {
	pid = model_prod->index(idx.row(), ProductsTableModel::HId).data();
	model_batch_proxyP->invalidate();
	table_batchbyid->setModel(model_batch_proxyP);
	table_batchbyid->hideColumn(BatchTableModel::HProdId);
}

#include "TabProductsWidget.moc"