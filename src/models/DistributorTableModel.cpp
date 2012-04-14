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

#include "globals.h"

#include <QAbstractTableModel>
#include <QDate>
#include <QTime>
#include <QColor>
#include <QStringBuilder>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

#include <cstdio>

#include "DistributorTableModel.h"
#include "BatchTableModel.h"
#include "DataParser.h"

#include "Database.h"

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
DistributorTableModel::DistributorTableModel(QObject* parent, QSqlDatabase sqldb) : AbstractTableModel(parent, sqldb) {
	CI();
// 	headers.resize(DummyHeadersSize);
// 	headers[HId] = tr("ID");
// 	headers[HBatchId] = tr("Batch");
// 	headers[HQty] = tr("Quantity");
// 	headers[HDistDate] = tr("Distributing date");
// 	headers[HEntryDate] = tr("Registered");
// 	headers[HDistType] = tr("Distribution type");
// 	headers[HDistTypeA] = tr("Main reason");
// 	headers[HDistTypeB] = tr("Sub reason");
// 
// 	columns.resize(DummyHeadersSize);
// 	columns[HId] = "id";
// 	columns[HBatchId] = "batch_id";
// 	columns[HQty] = "quantity";
// 	columns[HDistDate] = "distdate";
// 	columns[HEntryDate] = "entrydate";
// 	columns[HDistType] = "disttype";
// 	columns[HDistTypeA] = "disttype_a";
// 	columns[HDistTypeB] = "disttype_b";

	headers.push_back(tr("ID"));
	headers.push_back(tr("Batch"));
	headers.push_back(tr("Quantity"));
	headers.push_back(tr("Distributing date"));
	headers.push_back(tr("Registered"));
	headers.push_back(tr("Distribution type"));
	headers.push_back(tr("Main reason"));
	headers.push_back(tr("Sub reason"));

	columns.push_back("id");
	columns.push_back("batch_id");
	columns.push_back("quantity");
	columns.push_back("distdate");
	columns.push_back("entrydate");
	columns.push_back("disttype");
	columns.push_back("disttype_a");
	columns.push_back("disttype_b");

	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);

	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);

	dtypes[Qt::EditRole].push_back(QVariant::LongLong);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::Date);
	dtypes[Qt::EditRole].push_back(QVariant::Date);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::String);
}

QVariant DistributorTableModel::prepareBatch(const QVariant & v) {
	QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, v.toInt());
	if (!qmil.isEmpty()) {
// 		return db->CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);

		QString name = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString();
		QString unit = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();
		QString price = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HPrice).data(Qt::DisplayRole).toString();
// 		
// 		return QVariant(tr("%1\t[ 1 unit = %2, price: %3/%2 ]").arg(name).arg(unit).arg(price));
		return QVariant(tr("%1 / %2 / %3").arg(name).arg(unit).arg(price));
	}
	return QVariant();
}

QVariant DistributorTableModel::prepareDistTypeB(const QVariant& v) {
// 	if (v.toInt() == RMeal) {
		QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, v);
		if (!qmil.isEmpty()) {
			return QString(/*tr("Invoice: ") % */db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HInvoice).data(Qt::DisplayRole).toString());
		}
// 	}
	return v;
}

/**
 * @brief Ta funckja nadpisuje standardową funckję zwracającą daną z modelu. Każda komórka tabeli
 * ma dodatkowy wymiar (tzw. rola, opis ról pod hasłem 'Qt Namespace'). Dzieki temu możemy modyfikować
 * to, co trafia do widoku. Jeśli widok zarząda podania danych z roli Qt::EditRole (czyli do edycji) to tak
 * naprawdę podamy mu to co, uzytkownik wprowadził w formularzu (i jest przechowywane domyślnie w Qt::DisplayRole).
 * Kiedy jednak zarząda wyświetlania danych, uruchamiamy parsery i tak pozyskane dane przesyłamy jako dane związane
 * z Qt::DisplayRole
 * Dla różnych kolumn uruchamiamy różne parsery.
 *
 * @param idx indeks wiersza i komórki z daną do pobrania
 * @param role żądana rola związana z tą daną
 * @return QVariant zwraca żądanie (QVariant to taki uniwersalny pojemnik na wiele typów danych:
 * QString, QColor QIcon,itp.
 **/
