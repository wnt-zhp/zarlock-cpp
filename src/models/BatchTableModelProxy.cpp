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

#include "BatchTableModelProxy.h"
#include "BatchTableModel.h"

#include <QDate>

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
BatchTableModelProxy::BatchTableModelProxy(const QCheckBox * exp, const QCheckBox * aexp,
										   const QCheckBox * nexp, QObject * parent) :
												QSortFilterProxyModel(parent),
												cb_exp(exp), cb_aexp(aexp), cb_nexp(nexp) {
}

/**
 * @brief Luck, I'm your father!
 *
 **/
BatchTableModelProxy::~BatchTableModelProxy() {
	FPR(__func__);
}


bool BatchTableModelProxy::filterAcceptsRow(int sourceRow,
											const QModelIndex &sourceParent) const {
	QModelIndex expidx = sourceModel()->index(sourceRow, BatchTableModel::HExpire, sourceParent);
	QString exp = expidx.data(Qt::DisplayRole).toString();
	QString td = QDate::currentDate().toString(Qt::ISODate);
	bool val = false;
	if (cb_exp->isChecked())
		val |= exp < td ? true : false;
	if (cb_aexp->isChecked())
		val |= exp == td ? true : false;
	if (cb_nexp->isChecked())
		val |= exp > td ? true : false;

	return val;
}

#include "BatchTableModelProxy.moc"