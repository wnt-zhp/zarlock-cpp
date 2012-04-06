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

	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	virtual bool addRecord(unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b, bool autoupdate = true);
	virtual bool updateRecord(int id, unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b);

	virtual bool setIndexData(const QModelIndex & idx, const QVariant & data);
	virtual bool setIndexData(int row, int column, const QVariant & data);

	virtual bool selectRow(int row);
	virtual bool selectColumn(int column);

	virtual bool fillRow(const QSqlQuery& q, int row, bool do_sort = true, bool emit_signal = true);

public:
	enum UserRoles { RRaw = Qt::UserRole + 1 };

public slots:
// 	void filterDB(const QString &);

public:
	enum Headers {HId = 0, HBatchId, HQty, HDistDate, HEntryDate, HDistType, HDistTypeA, HDistTypeB, DummyHeadersSize };
	enum Reasons {RGeneral = 0, RExpired, RMeal };

	static int sort_column;
	static bool sort_order_asc;

protected:
	QVariant display(const QModelIndex & idx, const int role = Qt::DisplayRole) const;
	QVariant raw(const QModelIndex & idx) const;

	QVariant prepareBatch(const QVariant & v);
	QVariant prepareDistTypeB(const QVariant & v);
};

#endif // DISTRIBUTORTABLEMODEL_H
