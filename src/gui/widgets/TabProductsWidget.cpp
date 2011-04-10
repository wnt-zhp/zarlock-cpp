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

TabProductsWidget::TabProductsWidget(QWidget * parent) :
	Ui::TabProductsWidget(), db(Database::Instance()), model_batchbyid(NULL) {

	setupUi(this);

	widget_add_products->setVisible(false);
	aprw = new AddProductsRecordWidget(widget_add_products);

	activateUi(true);

	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(add_prod_record(bool)));
	connect(aprw, SIGNAL(canceled(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(table_products, SIGNAL(recordsFilter(QString)), this, SLOT(set_filter(QString)));

// 	connect(table_products, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));

	table_products->setEditTriggers(QAbstractItemView::DoubleClicked);
// 	table_products->setAlternatingRowColors(true);
}

TabProductsWidget::~TabProductsWidget() {
	activateUi(false);
	if (aprw) delete aprw;
}

void TabProductsWidget::set_filter(const QString & str) {
	QString q("select \"spec\", \"used_qty\" from batch");
	q.append(str);
	if (table_batchbyid->model() != NULL) {
		((QSqlQueryModel *)table_batchbyid->model())->setQuery(q);
		model_batchbyid->setQuery(q);
	}
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
		// products
		if ((model_prod = db.CachedProducts())) {
			table_products->setModel(model_prod);
			table_products->show();
			connect(edit_filter_prod, SIGNAL(textChanged(QString)), model_prod, SLOT(filterDB(QString)));
			aprw->update_model();
		}
		if (model_batchbyid != NULL) delete model_batchbyid;
		if (model_batchbyid = new QSqlQueryModel()) {
// 			model_batchbyid->setQuery("select \"spec\", \"used_qty\" from batch");
			table_batchbyid->setModel(model_batchbyid);
			table_batchbyid->show();
		}
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

#include "TabProductsWidget.moc"