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

	virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	virtual bool addRecord(int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes);
	virtual bool updateRecord(int row, int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes);
	virtual bool getRecord(int row, int & pid, QString& spec, int & price, QString& unit, int & qty, int & used, QDate& reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes);

	virtual bool fillRow(const QSqlQuery& q, int row, bool emit_signal = true);

	virtual void autoSubmit(bool asub = true);

public:
	enum Headers {HId = 0, HProdId, HSpec, HPrice, HUnit, HStaQty, HUsedQty, HRegDate, HExpiryDate, HEntryDate, HInvoice, HNotes, DummyHeadersSize }; // HENameQty
	enum UserRoles { RRaw = Qt::UserRole + 1, RNameQty = Qt::UserRole + 10, RFreeQty };

protected:
	virtual bool pushRow(const QSqlQuery& rec, bool emit_signal = true);

private:
	QVariant display(const QModelIndex & idx, const int role = Qt::DisplayRole) const;
	QVariant raw(const QModelIndex & idx) const;

protected:
	QVariant prepareProduct(const QVariant & v);

	bool autosubmit;

	QVector<QString> filter_data;
};

#endif // BATCHTABLEMODEL_H
