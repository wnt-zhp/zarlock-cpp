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
#include "ModelsCommon.h"
#include <QDate>
#include <QVector>

/**
 * @brief Klasa dziedziczy po QSqlTableModel i odpowiada za
 * przechowywanie danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym modelu  danych tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class DistributorTableModel : public QAbstractTableModel, public ModelsCommon {
Q_OBJECT
public:
	DistributorTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
// 	virtual ~DistributorTableModel();

	virtual bool select();
	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual void setTable(const QString & table);

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	virtual bool addRecord(unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b, bool autoupdate = true);
	virtual bool updateRecord(int id, unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b);
	virtual bool removeRecord(int row);

	virtual bool setIndexData(const QModelIndex & idx, const QVariant & data);
	virtual bool setIndexData(int row, int column, const QVariant & data);

	virtual bool selectRow(int row);
	virtual bool selectColumn(int column);
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
	
	QString table;
	QSqlDatabase database;

// 	struct d_record { int id; int bid; int qty; QDate distdate; QDate entrydate; int dt; QString dt_a; QString dt_b; };
	struct d_record { QVariant arr[DummyHeadersSize]; };

	QVector<d_record> records;
	QString columns[DummyHeadersSize];
	QString headers_h[DummyHeadersSize];
};

#endif // DISTRIBUTORTABLEMODEL_H
