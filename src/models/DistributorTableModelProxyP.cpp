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

#include "DistributorTableModelProxyP.h"
#include "BatchTableModel.h"
#include "DistributorTableModel.h"

#include <QDate>

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
DistributorTableModelProxyP::DistributorTableModelProxyP(const BatchTableView * batchview, QObject * parent) :
															QSortFilterProxyModel(parent), bv(batchview) {
}

/**
 * @brief Luck, I'm your father!
 *
 **/
DistributorTableModelProxyP::~DistributorTableModelProxyP() {
	FPR(__func__);
}

bool DistributorTableModelProxyP::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	QModelIndex bidx = sourceModel()->index(sourceRow, DistributorTableModel::HBatchId, sourceParent);

	if (!bv or !bv->selectionModel())
		return false;

	int snum = bv->selectionModel()->selectedRows().count();

	for (int i = 0; i < snum; ++i) {
		int row = bv->selectionModel()->selectedRows().at(i).row();
		int bpid = bv->model()->index(row, BatchTableModel::HId).data(Qt::EditRole).toInt();

		if (bpid == bidx.data(DistributorTableModel::RRaw).toInt()) {
// 			sourceModel()->setData(bidx, QColor(Qt::lightGray), Qt::DecorationRole);
			return true;
		}
	}

	return false;
}

QVariant DistributorTableModelProxyP::data(const QModelIndex& index, int role) const {
	QVariant res = QSortFilterProxyModel::data(index, role);

	if ( (index.column() == BatchTableModel::HSpec) and (role == Qt::DisplayRole) ) {
		Database * db = Database::Instance();
		QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, res.toInt());
		if (!qmil.isEmpty()) {
			// 		return db->CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);
			
			QString name = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString();
			QString unit = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();
			QString price = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HPrice).data(Qt::DisplayRole).toString();
			// 		
			// 		return QVariant(tr("%1\t[ 1 unit = %2, price: %3/%2 ]").arg(name).arg(unit).arg(price));
			return QVariant(tr("%1 / %2 / %3").arg(name).arg(unit).arg(price));
		}
	}

	return res;
}

#include "DistributorTableModelProxyP.moc"