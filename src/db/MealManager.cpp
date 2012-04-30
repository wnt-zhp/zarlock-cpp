/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#include "MealManager.h"

/**
 * @brief Instancja klasy MealManager
 **/
MealManager * MealManager::manager = NULL;

/**
 * @brief Tworzy i zwraca instację do obiektu MealManager
 *
 * @return MealManager &
 **/
MealManager * MealManager::Instance() {
	if (!manager) {
		manager = new MealManager;
		FI();
		std::cout << "++ Create MealManager instance\n";
	}
	
	return manager;
}

void MealManager::Destroy() {
	if (manager) {
		delete manager; manager = NULL;
		std::cout << "++ Destroy MealManager instance\n";
	}
}

MealManager::MealManager() : QObject(), db(Database::Instance())
{
	defMealsNames = {
		QObject::tr("Breakfast"), QObject::tr("Brunch"),
		QObject::tr("Lunch"), QObject::tr("Tea"),
		QObject::tr("Diner"), QObject::tr("Other")
	};

	model_mealday = db->CachedMealDay();
	model_meal = db->CachedMeal();
	CI();
}

MealManager::MealManager(const MealManager & /*db*/) : QObject() {
	CI();
}

MealManager::~MealManager() {
	DI();
}

int MealManager::insertMeal(int mealdayid, MealManager::MealKind mk) {
	return insertMeal(mealdayid, mk, defMealsNames[mk]);
}

int MealManager::insertMeal(int mealdayid, MealManager::MealKind mk, const QString& name) throw (int) {PR(mealdayid);
	if (addMealRecord(mealdayid, mk, name, 0, 0, 0, 0, "")) {

		int mid = model_meal->index(model_meal->rowCount()-1, MealTableModel::HId).data(Qt::EditRole).toInt();

		emit mealInserted(mealdayid);

		return mid;
	} else {
		throw mealdayid*100 + mk;
	}
}



/** @brief Insert MealDay record to database
 * @param date meal day
 **/
bool MealManager::addMealDayRecord(const QDate & mealday, int avgcost) {
	bool status = true;
	
	int row = model_mealday->rowCount();
	PR(row);
	model_mealday->autoSubmit(false);
	status &= model_mealday->insertRows(row, 1);
	status &= model_mealday->setData(model_mealday->index(row, MealDayTableModel::HMealDate), mealday.toString(Qt::ISODate));
	status &= model_mealday->setData(model_mealday->index(row, MealDayTableModel::HAvgCost), avgcost);
	model_mealday->autoSubmit(true);

	if (!status) {
		model_mealday->revertAll();
		return false;
	} else
		return model_mealday->submitAll();
}

bool MealManager::updateMealDayRecord(const int row, const QDate & mealday, int avgcost) {
	bool status = true;
	
	// 	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HId), row);
	status &= model_meal->setData(model_mealday->index(row, MealDayTableModel::HMealDate), mealday.toString(Qt::ISODate));
	status &= model_meal->setData(model_mealday->index(row, MealDayTableModel::HAvgCost), avgcost);	
	
	if (!status) {
		model_meal->revertAll();
		return false;
	}
	
	status = model_meal->submitAll();
	return status;
}

bool MealManager::removeMealDayRecord(QVector<int> & ids, bool askForConfirmation) {
	bool status = false;
	QString details;
	
	qSort(ids);
	
	int rowcount = ids.count();
	
	if (askForConfirmation) {
		for (int i = 0; i < rowcount; ++i) {
			// 			QVariant mdid = model_mealday->index(idxl.at(i).row(), MealDayTableModel::HId).data();
			// 			QString mdday = model_mealday->index(idxl.at(i).row(), MealDayTableModel::HMealDate).data().toDate().toString(Qt::DefaultLocaleShortDate);
			// 			++counter;
			// 			details = details % mdday % "\n";
			// 			QModelIndexList mkinds = model_mealkind->match(model_mealkind->index(0, MealKindTableModel::HMealDate), Qt::EditRole, mdid, -1);
			// 			for (int i = 0; i < mkinds.count(); ++i) {
				// 				details = details % "  \\--  " % model_batch->data(model_batch->index(mkinds.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
				// 			}
				// 				bat.append(batches);
		}
		status = model_mealday->mealdayRemoveConfirmation(rowcount, details);
	}
	
	if (askForConfirmation & !status )
		return false;
	
	// rows must be sorted!
		for (int i = rowcount-1; i >= 0; --i) {
			int midr = model_mealday->match(model_mealday->index(0, MealDayTableModel::HId), Qt::EditRole, ids.at(i)).at(0).row();
			
			if (!model_meal->removeRow(midr)) {
				model_meal->revertAll();
				return false;
			}
		}
		
		status = model_meal->submitAll();
		
		status = model_mealday->submitAll();
		return status;
}

