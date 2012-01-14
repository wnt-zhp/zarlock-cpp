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

#include "MealTableModelProxy.h"
#include "BatchTableModel.h"
#include "DistributorTableModel.h"

#include <QDate>

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
MealTableModelProxy::MealTableModelProxy(QObject * parent) : QSortFilterProxyModel(parent),
															 mealkey(0), ref_date(0,0,0) {
}

/**
 * @brief Luck, I'm your father!
 *
 **/
MealTableModelProxy::~MealTableModelProxy() {
	FPR(__func__);
}


bool MealTableModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	int rmeal = sourceModel()->index(sourceRow, DistributorTableModel::HDistType, sourceParent).data().toInt();
	int mdid = sourceModel()->index(sourceRow, DistributorTableModel::HDistTypeA, sourceParent).data().toInt();

// 	QString mdate = sourceModel()->index(sourceRow, DistributorTableModel::HDistTypeB, sourceParent).data(DistributorTableModel::RRaw).toString();

	if (rmeal == DistributorTableModel::RMeal and mdid == mealkey/* and mdate == dateref.toString(Qt::ISODate)*/)
		return true;

	return false;
}

/** @brief Set proxy key
 * @param key meal day id
 */
void MealTableModelProxy::setKey(int key) {
	mealkey = key;
}

/** Get proxy key
 * @return key value
 */
int MealTableModelProxy::key() const {
	return mealkey;
}

/** @brief Set proxy key
 * @param key meal day id
 */
void MealTableModelProxy::setRefDate(const QDate & date) {
	ref_date = date;
}

/** Get proxy key
 * @return key value
 */
const QDate & MealTableModelProxy::refDate() const {
	return ref_date;
}

#include "MealTableModelProxy.moc"