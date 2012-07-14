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
#include "TabDistributorWidget.h"
#include "Database.h"
#include "DataParser.h"

#include "DistributorRecordWidget.h"

#include <QtSql>


TabDistributorWidget::TabDistributorWidget(QWidget * parent) : QWidget(parent), db(Database::Instance()), proxy_model(NULL) {
	CI();

	setupUi(this);

// 	widget_add_distributor->setVisible(true);
	drw = new DistributorRecordWidget(widget_add_distributor);

	activateUi(true);

// 	connect(db, SIGNAL(dbSaved()), drw, SLOT(update_model()));

	connect(button_add_distribution, SIGNAL(toggled(bool)), this, SLOT(addRecord(bool)));
	connect(drw, SIGNAL(closed(bool)), button_add_distribution, SLOT(setChecked(bool)));
	connect(drw, SIGNAL(closed(bool)), this, SLOT(addRecord(bool)));

// 	connect(table_dist, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editRecord(QModelIndex)));
	connect(table_dist, SIGNAL(addRecordRequested(bool)), button_add_distribution, SLOT(setChecked(bool)));
	connect(table_dist, SIGNAL(editRecordRequested(const QVector<int> &)), this, SLOT(editRecord(const QVector<int> &)));
	connect(table_dist, SIGNAL(removeRecordRequested(const QVector<int> &, bool)), Database::Instance(), SLOT(removeDistributorRecord(const QVector<int>&, bool)));

	connect(model_dist, SIGNAL(rowInserted(int)), this, SLOT(markSourceRowActive(int)));

	connect(cb_hidemeals, SIGNAL(stateChanged(int)), this, SLOT(setFilter()));

	connect(edit_filter_batch, SIGNAL(textChanged(QString)), this, SLOT(setFilterString(QString)));


	cb_hidemeals->setChecked(true);

// 	table_dist->setPalette(globals::item_palette);

	dwbox = new DimmingWidget(this);

	dwbox->setOverlayAnimated(true);
	dwbox->setOverlayStyled(true);
	dwbox->setOverlayDefaultOpacity(100);
	dwbox->setWidget(widget_add_distributor);
}

TabDistributorWidget::~TabDistributorWidget() {
	DI();
	activateUi(false);
	delete drw;
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

void TabDistributorWidget::addRecord(bool newrec) {
	if (newrec) {
		drw->setInsertMode();
		// 		widget_add_batch->setVisible(newrec);
		dwbox->setEventTransparent(false);
		dwbox->showWidget();
	} else {
		dwbox->hideWidget();
		// 		widget_add_batch->setVisible(newrec);
	}
}

void TabDistributorWidget::editRecord(const QVector< int >& ids) {
	for (QVector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		editRecord(db->CachedDistributor()->getIndexById(*it));
// 		drw->setUpdateMode(db->CachedDistributor()->getIndexById(*it));
// // 		widget_add_distributor->setVisible(true);
// 		dwbox->setEventTransparent(true);
// 		dwbox->showWidget();
	}
}

void TabDistributorWidget::editRecord(const QModelIndex& idx) {
// 	drw->setUpdateMode(proxy_model->mapFromSource(idx));
	drw->setUpdateMode(idx);
// 	widget_add_distributor->setVisible(true);
	dwbox->setEventTransparent(true);
	dwbox->showWidget();
}

void TabDistributorWidget::markSourceRowActive(int row) {
	int proxy_row = proxy_model->mapFromSource(model_dist->index(row, DistributorTableModel::HId)).row();
	table_dist->selectRow(row);
}

#include "TabDistributorWidget.moc"