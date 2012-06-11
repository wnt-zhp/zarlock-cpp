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
#include "TabBatchWidget.h"
#include <DimmingMessage.h>
#include "Database.h"
#include "DataParser.h"

#include "BatchRecordWidget.h"

TabBatchWidget::TabBatchWidget(QWidget * /*parent*/) : Ui::TabBatchWidget(), db(Database::Instance()),
	model_batch(NULL), proxy_model(NULL)/*, model_batch_delegate(NULL)*/ {

	setupUi(this);

	QPixmap pxme(QSize(20, 20));
	pxme.fill(globals::item_expired_altbase);
	cb_expired->setIcon(pxme);
	cb_expired->setChecked(true);
	pxme.fill(globals::item_aexpired_altbase);
	cb_aexpired->setIcon(pxme);
	cb_aexpired->setChecked(true);
	pxme.fill(globals::item_nexpired_altbase);
	cb_nexpired->setIcon(pxme);
	cb_nexpired->setChecked(true);

	cb_hideempty->setChecked(false);

	widget_add_batch->setVisible(false);
	brw = new BatchRecordWidget(widget_add_batch);

	activateUi(true);

	list_messages->setVisible(false);

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(addRecord(bool)));
	connect(brw, SIGNAL(closed(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(brw, SIGNAL(closed(bool)), this, SLOT(addRecord(bool)));

	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(table_batch, SIGNAL(editRecordRequested(const QVector<int> &)), this, SLOT(editRecord(const QVector<int> &)));
	connect(table_batch, SIGNAL(removeRecordRequested(const QVector<int> &, bool)), Database::Instance(), SLOT(removeBatchRecord(const QVector<int>&, bool)));

	connect(model_batch, SIGNAL(rowInserted(int)), table_batch, SLOT(selectRow(int)));

	connect(cb_expired, SIGNAL(clicked()), this, SLOT(setFilter()));
	connect(cb_aexpired, SIGNAL(clicked()), this, SLOT(setFilter()));
	connect(cb_nexpired, SIGNAL(clicked()), this, SLOT(setFilter()));
	connect(cb_hideempty, SIGNAL(clicked()), this, SLOT(setFilter()));

	connect(edit_filter_batch, SIGNAL(textChanged(QString)), this, SLOT(setFilterString(QString)));

// 	connect(table_batch, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));

	dwbox = new DimmingWidget(this);
	
	dwbox->setOverlay(true, true);
	dwbox->setWidget(widget_add_batch);
	dwbox->setOverlayOpacity(100);
// 
// 	widget_add_batch->setVisible(true);
}

TabBatchWidget::~TabBatchWidget() {
	FI();
	activateUi(false);
// 	if (model_batch_delegate) delete model_batch_delegate;
	if (proxy_model) delete proxy_model;
	if (brw) delete brw;
}

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void TabBatchWidget::activateUi(bool activate) {
	if (activate) {
		if (!proxy_model) {
			proxy_model = new BatchTableModelProxy(cb_expired, cb_aexpired, cb_nexpired, cb_hideempty);
			proxy_model->setDynamicSortFilter(true);
			proxy_model->setSortCaseSensitivity(Qt::CaseInsensitive);
		}
		// batch
		if ((model_batch = db->CachedBatch())){
			proxy_model->setSourceModel(model_batch);
			table_batch->setModel(proxy_model);

			table_batch->show();
			brw->update_model();

			connect(model_batch, SIGNAL(dataChanged(QModelIndex,QModelIndex)), proxy_model, SLOT(invalidate()));
		}
	}
}

void TabBatchWidget::addRecord(bool newrec) {
// 	DimmingMessage * mbox = new DimmingMessage(this);
// 	mbox->setOverlay(true, true);
// 	mbox->setMessage("My message for you");
// 	QIcon ic = QApplication::style()->standardIcon(QStyle::SP_DesktopIcon);
// 	mbox->setIcon(&ic);
// 	mbox->showBusy();
// 	mbox->go();

// 	dwbox->setWidget(widget_add_batch);

	if (newrec) {
		brw->prepareInsert(newrec);
// 		widget_add_batch->setVisible(newrec);
		dwbox->go(true);
	} else {
		dwbox->og();
		dwbox->setEventTransparent(false);
// 		widget_add_batch->setVisible(newrec);
	}
}

void TabBatchWidget::editRecord(const QVector<int>& ids) {
	for (QVector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		brw->prepareUpdate(db->CachedBatch()->getIndexById(*it));
		widget_add_batch->setVisible(true);
		dwbox->go();
		dwbox->setEventTransparent(true);
	}
}

void TabBatchWidget::setFilter() {
	proxy_model->invalidate();
	table_batch->setModel(proxy_model);
}

void TabBatchWidget::setFilterString(const QString& string) {
	QString f = string;
	f.replace(' ', '*');
// 	proxy_model->setFilterRegExp(QRegExp(f, Qt::CaseInsensitive, QRegExp::Wildcard));
	proxy_model->setFilterWildcard(f);
	proxy_model->setFilterKeyColumn(BatchTableModel::HSpec);
	proxy_model->setFilterRole(Qt::UserRole);
}

#include "TabBatchWidget.moc"
