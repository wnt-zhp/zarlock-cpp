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

#ifndef BATCHTABLEMODELPROXY_H
#define BATCHTABLEMODELPROXY_H

#include <QtGui/QCheckBox>
#include <QtGui/QSortFilterProxyModel>
#include <QDate>

/**
 * @brief Klasa dziedziczy po QSortFilterProxyModel i odpowiada za
 * sortowanie danych.
 **/
class BatchTableModelProxy : public QSortFilterProxyModel {
Q_OBJECT
public:
	BatchTableModelProxy(const QCheckBox * hide, QObject * parent = 0);
	BatchTableModelProxy(const QCheckBox * exp, const QCheckBox * aexp,
						 const QCheckBox * nexp, const QCheckBox * hide, QObject * parent = 0);
						 
	virtual ~BatchTableModelProxy();

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	virtual void setDateKey(const QDate & dk);
	virtual void setItemNum(int * item);
	virtual void allwaysAccept(const QModelIndex * idx);
	virtual void setFilter(const QString & filter);

	virtual void setExtendedSpec(bool extspec = false);

private:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	const QCheckBox * cb_exp;
	const QCheckBox * cb_aexp;
	const QCheckBox * cb_nexp;
	const QCheckBox * cb_hide;
	QDate datekey;
	int * itemnum;
	const QModelIndex * aaidx;

	QString filter;
	bool extended_spec;
};

#endif // BATCHTABLEMODELPROXY_H
