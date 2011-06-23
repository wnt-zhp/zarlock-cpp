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

#include "MealTabWidget.h"

#include "DistributorTableModel.h"
#include "Database.h"

#include <QStyle>
#include <QAction>

MealTabWidget::MealTabWidget(QWidget* parent): QTabWidget(parent) {
	this->setDocumentMode(true);

	for (int i = 0; i < meals; ++i)
		foodlist[i] = new MealFoodList(this);

	this->addTab(foodlist[0], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Breakfast"));
	this->addTab(foodlist[1], tr("2nd Breakfast"));
	this->addTab(foodlist[2], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Lunch"));
	this->addTab(foodlist[3], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Tea"));
	this->addTab(foodlist[4], style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Diner"));
	this->addTab(foodlist[5], tr("Other 1"));
	this->addTab(foodlist[6], tr("Other 2"));

	for (int i = 0; i < meals; ++i) {
		proxy[i] = new MealTableModelProxy();
		proxy[i]->setSourceModel((QAbstractItemModel *)Database::Instance().CachedDistributor());
	}
}

MealTabWidget::~MealTabWidget() {
	FPR(__func__);
}

void MealTabWidget::setDateRef(const QString& ref) {
	for (int i = 0; i < meals; ++i) {
		foodlist[i]->setProxyModel(proxy[i]);
		proxy[i]->setRef(ref);
		proxy[i]->setKey(i);
		proxy[i]->invalidate();
		foodlist[i]->populateModel();
	}
}

#include "MealTabWidget.moc"