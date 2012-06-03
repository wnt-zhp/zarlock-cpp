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

#include "AddDistributorRecordWidget.h"

#include <QtSql>


TabDistributorWidget::TabDistributorWidget(QWidget * parent) : QWidget(parent), db(Database::Instance()), proxy_model(NULL) {
	CI();

	setupUi(this);

	widget_add_distributor->setVisible(true);
	adrw = new AddDistributorRecordWidget(widget_add_distributor);

	activateUi(true);

	connect(db, SIGNAL(dbSaved()), adrw, SLOT(update_model()));
	connect(edit_filter_batch, SIGNAL(textChanged(QString)), this, SLOT(setFilterString(QString)));
	connect(cb_hidemeals, SIGNAL(stateChanged(int)), this, SLOT(setFilter()));

	connect(model_dist, SIGNAL(rowInserted(int)), this, SLOT(markSourceRowActive(int)));

// 	connect(table_dist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editRecord(QModelIndex)));
	connect(table_dist, SIGNAL(editRecordRequested(const QVector<int> &)), this, SLOT(editRecord(const QVector<int> &)));
	connect(table_dist, SIGNAL(removeRecordRequested(const QVector<int> &, bool)), Database::Instance(), SLOT(removeDistributorRecord(const QVector<int>&, bool)));

	cb_hidemeals->setChecked(true);

// 	table_dist->setPalette(globals::item_palette);
}

TabDistributorWidget::~TabDistributorWidget() {
	DI();
	activateUi(false);
	delete adrw;
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
		if (!proxy_model) {
			proxy_model = new DistributorTableModelProxy(cb_hidemeals);
			proxy_model->setDynamicSortFilter(true);
			proxy_model->setSortCaseSensitivity(Qt::CaseInsensitive);
		}

		if ((model_dist = db->CachedDistributor())){
			proxy_model->setSourceModel(model_dist);
			table_dist->setModel(proxy_model);
			table_dist->show();
			adrw->update_model();
		}
	}
}

void TabDistributorWidget::setFilter() {
	proxy_model->invalidate();
// 	table_dist->setModel(proxy_model);
}

void TabDistributorWidget::setFilterString(const QString& string) {
	QString f = string;
	f.replace(' ', '*');
// 	proxy_model->setFilterRegExp(QRegExp(f, Qt::CaseInsensitive, QRegExp::FixedString));
	proxy_model->setFilterWildcard(f);
	proxy_model->setFilterKeyColumn(DistributorTableModel::HBatchId);

	setFilter();
}

void TabDistributorWidget::editRecord(const QVector< int >& ids) {
	for (QVector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		editRecord(db->CachedDistributor()->getIndexById(*it));
	}
}

void TabDistributorWidget::editRecord(const QModelIndex& idx) {
	adrw->prepareUpdate(proxy_model->mapToSource(idx));
}

void TabDistributorWidget::markSourceRowActive(int row) {
	int proxy_row = proxy_model->mapFromSource(model_dist->index(row, DistributorTableModel::HId)).row();
	table_dist->selectRow(row);
}

#include "TabDistributorWidget.moc"