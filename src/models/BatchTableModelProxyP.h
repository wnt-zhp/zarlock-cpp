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

#ifndef BATCHTABLEMODELPROXYP_H
#define BATCHTABLEMODELPROXYP_H

#include <QtGui/QCheckBox>
#include <QtGui/QSortFilterProxyModel>

/**
 * @brief Klasa dziedziczy po QSortFilterProxyModel i odpowiada za
 * sortowanie danych.
 **/
class BatchTableModelProxyP : public QSortFilterProxyModel {
Q_OBJECT
public:
	BatchTableModelProxyP(const QVariant * pid, QObject * parent = 0);
						 
	virtual ~BatchTableModelProxyP();

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	const QVariant * Pid;
};

#endif // BATCHTABLEMODELPROXY_H