QVariant DistributorTableModel::data(const QModelIndex & idx, int role) const {
	int col = idx.column();
	int row = idx.row();

	switch (role) {
		case Qt::StatusTipRole:
		case Qt::DisplayRole:
				return display(idx, role);
			break;
		case Qt::EditRole:
			return raw(idx);
			break;
		case Qt::TextAlignmentRole:
			if (col == HQty)
				return Qt::AlignRight + Qt::AlignCenter;
			if (col == HDistDate)
				return Qt::AlignCenter;
			return QVariant();
			break;
		case Qt::CheckStateRole:
			return QVariant();
			break;
		case RRaw:
			return records.at(row)->arr[Qt::EditRole][col];
// 			return QSqlTableModel::data(idx, Qt::EditRole);
			break;
// 		default:
// 			return records[idx.row()]->arr[idx.column()];
// 			return QVariant();
// 			break;
	}

	return QVariant();
// 	return records.at(row)->arr[Qt::EditRole][col];

// 	return QSqlTableModel::data(idx, role);
}

/**
 * @brief Kiedy rządamy wyświetlenia danych, niektóre z nich muszą zostać sparsowane.
 * Korzystamy ze statycznych metod kalsy DataParse
 *
 * @param idx indeks z daną do sparsowania
 * @param role przypisana rola
 * @return QVariant dana po parsowaniu i standaryzacji
 **/
QVariant DistributorTableModel::display(const QModelIndex & idx, const int role) const {
	int row = idx.row();
	int col = idx.column();

	return records.at(row)->arr[Qt::DisplayRole][col];
}

/**
 * @brief Zwraca dany przechowywane w roli Qt::DisplayRole
 *
 * @param idx indeks
 * @return QVariant
 **/
QVariant DistributorTableModel::raw(const QModelIndex & idx) const {
	int row = idx.row();
	int col = idx.column();

	return records.at(row)->arr[Qt::EditRole][col];
}

bool DistributorTableModel::addRecord(unsigned int bid, int qty, const QDate& ddate, const QDate& rdate, int disttype, const QString& dt_a, const QString& dt_b, bool autoupdate) {
	QString query("INSERT INTO :table: VALUES (NULL, ?, ?, ?, ?, ?, ?, ?);");
	query.replace(":table:", table);
	
	QSqlQuery q;
	q.prepare(query);

	q.bindValue(0, bid);
	q.bindValue(1, qty);
	q.bindValue(2, ddate.toString(Qt::ISODate));
	q.bindValue(3, rdate.toString(Qt::ISODate));
	q.bindValue(4, disttype);
	q.bindValue(5, dt_a);
	q.bindValue(6, dt_b);
	bool status = q.exec();

	qInfo(globals::VLevel1, db->getLastExecutedQuery(q).toUtf8());

	if (!status)
		return false;

	query = "SELECT * FROM :table: ORDER BY id DESC LIMIT 1";
	query.replace(":table:", table);

	q.prepare(query);
	q.exec();

	int n = records.count();
	if (!q.next())
		return false;

	beginInsertRows(QModelIndex(), n, n);
	pushRow(q);
	endInsertRows();

	return true;
}

