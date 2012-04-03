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
BatchTableModelProxy::BatchTableModelProxy(const QCheckBox * hide, QObject * parent) :
												QSortFilterProxyModel(parent),
												cb_exp(NULL), cb_aexp(NULL), cb_nexp(NULL),
												cb_hide(hide), itemnum(NULL) {
}

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
BatchTableModelProxy::BatchTableModelProxy(const QCheckBox * exp, const QCheckBox * aexp,
										   const QCheckBox * nexp, const QCheckBox * hide,
										   QObject * parent) :
												QSortFilterProxyModel(parent),
												cb_exp(exp), cb_aexp(aexp), cb_nexp(nexp), cb_hide(hide), itemnum(NULL) {
}

/**
 * @brief Luck, I'm your father!
 *
 **/
BatchTableModelProxy::~BatchTableModelProxy() {
	FPR(__func__);
}

bool BatchTableModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	double free = sourceModel()->index(sourceRow, BatchTableModel::HUsedQty).data(BatchTableModel::RFreeQty).toDouble();	// quantity of free batches

	if (cb_hide and cb_hide->isChecked() and free == 0)		// hide empty
		return ((itemnum != NULL) and (sourceRow == *itemnum)) /*false*/;

	QDate refd;
	if (datekey.isValid()) {
		refd = datekey;
		QDate regd = sourceModel()->index(sourceRow, BatchTableModel::HRegDate).data(BatchTableModel::RRaw).toDate();
		if (regd.daysTo(refd) < 0)
			return false;
	} else
		refd = QDate::currentDate();

	QDate expds = sourceModel()->index(sourceRow, BatchTableModel::HExpiryDate).data(Qt::EditRole).toDate();

	if (!expds.isValid())
		return true;

	int daystoexp = refd.daysTo(expds);

	if (cb_exp and daystoexp < 0)
		return cb_exp->isChecked();
	if (cb_aexp and daystoexp == 0)
		return cb_aexp->isChecked();
	if (cb_nexp and daystoexp > 0)
		return cb_nexp->isChecked();

	return true;
}

QVariant BatchTableModelProxy::data(const QModelIndex& index, int role) const {
	return QSortFilterProxyModel::data(index, role);
}

void BatchTableModelProxy::setDateKey(const QDate& dk) {
	datekey = dk;
}

void BatchTableModelProxy::setItemNum(int * item) {
	itemnum = item;
}

void BatchTableModelProxy::allwaysAccept(const QModelIndex * idx) {
	aaidx = idx;
}

void BatchTableModelProxy::sort(int column, Qt::SortOrder order) {
// 	QSortFilterProxyModel::sort(column, order);
	sourceModel()->sort(column, order);

	emit dataChanged(this->index(0, 0), this->index(this->rowCount(), this->columnCount()));
}

#include "BatchTableModelProxy.moc"