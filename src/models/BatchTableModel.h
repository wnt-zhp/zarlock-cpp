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

#ifndef BATCHTABLEMODEL_H
#define BATCHTABLEMODEL_H

#include <QtSql/QSqlTableModel>
#include "AbstractTableModel.h"
#include "ModelsCommon.h"

/**
 * @brief Klasa dziedziczy po QSqlTableModel i odpowiada za
 * przechowywanie danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym modelu  danych tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class BatchTableModel : public AbstractTableModel {
Q_OBJECT
public:
	BatchTableModel(QObject * parent = NULL, QSqlDatabase db = QSqlDatabase());
	virtual ~BatchTableModel();

	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;

	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	virtual bool addRecord(unsigned int pid, const QString& spec, int price, const QString& unit, int qty, int used, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes);
	virtual bool updateRecord(int row, unsigned int pid, const QString& spec, int price, const QString& unit, int qty, int used, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes);
	virtual bool getRecord(int row, unsigned int & pid, QString& spec, int & price, QString& unit, int & qty, int & used, QDate& reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes);
	
	virtual bool setIndexData(const QModelIndex & idx, const QVariant & data);
	virtual bool setIndexData(int row, int column, const QVariant & data);
	
	virtual bool selectRow(int row);
	virtual bool selectColumn(int column);

	virtual bool fillRow(const QSqlQuery& q, int row, bool emit_signal = true);

	virtual void autoSubmit(bool asub = true);

public:
	enum Headers {HId = 0, HProdId, HSpec, HPrice, HUnit, HStaQty, HUsedQty, HRegDate, HExpiryDate, HEntryDate, HNotes, HInvoice, DummyHeadersSize }; // HENameQty
	enum UserRoles { RRaw = Qt::UserRole + 1, RNameQty = Qt::UserRole + 10, RFreeQty };

private:
	QVariant display(const QModelIndex & idx, const int role = Qt::DisplayRole) const;
	QVariant raw(const QModelIndex & idx) const;

protected:
	QVariant prepareProduct(const QVariant & v);

	bool autosubmit;
};

#endif // BATCHTABLEMODEL_H