bool MealManager::getMealDayRecord(const int row, int & mdid, QDate & mealday, int & avgcost) {
	mdid		= model_mealday->index(row, MealDayTableModel::HId).data().toUInt();
	mealday		= model_mealday->index(row, MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();
	avgcost		= model_mealday->index(row, MealDayTableModel::HAvgCost).data(Qt::EditRole).toUInt();
	
	return true;
}

bool MealManager::addMealRecord(int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes) {
	bool status = true;
	
	int row = model_meal->rowCount();
	
	model_meal->autoSubmit(false);
	status &= model_meal->insertRows(row, 1);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealDay), mealday);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealKind), mealkind);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealName), name);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HScouts), scouts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HLeaders), leaders);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HOthers), others);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HAvgCosts), avgcosts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HNotes), notes);
	model_meal->autoSubmit(true);
	
	if (!status) {
		model_meal->revertAll();
	} else
		return model_meal->submitAll();

	return false;
}

bool MealManager::updateMealRecord(int mid, int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int /*avgcosts*/, const QString & notes) {
	bool status = true;
	
	int row = mid;
	model_meal->autoSubmit(false);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealDay), mealday);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealKind), mealkind);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealName), name);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HScouts), scouts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HLeaders), leaders);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HOthers), others);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HNotes), notes);
	model_meal->autoSubmit(true);
	
	if (!status) {
		model_meal->revertAll();
		return false;
	}
	
	return model_meal->submitAll();
}

bool MealManager::removeMealRecord(const QVector<int> & ids) {
	bool status = false;
	QString details;
	
	int rowcount = ids.count();
	
	for (int i = rowcount-1; i >= 0; --i) {
		int midr = model_meal->match(model_meal->index(0, MealTableModel::HId), Qt::EditRole, ids.at(i)).at(0).row();

		if (!model_meal->removeRow(midr)) {
			model_meal->revertAll();
			return false;
		}
	}

	status = model_meal->submitAll();

	return status;
}

void MealManager::selectMealDay() {
	model_mealday->select();
	while (model_mealday->canFetchMore())
		model_mealday->fetchMore();
}

void MealManager::selectMeal() {
	model_meal->select();
	while (model_meal->canFetchMore())
		model_meal->fetchMore();
}

QVariant MealManager::getMealDayDate(int id) {
	QModelIndexList l = model_mealday->match(model_mealday->index(0, MealDayTableModel::HId), Qt::EditRole, id, -1, Qt::MatchExactly);
	if (l.size() == 1)
		return model_mealday->index(l.at(0).row(), MealDayTableModel::HMealDate).data(Qt::EditRole);
	return QVariant();
}

QVariant MealManager::getMealDayId(const QDate& date) {
	QModelIndexList l = model_mealday->match(model_mealday->index(0, MealDayTableModel::HMealDate), Qt::EditRole, date, -1, Qt::MatchExactly);
	if (l.size() == 1)
		return model_mealday->index(l.at(0).row(), MealDayTableModel::HId).data(Qt::EditRole);
	return QVariant();
}

void MealManager::mealDayDefauls(int mealDayId) {
	bool is_break = false;
	bool is_lunch = false;
	bool is_tea = false;
	bool is_diner = false;

	QModelIndexList ml = model_meal->match(model_meal->index(0, MealTableModel::HMealDay), Qt::EditRole, mealDayId, -1, Qt::MatchExactly);

	for (int i = 0; i < ml.size(); ++i) {
		int mealkind = model_meal->index(ml.at(i).row(), MealTableModel::HMealKind).data(Qt::EditRole).toInt();
		switch (mealkind) {
			case MealManager::MBreakfast:
				is_break = true;
				break;
			case MealManager::MLunch:
				is_lunch = true;
				break;
			case MealManager::MTea:
				is_tea = true;
				break;
			case MealManager::MDiner:
				is_diner = true;
				break;
			default:
				break;
		}
	}

	if (!is_break)		insertMeal(mealDayId, MBreakfast);
	if (!is_lunch)		insertMeal(mealDayId, MLunch);
	if (!is_tea)		insertMeal(mealDayId, MTea);
	if (!is_diner)		insertMeal(mealDayId, MDiner);
}

#include "MealManager.moc"