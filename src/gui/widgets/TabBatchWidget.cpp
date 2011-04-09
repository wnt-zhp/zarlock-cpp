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

TabBatchWidget::TabBatchWidget(QWidget * parent) : Ui::TabBatchWidget(), db(Database::Instance()),
	model_batch_delegate(NULL) {

	setupUi(this);

	widget_add_batch->setVisible(false);
	abrw = new AddBatchRecordWidget(widget_add_batch);

	activateUi(false);

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(add_batch_record(bool)));
	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(abrw, SIGNAL(canceled(bool)), button_add_batch, SLOT(setChecked(bool)));

	connect(table_batch, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
}

TabBatchWidget::~TabBatchWidget() {
	if (model_batch_delegate) delete model_batch_delegate;
	if (abrw) delete abrw;
}

void TabBatchWidget::setVisible(bool visible) {
	if (visible) {
		activateUi(true);
	} else {
// 		if (model_batch->isDirty())
		if (model_batch)
			model_batch->submitAll();
	}

    QWidget::setVisible(visible);
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
		// batch
		if ((model_batch = db.CachedBatch())){
			table_batch->setModel(model_batch);
			if (model_batch_delegate) delete model_batch_delegate;
			model_batch_delegate = new QSqlRelationalDelegate(table_batch);
			table_batch->setItemDelegate(model_batch_delegate);
			table_batch->show();
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

#include "TabBatchWidget.moc"