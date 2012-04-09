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
#include "DBReports.h"

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
	abrw = new AddBatchRecordWidget(widget_add_batch);

	activateUi(true);

	list_messages->setVisible(false);

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(addBatchRecord(bool)));
	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(abrw, SIGNAL(canceled(bool)), button_add_batch, SLOT(setChecked(bool)));

	connect(cb_expired, SIGNAL(clicked()), this, SLOT(setFilter()));
	connect(cb_aexpired, SIGNAL(clicked()), this, SLOT(setFilter()));
	connect(cb_nexpired, SIGNAL(clicked()), this, SLOT(setFilter()));
	connect(cb_hideempty, SIGNAL(clicked()), this, SLOT(setFilter()));

	connect(edit_filter_batch, SIGNAL(textChanged(QString)), this, SLOT(setFilterString(QString)));

// 	connect(table_batch, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(edit_record(QModelIndex)));
// 	table_batch->setEditTriggers(QAbstractItemView::DoubleClicked);

	syncdb = new QAction(tr("Sync database"), this);
	createSMrep = new QAction(tr("Create SM reports"), this);
	createKMrep = new QAction(tr("Create KM reports"), this);
// 	browsePDF = new QAction(style()->standardIcon(QStyle::SP_DirHomeIcon), tr("Browse reports directory"), this);

	tools->setPopupMode(QToolButton::InstantPopup);
	tools->setIcon(QIcon(":/resources/icons/tools-wizard.png"));
	tools->setText(tr("Tools"));
	tools->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

	tools->addAction(syncdb);
	tools->addAction(createSMrep);
	tools->addAction(createKMrep);

// 	connect(syncdb, SIGNAL(triggered(bool)), this, SLOT(syncDB()));
	connect(createSMrep, SIGNAL(triggered(bool)), this, SLOT(doCreateSMreports()));
	connect(createKMrep, SIGNAL(triggered(bool)), this, SLOT(doCreateKMreports()));

	connect(model_batch, SIGNAL(rowInserted(int)), table_batch, SLOT(selectRow(int)));

	connect(table_batch, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editRecord(QModelIndex)));
	connect(table_batch, SIGNAL(removeRecordRequested(QVector<int> &, bool)), Database::Instance(), SLOT(removeBatchRecord(QVector<int>&, bool)));
}

TabBatchWidget::~TabBatchWidget() {
	FPR(__func__);
	activateUi(false);
// 	if (model_batch_delegate) delete model_batch_delegate;
	if (proxy_model) delete proxy_model;
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
			abrw->update_model();

			connect(model_batch, SIGNAL(dataChanged(QModelIndex,QModelIndex)), proxy_model, SLOT(invalidate()));
		}
	}
}

void TabBatchWidget::addBatchRecord(bool newrec) {
	abrw->prepareInsert(newrec);
// 	if (newrec) {
// // 		table_products->setVisible(false);
// 		widget_add_batch->setVisible(true);
// 	} else {
// // 		table_products->setVisible(true);
// 		widget_add_batch->setVisible(false);
// 	}
	widget_add_batch->setVisible(newrec);
}

void TabBatchWidget::editRecord(const QModelIndex& idx) {
// 	if (model_batch->isDirty(idx)) {
// 		table_batch->setEditTriggers(QAbstractItemView::DoubleClicked);
// 	} else {
// 		table_batch->setEditTriggers(QAbstractItemView::NoEditTriggers);
// 	}

// 	abrw->prepareUpdate(idx.model()->index(idx.row(), BatchTableModel::HId).data().toUInt());
	abrw->prepareUpdate(idx);
	widget_add_batch->setVisible(true);
}

void TabBatchWidget::setFilter() {
	proxy_model->invalidate();
	table_batch->setModel(proxy_model);
}

void TabBatchWidget::setFilterString(const QString& string) {
	QString f = string;
	f.replace(' ', '*');
// 	proxy_model->setFilterRegExp(QRegExp(f, Qt::CaseInsensitive, QRegExp::FixedString));
	proxy_model->setFilterWildcard(f);
	proxy_model->setFilterKeyColumn(BatchTableModel::HSpec);
}

// void TabBatchWidget::syncDB() {
// 	Database::Instance().updateBatchQty();
// 	Database::Instance().updateMealCosts();
// }

void TabBatchWidget::doCreateSMreports() {
	DBReports::printSMReport();
}

void TabBatchWidget::doCreateKMreports() {
	DBReports::printKMReport();
}

#include "TabBatchWidget.moc"
