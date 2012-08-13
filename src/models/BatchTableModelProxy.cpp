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
#include <QApplication>
#include <QStyle>

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
BatchTableModelProxy::BatchTableModelProxy(QObject * parent) :
										   QSortFilterProxyModel(parent), hide_exp(false),
										   hide_aexp(false), hide_nexp(false),
										   hide_empty(false), hide_future(true), itemnum(NULL) {
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

	if (hide_empty and free == 0) {		// hide empty
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
			return (false or !hide_future);
	} else
		refd = QDate::currentDate();

	QDate expds = sourceModel()->index(sourceRow, BatchTableModel::HExpiryDate).data(Qt::EditRole).toDate();

	if (!expds.isValid())
		return true;

	int daystoexp = refd.daysTo(expds);

	if (daystoexp < 0)
		return !hide_exp;
	if (daystoexp == 0)
		return !hide_aexp;
	if (daystoexp > 0)
		return !hide_nexp;

	return true;
}

QVariant BatchTableModelProxy::data(const QModelIndex& index, int role) const {
	if ( role == Qt::DecorationRole ) {

		switch (index.column()) {
			case BatchTableModel::HSpec:
				if ((itemnum != NULL) and (mapToSource(index).row() == *itemnum)) {
					return QApplication::style()->standardIcon(QStyle::SP_ArrowRight);
				}
				break;
			case BatchTableModel::HRegDate:
				{
					QDate refdate;
					if (datekey.isValid()) {
						refdate = datekey;
					} else
						refdate = QDate::currentDate();

					QDate regd = this->index(index.row(), BatchTableModel::HRegDate).data(BatchTableModel::RRaw).toDate();

					if (regd.daysTo(refdate) < 0)
						return QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);

					QPixmap px(QSize(16,16));
					px.fill(Qt::transparent);
					QIcon ic(px);
					return ic;
				}
				break;
			case BatchTableModel::HExpiryDate:
				{
					QDate expd = this->index(index.row(), BatchTableModel::HExpiryDate).data(Qt::EditRole).toDate();
				
					if (!expd.isValid())
						break;
				
					QDate refdate;
					if (datekey.isValid()) {
						refdate = datekey;
					} else
						refdate = QDate::currentDate();
				
					int daystoexp = expd.daysTo(refdate);
				
					if (daystoexp > 0) {
						return QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
					} else if (daystoexp == 0) {
						return QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);
					} else {
						QPixmap px(QSize(16,16));
						px.fill(Qt::transparent);
						QIcon ic(px);
						return ic;
					}
				}
				break;
		}
	}

	if (role == Qt::BackgroundRole) {
			int row = index.row();
			QDate expd = this->index(row, BatchTableModel::HExpiryDate).data(Qt::EditRole).toDate();

			if (!expd.isValid())
				return globals::item_nexpired_base;

			QDate refdate;
			if (datekey.isValid()) {
				refdate = datekey;
			} else
				refdate = QDate::currentDate();

			int daystoexp = expd.daysTo(refdate);

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

#include "BatchTableModelProxy.moc"