/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

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

#include "iostream"

#include "time.h"

#include "globals.h"

#include "MealFoodListItemDataWidget.h"
#include "MealFoodList.h"
#include "MealTabWidget.h"

#include "Database.h"

#include <QStringBuilder>

MealFoodListItemDataWidget::MealFoodListItemDataWidget(QWidget* parent, QListWidgetItem * item, Qt::WindowFlags f):
		QWidget(parent, f), lock(true), owner(item) {
	mfl = (MealFoodList *)parent;
	setupUi(this);
	btmp = ((MealTabWidget *)(mfl->parent()->parent()))->getBatchProxyModel();

// 	batch->setEditable(true);
	batch->setInsertPolicy(QComboBox::NoInsert);
	batch->setModel(btmp);

	batch->setModelColumn(2);
	batch->setAutoCompletion(true);
	batch->setAutoCompletionCaseSensitivity(Qt::CaseInsensitive);

	addB->setMaximumSize(24, 24);
	addB->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	updateB->setMaximumSize(24, 24);
	updateB->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	removeB->setMaximumSize(24, 24);
	removeB->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
	closeB->setMaximumSize(24, 24);
	closeB->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));

	qty->setMaximumSize(128, 24);
	qty->setMaximum(9999);
	qty_label->setMaximumSize(128, 24);

	proxyindex = -1;
	btmp->setItemNum(&proxyindex);
// 	connect(addB, SIGNAL(clicked(bool)), this, SLOT(printStatus()));

	connect(batch, SIGNAL(currentIndexChanged(int)), this, SLOT(validateBatchAdd()));
// 	connect(batch, SIGNAL(editTextChanged(QString)), this, SLOT(validateBatchAdd()));
	connect(qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));

	connect(addB, SIGNAL(clicked(bool)), this, SLOT(buttonAdd()));
	connect(updateB, SIGNAL(clicked(bool)), this, SLOT(buttonUpdate()));
	connect(removeB, SIGNAL(clicked(bool)), this, SLOT(buttonRemove()));
	connect(closeB, SIGNAL(clicked(bool)), mfl->parent()->parent(), SLOT(closeOpenedItems()));

	convertToEmpty();
	validateBatchAdd();
}

MealFoodListItemDataWidget::~MealFoodListItemDataWidget() {
// 	PR(__func__);
}

/** @brief Render widget
 * @param doRender if thre then render 'readable' version of widget, otherwise render 'editable' version
 */
void MealFoodListItemDataWidget::render(bool doRender) {
	addB->setVisible(!doRender);
	updateB->setVisible(doRender);
	removeB->setVisible(!empty);
	closeB->setVisible(!doRender and !empty);

	qty->setVisible(!doRender);
	qty_label->setVisible(doRender);

	batch->setVisible(!doRender);
	batch_label->setVisible(doRender);
}

	
void MealFoodListItemDataWidget::validateBatchAdd() {
	if (!lock)
		return;

	double free = btmp->index(batch->currentIndex(), BatchTableModel::HUsedQty).data(BatchTableModel::RFreeQty).toDouble();

	if (batch->currentIndex() >= 0) {
		if (batch->currentIndex() != batch_idx.row()) {
			qty->setMaximum(free);
			qty->setSuffix(tr(" of %1").arg(free));
		} else {
			qty->setMaximum(free + quantity);
			qty->setSuffix(tr(" of %1").arg(free + quantity));
		}
	} else {
		qty->setMaximum(9999);
	}

	validateAdd();
}

void MealFoodListItemDataWidget::validateAdd() {
	if (qty->value() && batch->currentIndex() >= 0) {
		addB->setEnabled(true);
	} else {
		addB->setEnabled(false);
	}
}

void MealFoodListItemDataWidget::convertToEmpty() {
	batch->setCurrentIndex(-1);

	qty->setValue(0.0);
	empty = true;
	render(false);
}

