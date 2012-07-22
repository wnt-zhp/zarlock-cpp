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

#ifndef BATCHTABLEMODELPROXY_H
#define BATCHTABLEMODELPROXY_H

#include <QSortFilterProxyModel>
#include <QAction>
#include <QDate>

/**
 * @brief Klasa dziedziczy po QSortFilterProxyModel i odpowiada za
 * sortowanie danych.
 **/
class BatchTableModelProxy : public QSortFilterProxyModel {
Q_OBJECT
public:
	BatchTableModelProxy(QObject * parent = 0);
	virtual ~BatchTableModelProxy();

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	virtual void setDateKey(const QDate & dk);
	virtual void setItemNum(int * item);
	virtual void allwaysAccept(const QModelIndex * idx);

public slots:
	inline void setShowExpired(bool show) { hide_exp = !show; }
	inline void setShowAExpired(bool show) { hide_aexp = !show; }
	inline void setShowNExpired(bool show) { hide_nexp = !show; }
	inline void setShowEmpty(bool show) { hide_empty = !show; }
	inline void setShowFuture(bool show) { hide_future = !show; }

	inline void setHideExpired(bool hide) { hide_exp = hide; }
	inline void setHideAExpired(bool hide) { hide_aexp = hide; }
	inline void setHideNExpired(bool hide) { hide_nexp = hide; }
	inline void setHideEmpty(bool hide) { hide_empty = hide; }
	inline void setHideFuture(bool hide) { hide_future = hide; }

private:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	bool hide_exp;
	bool hide_aexp;
	bool hide_nexp;
	bool hide_empty;
	bool hide_future;

	QDate datekey;
	int * itemnum;
	const QModelIndex * aaidx;
};

#endif // BATCHTABLEMODELPROXY_H
