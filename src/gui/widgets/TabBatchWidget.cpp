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
#include "TabBatchWidget.h"
#include "Database.h"
#include "DataParser.h"

TabBatchWidget::TabBatchWidget(QWidget * /*parent*/) : Ui::TabBatchWidget(), db(Database::Instance()),
	model_batch(NULL), modelproxy_batch(NULL), model_batch_delegate(NULL) {

	setupUi(this);

	QPixmap pxme(QSize(20, 20));
	pxme.fill(globals::item_expired);
	cb_expired->setIcon(pxme);
	cb_expired->setChecked(true);
	pxme.fill(globals::item_aexpired);
	cb_aexpired->setIcon(pxme);
	cb_aexpired->setChecked(true);
	pxme.fill(globals::item_nexpired);
	cb_nexpired->setIcon(pxme);
	cb_nexpired->setChecked(true);

	widget_add_batch->setVisible(false);
	abrw = new AddBatchRecordWidget(widget_add_batch);

	activateUi(true);

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(add_batch_record(bool)));
	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(abrw, SIGNAL(canceled(bool)), button_add_batch, SLOT(setChecked(bool)));

	connect(cb_expired, SIGNAL(clicked()), this, SLOT(set_filter()));
	connect(cb_aexpired, SIGNAL(clicked()), this, SLOT(set_filter()));
	connect(cb_nexpired, SIGNAL(clicked()), this, SLOT(set_filter()));

	connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_batch, SLOT(filterDB(QString)));

// 	connect(table_batch, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
	table_batch->setEditTriggers(QAbstractItemView::DoubleClicked);
}

TabBatchWidget::~TabBatchWidget() {
	FPR(__func__);
	activateUi(false);
	if (model_batch_delegate) delete model_batch_delegate;
	if (modelproxy_batch) delete modelproxy_batch;
	if (abrw) delete abrw;
}

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void TabBatchWidget::activateUi(bool activate) {
// 	this->setVisible(activate);

	if (activate) {
		if (!modelproxy_batch) {
			modelproxy_batch = new BatchTableModelProxy(cb_expired, cb_aexpired, cb_nexpired);
			modelproxy_batch->setDynamicSortFilter(true);

			// TODO: Co z tym zrobić?
			connect(cb_expired, SIGNAL(clicked()), this, SLOT(set_filter()));
			connect(cb_aexpired, SIGNAL(clicked()), this, SLOT(set_filter()));
			connect(cb_nexpired, SIGNAL(clicked()), this, SLOT(set_filter()));
		}
		// batch
		if ((model_batch = db.CachedBatch())){
			modelproxy_batch->setSourceModel(model_batch);
			table_batch->setModel(modelproxy_batch);

			if (model_batch_delegate) delete model_batch_delegate;
			model_batch_delegate = new QSqlRelationalDelegate(table_batch);
			table_batch->setItemDelegate(model_batch_delegate);
			table_batch->show();
			// TODO: Co z tym zrobić?
			connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_batch, SLOT(filterDB(QString)));
			abrw->update_model();
		}
	}
}

void TabBatchWidget::add_batch_record(bool newrec) {
	if (newrec) {
// 		table_products->setVisible(false);
		widget_add_batch->setVisible(true);
	} else {
// 		table_products->setVisible(true);
		widget_add_batch->setVisible(false);
	}
}

void TabBatchWidget::edit_record(const QModelIndex& idx) {
	if (model_batch->isDirty(idx)) {
		table_batch->setEditTriggers(QAbstractItemView::DoubleClicked);
	} else {
		table_batch->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
}

void TabBatchWidget::set_filter() {
	modelproxy_batch->invalidate();
	table_batch->setModel(modelproxy_batch);
}

#include "TabBatchWidget.moc"