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

#ifndef DISTRIBUTORTABLEMODEL_H
#define DISTRIBUTORTABLEMODEL_H

#include <QtSql/QSqlDatabase>
#include <QSqlTableModel>
#include <QAbstractTableModel>
#include <QDate>

#include "ModelsCommon.h"
#include "AbstractTableModel.h"

class Database;

/**
 * @brief Klasa dziedziczy po QSqlTableModel i odpowiada za
 * przechowywanie danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym modelu  danych tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class DistributorTableModel : public AbstractTableModel {
Q_OBJECT
public:
	DistributorTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
// 	virtual ~DistributorTableModel();

	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;

	virtual bool addRecord(unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b, bool autoupdate = true);
	virtual bool updateRecord(int id, unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b);
	virtual bool getRecord(int row, unsigned int & bid, int & qty, QDate & ddate, QDate & rdate, int & disttype, QString & dt_a, QString & dt_b);

	virtual bool fillRow(const QSqlQuery& q, int row, bool emit_signal = true);

public:
	enum UserRoles { RRaw = Qt::UserRole + 1 };

public slots:
// 	void filterDB(const QString &);

public:
	enum Headers {HId = 0, HBatchId, HQty, HDistDate, HEntryDate, HDistType, HDistTypeA, HDistTypeB, DummyHeadersSize };
	enum Reasons {RGeneral = 0, RExpired, RMeal };

protected:
	QVariant display(const QModelIndex & idx, const int role = Qt::DisplayRole) const;
	QVariant raw(const QModelIndex & idx) const;

	virtual QVariant prepareBatch(const QVariant & v);
	virtual QVariant prepareDistTypeB(const QVariant & v);

	virtual QVariant prepareQty(const QVariant& v, const QVariant & bid);
};

#endif // DISTRIBUTORTABLEMODEL_H
