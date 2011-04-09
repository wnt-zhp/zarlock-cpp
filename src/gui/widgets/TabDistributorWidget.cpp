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
#include "TabDistributorWidget.h"
#include "Database.h"
#include "DataParser.h"

TabDistributorWidget::TabDistributorWidget(QWidget * parent) : QWidget(parent), db(Database::Instance()),
	model_dist_delegate(NULL) {

	setupUi(this);

	widget_add_distributor->setVisible(true);
	adrw = new AddDistributorRecordWidget(widget_add_distributor);

	activateUi(false);

	connect(table_dist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
}

TabDistributorWidget::~TabDistributorWidget() {
	if (adrw) delete adrw;
}

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void TabDistributorWidget::activateUi(bool activate) {
// 	this->setVisible(activate);

	if (activate) {
		// dist
		if ((model_dist = db.CachedDistributor())){
			table_dist->setModel(model_dist);
			if (model_dist_delegate) delete model_dist_delegate;
			model_dist_delegate = new QSqlRelationalDelegate(table_dist);
			table_dist->setItemDelegate(model_dist_delegate);
			table_dist->show();
// 			connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_dist, SLOT(filterDB(QString)));
			adrw->update_model();
		}
	}
}

// void TabDistributorWidget::add_batch_record(bool newrec) {
// 	if (newrec) {
// // 		table_products->setVisible(false);
// 		widget_add_batch->setVisible(true);
// 	} else {
// // 		table_products->setVisible(true);
// 		widget_add_batch->setVisible(false);
// 	}
// }

void TabDistributorWidget::edit_record(const QModelIndex& idx) {
	if (model_dist->isDirty(idx)) {
		table_dist->setEditTriggers(QAbstractItemView::DoubleClicked);
	} else {
		table_dist->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}
}

#include "TabDistributorWidget.moc"