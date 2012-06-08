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

#ifndef DISTRIBUTORTABLEMODELPROXYP_H
#define DISTRIBUTORTABLEMODELPROXYP_H

#include <QtGui/QCheckBox>
#include <QtGui/QSortFilterProxyModel>
#include <QDate>
#include "BatchTableView.h"

/**
 * @brief Klasa dziedziczy po QSortFilterProxyModel i odpowiada za
 * sortowanie danych.
 **/
class DistributorTableModelProxyP : public QSortFilterProxyModel {
Q_OBJECT
public:
	DistributorTableModelProxyP(const BatchTableView * batchview, QObject * parent = 0);
						 
	virtual ~DistributorTableModelProxyP();

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	const BatchTableView * bv;
};

#endif // DISTRIBUTORTABLEMODELPROXYP_H
