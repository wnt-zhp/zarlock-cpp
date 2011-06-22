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
		QWidget(parent, f) {

	setupUi(this);

// 	batch->setEditable(true);
	batch->setInsertPolicy(QComboBox::NoInsert);
	batch->setModel(Database::Instance().CachedBatch());
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
	qty_label->setMaximumSize(128, 24);

// 	connect(addB, SIGNAL(clicked(bool)), this, SLOT(printStatus()));

	mfl = (MealFoodList *)parent;

	connect(batch, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
// 	connect(batch, SIGNAL(editTextChanged(QString)), this, SLOT(validateAdd()));
	connect(qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));

	connect(addB, SIGNAL(clicked(bool)), this, SLOT(buttonAdd()));
	connect(updateB, SIGNAL(clicked(bool)), this, SLOT(buttonUpdate()));
	connect(removeB, SIGNAL(clicked(bool)), this, SLOT(buttonRemove()));

	connect(this, SIGNAL(removeRecord(int)), mfl, SLOT(removeItem(int)));

	convertToEmpty();
}

MealFoodListItemDataWidget::~MealFoodListItemDataWidget() {
// 	PR(__func__);
}

void MealFoodListItemDataWidget::render(bool doRender) {
	if (doRender) {
		batch_label->setText(batch->currentText());
		qty_label->setText(qty->text());
	} else {
	}
	addB->setVisible(!doRender);
	updateB->setVisible(doRender);
	removeB->setVisible(!empty);

	qty->setVisible(!doRender);
	qty_label->setVisible(doRender);
	batch->setVisible(!doRender);
	batch_label->setVisible(doRender);

}

void MealFoodListItemDataWidget::validateAdd() {
// 	PR(batch->currentIndex());
	if (qty->value() && batch->currentIndex() >= 0) {
		addB->setEnabled(true);
	} else {
		addB->setEnabled(false);
	}
}

void MealFoodListItemDataWidget::convertToEmpty() {
// 	if (empty)
	batch->setModelColumn(2);
	batch->setCurrentIndex(-1);
		
	qty->setValue(0.0);
	empty = true;
	render(false);
}

void MealFoodListItemDataWidget::buttonAdd() {
	Database & db = Database::Instance();

	int batch_id = db.CachedBatch()->index(batch->currentIndex(), BatchTableModel::HId).data().toInt();
	if (empty) {
		if (db.addDistributorRecord(batch_id, qty->text().toFloat(),
								mfl->proxyModel()->ref(),
								mfl->proxyModel()->ref(),
								QString("%1").arg(mfl->proxyModel()->key()), "", DistributorTableModel::RMeal)) {
			empty = false;
// 			mfl->populateModel();
			mfl->insertEmptySlot();
		} else
			return;
	} else {
		if (db.updateDistributorRecord(idx.row(), batch_id, qty->text().toFloat(),
								mfl->proxyModel()->ref(),
								mfl->proxyModel()->ref(),
								QString("%1").arg(mfl->proxyModel()->key()), "", DistributorTableModel::RMeal)) {
			empty = false;
		} else
			return;
	}
	render(true);
}

void MealFoodListItemDataWidget::buttonUpdate() {
	render(false);
}

void MealFoodListItemDataWidget::buttonRemove() {
	Database & db = Database::Instance();

	db.removeDistributorRecord(idx.row());
	convertToEmpty();
	mfl->populateModel();
// 	emit removeRecord(idx.row());
// 	PR(removeB);

// 	qlwi->listWidget()->takeItem(qlwi->listWidget()->row(qlwi));
}
void MealFoodListItemDataWidget::printStatus() {
	int curIdx = batch->currentIndex();
	PR(curIdx);
	PR(batch->model()->data(batch->model()->index(curIdx, BatchTableModel::HId)).toString().toStdString());
	PR(batch->model()->data(batch->model()->index(curIdx, BatchTableModel::HSpec)).toString().toStdString());
}

void MealFoodListItemDataWidget::setBatchData(const QModelIndex & didx) {
	if (idx != didx)
		idx = didx;

	BatchTableModel * btm = Database::Instance().CachedBatch();
	DistributorTableModel * dtm = Database::Instance().CachedDistributor();

	QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, didx.data(Qt::EditRole));
	if (qmil.count()) {
		batch->setCurrentIndex(qmil.at(0).row());
		qty->setValue(dtm->data(dtm->index(didx.row(), DistributorTableModel::HQty)).toDouble());
		empty = false;
		render(true);
	}
}

bool MealFoodListItemDataWidget::isEmpty() {
	return empty;
}

#include "MealFoodListItemDataWidget.moc"