void MealFoodListItemDataWidget::buttonAdd() {
	Database & db = Database::Instance();
	lock = false;

	batch_idx = btmp->mapToSource(btmp->index(batch->currentIndex(), BatchTableModel::HId));
	quantity = qty->value();
	batchlabel = db.CachedBatch()->index(batch_idx.row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString();

// 	QModelIndexList mdl =  db.CachedMealDay()->match(db.CachedMealDay()->index(0, MealDayTableModel::HId), Qt::EditRole, mfl->proxyModel()->key(), 1);
// 	QDate d = db.CachedMealDay()->index(mdl.at(0).row(), MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();

	QDate d = mfl->proxyModel()->refDate();

	if (empty) {
		if (db.addDistributorRecord(batch_idx.data().toInt(), quantity*100, d, d,
			DistributorTableModel::RMeal, QString("%1").arg(mfl->proxyModel()->key()), "")) {
			empty = false;
			mfl->insertEmptySlot();
			dist_idx = db.CachedDistributor()->index(db.CachedDistributor()->rowCount()-1, DistributorTableModel::HId);
		} else
			return;
	} else {
		if (db.updateDistributorRecord(dist_idx, batch_idx.data().toUInt(), quantity*100, d, d,
			DistributorTableModel::RMeal, QString("%1").arg(mfl->proxyModel()->key()), "")) {
			empty = false;
		} else
			return;
	}

	batch_label->setText(batchlabel);
	qty_label->setText(QString("%1").arg(quantity));

	mfl->markDirty();
	render(true);
	db.CachedMealDay()->select();
}

/** @brief Prepare widget to update data.
 */
void MealFoodListItemDataWidget::buttonUpdate() {
	if (lock)
		return;

	((MealTabWidget *)(mfl->parent()->parent()))->closeOpenedItems();
	((MealTabWidget *)(mfl->parent()->parent()))->markOpenedItems(owner);
	lock = true;
// 	proxyindex = btmp->mapToSource(btmp->index(batch->currentIndex(), BatchTableModel::HId)).row();
	proxyindex = batch_idx.row();
	btmp->setItemNum(&proxyindex);
	btmp->invalidate();
	batch->setCurrentIndex(btmp->mapFromSource(batch_idx).row());

	double free = btmp->data(btmp->index(batch->currentIndex(), BatchTableModel::HUsedQty), BatchTableModel::RFreeQty).toDouble();
	qty->setMaximum(free+quantity);
	qty->setSuffix(tr(" of %1").arg(free + quantity));
	qty->setValue(quantity);

	render(false);
}

/** @brief Close widget and display data
 */
void MealFoodListItemDataWidget::buttonClose() {
	lock = false;
	render(true);
}

void MealFoodListItemDataWidget::buttonRemove() { EGTD
	Database & db = Database::Instance();
GTM
	db.removeDistributorRecord(dist_idx.row());
GTM
	mfl->markDirty();
GTM
	convertToEmpty();
GTM
	mfl->populateModel();
	db.CachedMealDay()->select();
GTM
}

/** @brief Prepare data for widget
 * @param idx index of batch
 */
void MealFoodListItemDataWidget::setBatchData(const QModelIndex & idx) {
	dist_idx = idx;

	BatchTableModel * btm = Database::Instance().CachedBatch();
	DistributorTableModel * dtm = Database::Instance().CachedDistributor();

	QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, idx.data(DistributorTableModel::RRaw));
	if (qmil.count()) {
		batch_idx = qmil.at(0);

		quantity = dtm->data(dtm->index(dist_idx.row(), DistributorTableModel::HQty), Qt::DisplayRole).toDouble();
		batchlabel = Database::Instance().CachedBatch()->index(batch_idx.row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString();

		batch_label->setText(batchlabel);
		qty_label->setText(QString("%1").arg(quantity));

		empty = false;
		render(true);
		lock = false;
	}
}

bool MealFoodListItemDataWidget::isEmpty() {
	return empty;
}

#include "MealFoodListItemDataWidget.moc"