bool DistributorTableModel::updateRecord(int row, unsigned int bid, int qty, const QDate& ddate, const QDate& rdate, int disttype, const QString& dt_a, const QString& dt_b) {
	int id = records.at(row)->arr[Qt::EditRole][HId].toInt();

	QString query("UPDATE :table: SET batch_id=?, quantity=?, distdate=?, entrydate=?, disttype=?, disttype_a=?, disttype_b=? WHERE id=?;");
	query.replace(":table:", table);
	
	QSqlQuery q;
	q.prepare(query);

	q.bindValue(0, bid);
	q.bindValue(1, qty);
	q.bindValue(2, ddate.toString(Qt::ISODate));
	q.bindValue(3, rdate.toString(Qt::ISODate));
	q.bindValue(4, disttype);
	q.bindValue(5, dt_a);
	q.bindValue(6, dt_b);
	q.bindValue(7, id);
	bool status = q.exec();

	qInfo(globals::VLevel1, db->getLastExecutedQuery(q).toUtf8());

	if (!status) {
		qFatal(q.lastError().driverText().toStdString().c_str());
		return false;
	}

	query = "SELECT * FROM :table: WHERE id = ?";
	query.replace(":table:", table);
	
	q.prepare(query);
	q.bindValue(0, id);
	q.exec();
	if (!q.next())
		return false;

	fillRow(q, row);

	return true;
}

bool DistributorTableModel::getRecord(int row, unsigned int& bid, int& qty, QDate& ddate, QDate& rdate, int& disttype, QString& dt_a, QString& dt_b) {
	if (row < records.count()) {
		bid			= records[row]->arr[Qt::EditRole][HId].toUInt();
		qty			= records[row]->arr[Qt::EditRole][HQty].toInt();
		ddate		= records[row]->arr[Qt::EditRole][HDistDate].toDate();
		rdate		= records[row]->arr[Qt::EditRole][HEntryDate].toDate();
		disttype	= records[row]->arr[Qt::EditRole][HDistDate].toInt();
		dt_a		= records[row]->arr[Qt::EditRole][HDistTypeA].toString();
		dt_b		= records[row]->arr[Qt::EditRole][HDistTypeB].toString();

		return true;
	}

	return false;
}

bool DistributorTableModel::fillRow(const QSqlQuery& q, int row, bool emit_signal) {
	d_record * rec = records[row];

	rec->arr[Qt::DisplayRole].resize(DummyHeadersSize);
	rec->arr[Qt::DecorationRole].resize(DummyHeadersSize);
	rec->arr[Qt::EditRole].resize(DummyHeadersSize);

	for (int r = 0; r < DummyHeadersSize; ++r) {
		rec->arr[Qt::DisplayRole][r]			= q.value(r);
		rec->arr[Qt::DisplayRole][r].convert(dtypes[Qt::DisplayRole][r]);

		rec->arr[Qt::DecorationRole][r]			= QVariant(dtypes[Qt::DecorationRole][r]);

		rec->arr[Qt::EditRole][r]				= q.value(r);
		rec->arr[Qt::EditRole][r].convert(dtypes[Qt::EditRole][r]);
		
	}
	
	rec->arr[Qt::DisplayRole][HBatchId]			= prepareBatch(q.value(HBatchId));
	rec->arr[Qt::DisplayRole][HQty]				= prepareQty(q.value(HQty), rec->arr[Qt::EditRole][HBatchId]);
	rec->arr[Qt::DisplayRole][HDistDate]		= prepareDate(q.value(HDistDate));
	rec->arr[Qt::DisplayRole][HEntryDate]		= prepareDate(q.value(HEntryDate));
	rec->arr[Qt::DisplayRole][HDistTypeB]		= prepareDistTypeB(q.value(HId));

	if (emit_signal)
		emit rowInserted(getRowById(rec->arr[Qt::DisplayRole][HId].toInt()));

	return true;
}

QVariant DistributorTableModel::prepareQty(const QVariant& v, const QVariant & bid) {
	QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, bid);
	if (!qmil.isEmpty()) {

// 		QString name = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString();
		QString unit = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HUnit).data(Qt::DisplayRole).toString();
		QString price = db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HPrice).data(Qt::DisplayRole).toString();

// 		return QVariant(tr("%1\t[ 1 unit = %2,\tprice: %3 zl/%2 ]").arg(v.toDouble()/100.0, 6, 'g', -1, ' ').arg(unit, 10, ' ').arg(price, 6, ' '));
	}

	return v.toDouble();
// 	return AbstractTableModel::prepareQty(v);
}

#include "DistributorTableModel.moc"