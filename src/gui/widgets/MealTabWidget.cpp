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
// 	fakeIcon = style()->standardIcon(QStyle::SP_DirClosedIcon);
// 	fakeLabel = tr("Add meal to activate tab");
// 	fakeWidget = new QWidget(this);
// 
// 	addTab(fakeWidget, fakeIcon, fakeLabel);
// 
// 	this->setTabsClosable(true);
// 
// 	aBreakfast = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Breakfast"), this);
// 	a2ndBreakfast = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("2nd Breakfast"), this);
// 	aLunch = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Lunch"), this);
// 	aDiner = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Diner"), this);
// 	aOther1 = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Other 1"), this);
// 	aOther2 = new QAction(style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Other 2"), this);

// 	connect(aBreakfast, SIGNAL(triggered(bool)), this, SLOT(insertNewTab()));
// 	connect(a2ndBreakfast, SIGNAL(triggered(bool)), this, SLOT(insertNewTab()));
// 	connect(aLunch, SIGNAL(triggered(bool)), this, SLOT(insertNewTab()));
// 	connect(aDiner, SIGNAL(triggered(bool)), this, SLOT(insertNewTab()));
// 	connect(aOther1, SIGNAL(triggered(bool)), this, SLOT(insertNewTab()));
// 	connect(aOther2, SIGNAL(triggered(bool)), this, SLOT(insertNewTab()));
// 
// 	b = new QToolButton(this);
// 	b->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
// 	b->setPopupMode(QToolButton::InstantPopup);
// 	b->addAction(aBreakfast);
// 	b->addAction(a2ndBreakfast);
// 	b->addAction(aLunch);
// 	b->addAction(aDiner);
// 	b->addAction(aOther1);
// 	b->addAction(aOther2);
// 	this->setCornerWidget(b, Qt::TopRightCorner);
// 
// 	PR(aBreakfast);
// 	PR(a2ndBreakfast);
// 	PR(aLunch);
// 	PR(aDiner);
// 	PR(aOther1);
// 	PR(aOther2);
	this->setDocumentMode(true);
// 	connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(removeNewTab(int)));
// 
// 	msgb.setText(tr("Remove meal from database"));
// 	msgb.setInformativeText(tr("Ary you sure you want delete meal from databas"));
// 	msgb.setIcon(QMessageBox::Question);
// 	msgb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
// 	msgb.setDefaultButton(QMessageBox::No);

	for (int i = 0; i < meals; ++i)
		foodlist[i] = new MealFoodList(this);

	this->addTab(foodlist[0], tr("Breakfast"));
	this->addTab(foodlist[1], tr("2nd Breakfast"));
	this->addTab(foodlist[2], tr("Lunch"));
	this->addTab(foodlist[3], tr("Tea"));
	this->addTab(foodlist[4], tr("Diner"));
	this->addTab(foodlist[5], tr("Other 1"));
	this->addTab(foodlist[6], tr("Other 2"));

	for (int i = 0; i < meals; ++i) {
		proxy[i] = new MealTableModelProxy();
		proxy[i]->setSourceModel((QAbstractItemModel *)Database::Instance().CachedDistributor());
	}
}

MealTabWidget::~MealTabWidget() {
	FPR(__func__);
// 	if (b) delete b;
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


// void MealTabWidget::tabInserted(int index) {
// 	int num = count();
// 	if (num > 1 && this->widget(num-1) == fakeWidget) {
// 		this->removeTab(num-1);
// 	}
// 
//     QTabWidget::tabInserted(index);
// }
// 
// void MealTabWidget::tabRemoved(int index) {
// 	if (count() == 0) {
// 		this->addTab(fakeWidget, fakeIcon, fakeLabel);
// 	}
// 
//     QTabWidget::tabRemoved(index);
// }
// 
// void MealTabWidget::insertNewTab() {
// 	PR(this->sender());
// 	this->insertTab(count()-1, new MealFoodList(), "asdas");
// }
// 
// void MealTabWidget::removeNewTab(int index) {
// 	if (this->widget(index) != fakeWidget) {
// 		if (msgb.exec() == QMessageBox::Yes)
// 			this->removeTab(index);
// 	}
// }

#include "MealTabWidget.moc"