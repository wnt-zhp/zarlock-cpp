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
#include "MealTabInsertWidget.h"
#include "Database.h"
#include "DataParser.h"
#include "DBReports.h"
#include "MealManager.h"

MealTabInsertWidget::MealTabInsertWidget(QWidget * /*parent*/) : Ui::MealTabInsertWidget(), db(Database::Instance()) {
	setupUi(this);

	mdid = -1;

	connect(push_br, SIGNAL(clicked(bool)), this, SLOT(pushButton()));
	connect(push_br2, SIGNAL(clicked(bool)), this, SLOT(pushButton()));
	connect(push_lunch, SIGNAL(clicked(bool)), this, SLOT(pushButton()));
	connect(push_tea, SIGNAL(clicked(bool)), this, SLOT(pushButton()));
	connect(push_dinner, SIGNAL(clicked(bool)), this, SLOT(pushButton()));
	connect(push_other, SIGNAL(clicked(bool)), this, SLOT(pushButton()));
	connect(line_other, SIGNAL(textChanged(QString)), this, SLOT(validateAdd()));

	push_br->setIcon(QIcon(":/resources/icons/folder-new.png"));
	push_br2->setIcon(QIcon(":/resources/icons/folder-new.png"));
	push_lunch->setIcon(QIcon(":/resources/icons/folder-new.png"));
	push_tea->setIcon(QIcon(":/resources/icons/folder-new.png"));
	push_dinner->setIcon(QIcon(":/resources/icons/folder-new.png"));
	push_other->setIcon(QIcon(":/resources/icons/folder-new.png"));

// 	push_br->setIconSize(QSize(32,32));
// 	push_br2->setIconSize(QSize(32,32));
// 	push_lunch->setIconSize(QSize(32,32));
// 	push_tea->setIconSize(QSize(32,32));
// 	push_dinner->setIconSize(QSize(32,32));
// 	push_other->setIconSize(QSize(32,32));

	push_other->setEnabled(true);

	validateAdd();
}

MealTabInsertWidget::~MealTabInsertWidget() {
	FI();
	activateUi(false);
}

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void MealTabInsertWidget::activateUi(bool activate) {
// 	this->setVisible(activate);
	if (activate) {
		MealTableModel * mm = Database::Instance()->CachedMeal();
		QModelIndexList ml  = mm ->match(mm->index(0, MealTableModel::HMealDay), Qt::EditRole, mdid, -1, Qt::MatchExactly);

		for (int i = 0; i < ml.size(); ++i) {
			int mealkind = mm->index(ml.at(i).row(), MealTableModel::HMealKind).data(Qt::EditRole).toInt();
			switch (mealkind) {
				case MealManager::MBreakfast:
					push_br->setEnabled(false);
					break;
				case MealManager::MBrunch:
					push_br2->setEnabled(false);
					break;
				case MealManager::MLunch:
					push_lunch->setEnabled(false);
					break;
				case MealManager::MTea:
					push_tea->setEnabled(false);
					break;
				case MealManager::MDiner:
					push_dinner->setEnabled(false);
					break;
				case MealManager::MOther:
// 					push_br->setEnabled(false);
					break;
				default:
					break;
			}
		}
	} else {
		push_br->setEnabled(true);
		push_br2->setEnabled(true);
		push_lunch->setEnabled(true);
		push_tea->setEnabled(true);
		push_dinner->setEnabled(true);
		push_other->setEnabled(true);
	}
}

void MealTabInsertWidget::pushButton() {
	MealManager * mm = MealManager::Instance();

	int mealkind = -1;
	if (sender() == push_br) {
		mm->insertMeal(mdid, MealManager::MBreakfast);
		mealkind = MealManager::MBreakfast;
	}
	else
	if (sender() == push_br2) {
		mm->insertMeal(mdid, MealManager::MBrunch);
		mealkind = MealManager::MBrunch;
	}
	else
	if (sender() == push_lunch) {
		mm->insertMeal(mdid, MealManager::MLunch);
		mealkind = MealManager::MLunch;
	}
	else
	if (sender() == push_tea) {
		mm->insertMeal(mdid, MealManager::MTea);
		mealkind = MealManager::MTea;
	}
	else
	if (sender() == push_dinner) {
		mm->insertMeal(mdid, MealManager::MDiner);
		mealkind = MealManager::MDiner;
	}
	else
	if (sender() == push_other) {
		mm->insertMeal(mdid, MealManager::MOther, line_other->text());
		mealkind = MealManager::MOther;
	}

	mm->selectMeal();
}

void MealTabInsertWidget::setKey(int mealdayid) {
	activateUi(false);
	mdid = mealdayid;
	activateUi(true);
}

void MealTabInsertWidget::validateAdd() {
	push_other->setEnabled(!line_other->text().isEmpty());
}

#include "MealTabInsertWidget.moc"
