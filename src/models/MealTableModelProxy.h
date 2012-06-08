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

#ifndef MEALTABLEMODELPROXY_H
#define MEALTABLEMODELPROXY_H

#include <QtGui/QCheckBox>
#include <QtGui/QSortFilterProxyModel>
#include <QDate>

/**
 * @brief Klasa dziedziczy po QSortFilterProxyModel i odpowiada za
 * sortowanie danych.
 **/
class MealTableModelProxy : public QSortFilterProxyModel {
Q_OBJECT
public:
	MealTableModelProxy(QObject * parent = 0);
						 
	virtual ~MealTableModelProxy();

	virtual void setKey(int key);
	virtual int key() const;

	virtual void setRefDate(const QDate & date);
	virtual const QDate & refDate() const;

private:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	int mealkey;
	QDate ref_date;
};

#endif // MEALTABLEMODELPROXY_H
