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

#include <QStringBuilder>

#include "globals.h"

#include "BatchTableModelProxy.h"
#include "BatchTableModel.h"

#include "Database.h"

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
												cb_hide(hide), itemnum(NULL), extended_spec(false) {
	CI();
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
	CI();
}

/**
 * @brief Luck, I'm your father!
 *
 **/
BatchTableModelProxy::~BatchTableModelProxy() {
	DI();
}

bool BatchTableModelProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
	bool qres = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
	if (!qres)
		return false;

	double free = sourceModel()->index(sourceRow, BatchTableModel::HUsedQty).data(BatchTableModel::RFreeQty).toDouble();	// quantity of free batches

	if (cb_hide and cb_hide->isChecked() and free == 0) {		// hide empty
		if (itemnum == NULL)
			return false;
		else {
			if (sourceRow != *itemnum)
				return false;
		}
	}

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
	QVariant res = QSortFilterProxyModel::data(index, role);

	if ( extended_spec and (index.column() == BatchTableModel::HSpec) and (role == Qt::DisplayRole) ) {
		QString name = this->mapToSource(this->index(index.row(), BatchTableModel::HSpec)).data(Qt::DisplayRole).toString();
		QString unit = this->mapToSource(this->index(index.row(), BatchTableModel::HUnit)).data(Qt::DisplayRole).toString();
		QString price = this->mapToSource(this->index(index.row(), BatchTableModel::HPrice)).data(Qt::DisplayRole).toString();
		
// 		return QVariant(tr("%1\t[ 1 unit = %2,\tprice: %3 zl/%2 ]").arg(res.toString()).arg(unit, 10, ' ').arg(price, 6, ' '));
// 		index.data(Qt::DisplayRole).toString()

		QString item_mark;
		
		if ((itemnum != NULL) and (mapToSource(index).row() == *itemnum))
			name = "* " % name;

		return QVariant(tr("%1 / %2 / %3").arg(name).arg(unit).arg(price));

	}

	if (role == Qt::BackgroundRole) {
			int row = index.row();
			QDate expd = this->index(row, BatchTableModel::HExpiryDate).data(Qt::EditRole).toDate();

			if (!expd.isValid())
				return globals::item_nexpired_base;
			
			int daystoexp = expd.daysTo(QDate::currentDate());
			
			if (daystoexp > 0) {
				if (row % 2)
					return globals::item_expired_base;
				else
					return globals::item_expired_altbase;
			} else if (daystoexp == 0) {
				if (row % 2)
					return globals::item_aexpired_base;
				else
					return globals::item_aexpired_altbase;
			}
			if (row % 2)
				return globals::item_nexpired_base;
			else
				return globals::item_nexpired_altbase;
	}
	return res;
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

void BatchTableModelProxy::setFilter(const QString& filter) {
	this->filter = filter;
}

void BatchTableModelProxy::setExtendedSpec(bool extspec) {
	extended_spec = extspec;
}

#include "BatchTableModelProxy.moc"