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

#ifndef DISTRIBUTORTABLEMODELPROXY_H
#define DISTRIBUTORTABLEMODELPROXY_H

#include <QtGui/QCheckBox>
#include <QtGui/QSortFilterProxyModel>
#include <QDate>

/**
 * @brief Klasa dziedziczy po QSortFilterProxyModel i odpowiada za
 * sortowanie danych.
 **/
class DistributorTableModelProxy : public QSortFilterProxyModel {
Q_OBJECT
public:
	DistributorTableModelProxy(const QCheckBox * meal, QObject * parent = 0);
	virtual ~DistributorTableModelProxy();

private:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	const QCheckBox * cb_meal;
};

#endif // DISTRIBUTORTABLEMODELPROXY_H
