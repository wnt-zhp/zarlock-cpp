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

#include "BatchTableModelProxyP.h"
#include "BatchTableModel.h"

#include <QDate>

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
BatchTableModelProxyP::BatchTableModelProxyP(const QVariant * pid, QObject * parent) :
												QSortFilterProxyModel(parent), Pid(pid) {
}

/**
 * @brief Luck, I'm your father!
 *
 **/
BatchTableModelProxyP::~BatchTableModelProxyP() {
	FPR(__func__);
}


bool BatchTableModelProxyP::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	return (*Pid == sourceModel()->index(sourceRow, BatchTableModel::HProdId, sourceParent).data(Qt::EditRole));
}

QVariant BatchTableModelProxyP::data(const QModelIndex& index, int role) const {
	QVariant res = QSortFilterProxyModel::data(index, role);

	if ( (index.column() == BatchTableModel::HSpec) and (role == Qt::DisplayRole) ) {
		QString unit = this->mapToSource(this->index(index.row(), BatchTableModel::HUnit)).data(Qt::DisplayRole).toString();
		QString price = this->mapToSource(this->index(index.row(), BatchTableModel::HPrice)).data(Qt::DisplayRole).toString();

		return QVariant(tr("%1\t[ 1 unit = %2,\tprice: %3 zl/%2 ]").arg(res.toString()).arg(unit, 10, ' ').arg(price, 6, ' '));
	}

	return res;
}

#include "BatchTableModelProxyP.moc"