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

#include "globals.h"

#include "MealFoodListItemDataWidget.h"
#include "MealFoodList.h"

#include "Database.h"

MealFoodListItemDataWidget::MealFoodListItemDataWidget(QWidget* parent, Qt::WindowFlags f):
		QWidget(parent, f), lock(true) {

	setupUi(this);

	che = new QCheckBox();
	che->setChecked(true);

	btmp = new BatchTableModelProxy(che);
	btmp->setDynamicSortFilter(true);

	btmp->setSourceModel(Database::Instance().CachedBatch());

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

	qty->setMaximumSize(128, 24);
	qty->setMaximum(9999);
	qty_label->setMaximumSize(128, 24);

// 	connect(addB, SIGNAL(clicked(bool)), this, SLOT(printStatus()));

	mfl = (MealFoodList *)parent;

	connect(batch, SIGNAL(currentIndexChanged(int)), this, SLOT(validateBatchAdd()));
// 	connect(batch, SIGNAL(editTextChanged(QString)), this, SLOT(validateBatchAdd()));
	connect(qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));

	connect(addB, SIGNAL(clicked(bool)), this, SLOT(buttonAdd()));
	connect(updateB, SIGNAL(clicked(bool)), this, SLOT(buttonUpdate()));
	connect(removeB, SIGNAL(clicked(bool)), this, SLOT(buttonRemove()));

	convertToEmpty();
	validateBatchAdd();
}

MealFoodListItemDataWidget::~MealFoodListItemDataWidget() {
	delete btmp;
	delete che;
// 	PR(__func__);
}

void MealFoodListItemDataWidget::render(bool doRender) {
	addB->setVisible(!doRender);
	updateB->setVisible(doRender);
	removeB->setVisible(!empty);

	qty->setVisible(!doRender);
	qty_label->setVisible(doRender);
// 	qty_label->setVisible(false);
// 	qty->setReadOnly(doRender);
// 	qty->setFrame(!doRender);
// 	qty->setButtonSymbols((doRender ? QAbstractSpinBox::NoButtons : QAbstractSpinBox::UpDownArrows));

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

	int old_bid = batch_idx.data().toInt();

	batch_idx = db.CachedBatch()->index(batch->currentIndex(), BatchTableModel::HId);
	quantity = qty->value();
	batchlabel = Database::Instance().CachedBatch()->index(batch_idx.row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString();

	if (empty) {
		if (db.addDistributorRecord(batch_idx.data().toInt(), quantity,
				mfl->proxyModel()->ref(), mfl->proxyModel()->ref(),
				QString("%1").arg(mfl->proxyModel()->key()), "", DistributorTableModel::RMeal)) {
			empty = false;
			mfl->insertEmptySlot();
			dist_idx = db.CachedDistributor()->index(db.CachedDistributor()->rowCount()-1, DistributorTableModel::HId);
		} else
			return;
	} else {
		if (db.updateDistributorRecord(dist_idx.row(), batch_idx.data().toInt(), quantity,
				mfl->proxyModel()->ref(), mfl->proxyModel()->ref(),
				QString("%1").arg(mfl->proxyModel()->key()), "", DistributorTableModel::RMeal)) {
			empty = false;
		} else
			return;
	}
	if (old_bid != batch_idx.data().toInt())
		db.updateBatchQty(old_bid);
	db.updateBatchQty(batch_idx.data().toInt());

	batch_label->setText(batchlabel);
	qty_label->setText(QString("%1").arg(quantity));

	mfl->markDirty();
	render(true);
}

void MealFoodListItemDataWidget::buttonUpdate() {
	if (lock)
		return;

	lock = true;

	batch->setCurrentIndex(batch_idx.row());
	qty->setValue(quantity);


	render(false);
}

void MealFoodListItemDataWidget::buttonRemove() {
	Database & db = Database::Instance();

	db.removeDistributorRecord(dist_idx.row());
	mfl->markDirty();

	convertToEmpty();
	mfl->populateModel();
}

void MealFoodListItemDataWidget::setBatchData(const QModelIndex & idx) {
	dist_idx = idx;

	BatchTableModel * btm = Database::Instance().CachedBatch();
	DistributorTableModel * dtm = Database::Instance().CachedDistributor();

	QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, idx.data(Qt::EditRole));
	if (qmil.count()) {
		batch_idx = qmil.at(0);

		quantity = dtm->data(dtm->index(dist_idx.row(), DistributorTableModel::HQty)).toDouble();
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
