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
		QWidget(parent, f), doRender(true), isEmpty(true) {
	setupUi(this);
	batch->setModel(Database::Instance().CachedBatch());
	batch->setModelColumn(2);
	batch->setCurrentIndex(-1);

	addB->setMaximumSize(24, 24);
	addB->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	updateB->setMaximumSize(24, 24);
	updateB->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	removeB->setMaximumSize(24, 24);
	removeB->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));

	qty->setMaximumSize(128, 24);
	qty_label->setMaximumSize(128, 24);

// 	connect(addB, SIGNAL(clicked(bool)), this, SLOT(printStatus()));

	connect(batch, SIGNAL(currentIndexChanged(int)), this, SLOT(validateAdd()));
	connect(qty, SIGNAL(valueChanged(double)), this, SLOT(validateAdd()));

	connect(addB, SIGNAL(clicked(bool)), this, SLOT(updateDistributor()));
	connect(addB, SIGNAL(clicked(bool)), this, SLOT(updateDistributor()));
	connect(addB, SIGNAL(clicked(bool)), this, SLOT(updateDistributor()));

	convertToData();
	validateAdd();

	mfl = (MealFoodList *)parent;
}

MealFoodListItemDataWidget::~MealFoodListItemDataWidget() {
// 	PR(__func__);
}

void MealFoodListItemDataWidget::convertToData() {
	if (!doRender) {
		addB->setVisible(false);
		updateB->setVisible(true);
		removeB->setVisible(true);

		qty->setVisible(false);
		qty_label->setVisible(true);
		batch->setVisible(false);
		batch_label->setVisible(true);
		doRender = true;
	} else {
		addB->setVisible(true);
		updateB->setVisible(false);
		removeB->setVisible(false);

		batch_label->setText(batch->currentText());
		qty_label->setText(qty->text());
		qty->setVisible(true);
		qty_label->setVisible(false);
		batch->setVisible(true);
		batch_label->setVisible(false);
		doRender = false;
	}
}

void MealFoodListItemDataWidget::validateAdd() {
	if (qty->value() && batch->currentIndex() >= 0) {
		addB->setEnabled(true);
	} else {
		addB->setEnabled(false);
	}
}

void MealFoodListItemDataWidget::updateDistributor() {
	Database & db = Database::Instance();
	PR(sender());
	if (sender() == addB) {
		if (isEmpty) {
			int batch_id = db.CachedBatch()->index(batch->currentIndex(), BatchTableModel::HId).data().toInt();

			db.addDistributorRecord(batch_id, qty->text(), mfl->proxyModel()->ref(),
							mfl->proxyModel()->ref(), QString("%1").arg(mfl->proxyModel()->key()),
							"", DistributorTableModel::RMeal);
		} else {
			
		}
	}
	if (sender() == updateB) {
	}
	if (sender() == removeB) {
	}

	convertToData();
}

void MealFoodListItemDataWidget::printStatus() {
	int curIdx = batch->currentIndex();
	PR(curIdx);
	PR(batch->model()->data(batch->model()->index(curIdx, BatchTableModel::HId)).toString().toStdString());
	PR(batch->model()->data(batch->model()->index(curIdx, BatchTableModel::HSpec)).toString().toStdString());
}

void MealFoodListItemDataWidget::setBatchData(const QModelIndex & didx) {
	BatchTableModel * btm = Database::Instance().CachedBatch();
	DistributorTableModel * dtm = Database::Instance().CachedDistributor();
	idx = didx;

	QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, didx.data(Qt::EditRole));
	if (qmil.count()) {
		batch->setCurrentIndex(qmil.at(0).row());
		qty->setValue( dtm->data(dtm->index(didx.row(), DistributorTableModel::HQty)).toDouble());
		isEmpty = false;
		doRender = true;
		convertToData();
	}
}

#include "MealFoodListItemDataWidget.moc"