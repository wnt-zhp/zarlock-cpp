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

#include "DistributorTableModelProxy.h"
#include "DistributorTableModel.h"

#include <QDate>

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
DistributorTableModelProxy::DistributorTableModelProxy(const QCheckBox * meal, QObject * parent) :
												QSortFilterProxyModel(parent),
												cb_meal(meal) {
	CI();
}

/**
 * @brief Luck, I'm your father!
 *
 **/
DistributorTableModelProxy::~DistributorTableModelProxy() {
	DI();
}

bool DistributorTableModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	double disttype = sourceModel()->index(sourceRow, DistributorTableModel::HDistType).data(Qt::EditRole).toInt();	// quantity of free batches

	if (cb_meal and cb_meal->isChecked() and disttype == DistributorTableModel::RMeal)		// hide empty
		return false;

	return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}

#include "DistributorTableModelProxy.moc"