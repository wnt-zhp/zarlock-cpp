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
#include "MealTabInsertWidget.h"
#include "Database.h"
#include "DataParser.h"
#include "DBReports.h"

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

	validateAdd();
}

MealTabInsertWidget::~MealTabInsertWidget() {
	FPR(__func__);
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
		QModelIndexList ml  = mm ->match(mm->index(0, MealTableModel::HMealDay), Qt::EditRole, mdid, -1);

		for (int i = 0; i < ml.size(); ++i) {
			int mealkind = mm->index(ml.at(i).row(), MealTableModel::HMealKind).data(Qt::EditRole).toInt();
			switch (mealkind) {
				case MealTableModel::MBreakfast:
					push_br->setEnabled(false);
					break;
				case MealTableModel::MBreakfast2:
					push_br2->setEnabled(false);
					break;
				case MealTableModel::MLunch:
					push_lunch->setEnabled(false);
					break;
				case MealTableModel::MTea:
					push_tea->setEnabled(false);
					break;
				case MealTableModel::MDiner:
					push_dinner->setEnabled(false);
					break;
				case MealTableModel::MOther:
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
		push_br->setEnabled(true);
	}
}

void MealTabInsertWidget::pushButton() {
	Database * db = Database::Instance();

	int mealkind = -1;
	if (sender() == push_br) {
		db->addMealRecord(mdid, MealTableModel::MBreakfast, tr("Breakfast"), 0, 0, 0, 0, "");
		mealkind = MealTableModel::MBreakfast;
	}
	else
	if (sender() == push_br2) {
		db->addMealRecord(mdid, MealTableModel::MBreakfast2, tr("2nd breakfast"), 0, 0, 0, 0, "");
		mealkind = MealTableModel::MBreakfast2;
	}
	else
	if (sender() == push_lunch) {
		db->addMealRecord(mdid, MealTableModel::MLunch, tr("Lunch"), 0, 0, 0, 0, "");
		mealkind = MealTableModel::MLunch;
	}
	else
	if (sender() == push_tea) {
		db->addMealRecord(mdid, MealTableModel::MTea, tr("Tea"), 0, 0, 0, 0, "");
		mealkind = MealTableModel::MTea;
	}
	else
	if (sender() == push_dinner) {
		db->addMealRecord(mdid, MealTableModel::MDiner, tr("Diner"), 0, 0, 0, 0, "");
		mealkind = MealTableModel::MDiner;
	}
	else
	if (sender() == push_other) {
		db->addMealRecord(mdid, MealTableModel::MOther, line_other->text(), 0, 0, 0, 0, "");
		mealkind = MealTableModel::MOther;
	}

	db->CachedMeal()->select();

	emit mealInserted(mdid);
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
