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

#ifndef ABSTRACTTABLEMODEL_H
#define ABSTRACTTABLEMODEL_H

#include <QtSql/QSqlDatabase>
#include <QSqlTableModel>
#include <QAbstractTableModel>
#include <QDate>
#include <QVector>
#include <QtAlgorithms>

#include "ModelsCommon.h"

class Database;

/**
 * @brief Klasa dziedziczy po QSqlTableModel i odpowiada za
 * przechowywanie danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym modelu  danych tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class AbstractTableModel : public QAbstractTableModel, public ModelsCommon {
Q_OBJECT
public:
	AbstractTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
// 	virtual ~AbstractTableModel();

	virtual bool select() = 0;
	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) = 0;
	virtual void setTable(const QString & table);

	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	virtual QVector<int> find(int column, const QVariant& value, int role, int hits = 1, Qt::MatchFlags flags = Qt::MatchStartsWith | Qt::MatchWrap);

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	virtual bool removeRecord(int row);

	virtual bool setIndexData(const QModelIndex & idx, const QVariant & data);
	virtual bool setIndexData(int row, int column, const QVariant & data);

	virtual bool selectRow(int row) = 0;
	virtual bool selectColumn(int column) = 0;

public:
	enum Headers {HId = 0, DummyHeadersSize };
	enum UserRoles { RRaw = Qt::UserRole + 1 };

public slots:
// 	void filterDB(const QString &);

private:
	QSqlDatabase database;

protected:
	QString table;

	QVariant prepareQty(const QVariant & v);
	QVariant prepareDate(const QVariant & v);

	QVector<QString> columns;
	QVector<QString> headers;

	enum AbsDataType { DTInt, DTDate, DTString };
	QVector<AbsDataType> dtypes;

	struct d_record {
		d_record(AbstractTableModel * m = NULL);
		bool operator<(const d_record & rhs) const;
		bool operator==(const QVariant & rhs) const;

		QVector<QVariant> arr[2];
		AbstractTableModel * model;
	};

	QVector<d_record> records;

	int sort_column;
	bool sort_order_asc;
	int find_column;
	int find_role;
	QVariant find_value;

	Database * db;
};

#endif // ABSTRACTTABLEMODEL_H
