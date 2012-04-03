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

#include <cstdio>

#include "DistributorTableModel.h"
#include "BatchTableModel.h"
#include "DataParser.h"

#include "Database.h"

int DistributorTableModel::sort_column = 0;
bool DistributorTableModel::sort_order_asc = true;

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
DistributorTableModel::DistributorTableModel(QObject* parent, QSqlDatabase sqldb) : AbstractTableModel(parent, sqldb) {
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

	dtypes.push_back(DTInt);
	dtypes.push_back(DTString);
	dtypes.push_back(DTInt);
	dtypes.push_back(DTDate);
	dtypes.push_back(DTDate);
	dtypes.push_back(DTInt);
	dtypes.push_back(DTInt);
	dtypes.push_back(DTInt);
}

/**
 * @brief Ta funkcja odpowiada za pobranie danych z tabeli i synchronizację.
 * Tutaj możemy ustawić nagłówki dla kolumn
 *
 * @return bool stan otwarcia tabeli
 **/
bool DistributorTableModel::select() {
	records.clear();

	QString query("SELECT * FROM :table:");
	query.replace(":table:", table);
	QSqlQuery q;//(database);
	q.prepare(query);
	q.exec();

// 	PR(q.size());
	while (q.next()) {
		d_record rec(this);
		rec.arr[0].resize(DummyHeadersSize);
		rec.arr[1].resize(DummyHeadersSize);

		for (int r = 0; r < DummyHeadersSize; ++r) {
			rec.arr[0][r]			= q.value(r);
			rec.arr[1][r]			= q.value(r);
		}

		rec.arr[1][HBatchId]	= prepareBatch(q.value(HBatchId));
		rec.arr[1][HQty]		= prepareQty(q.value(HQty));
		rec.arr[1][HDistDate]	= prepareDate(q.value(HDistDate));
		rec.arr[1][HEntryDate]	= prepareDate(q.value(HEntryDate));
		rec.arr[1][HDistTypeB]	= prepareDistTypeB(q.value(HId));

		records.push_back(rec);
	}

	sort(sort_column, sort_order_asc ? Qt::AscendingOrder : Qt::DescendingOrder);
	return true;
}

QVariant DistributorTableModel::prepareBatch(const QVariant & v) {
	QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, v.toInt());
	if (!qmil.isEmpty()) {
		return db->CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);
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
			return records.at(row).arr[0][col];
// 			return QSqlTableModel::data(idx, Qt::EditRole);
			break;
// 		default:
// 			return records[idx.row()].arr[idx.column()];
// 			return QVariant();
// 			break;
	}

// 	return QVariant();
	return records.at(row).arr[0][col];

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

	return records.at(row).arr[1][col];
/*
	switch (role) {
		case Qt::DisplayRole:
			if (idx.column() == HBatchId) {
				return records[idx.row()].arr[DummyHeadersSize + idx.column()];
			}

			else if (idx.column() == HEntryDate) {
// 				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate();//.toString(Qt::DefaultLocaleShortDate);
				return records[idx.row()].arr[idx.column()].toDate();
			}

			else if (idx.column() == HDistDate) {
// 				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate();//.toString(Qt::DefaultLocaleShortDate);
				return records[idx.row()].arr[idx.column()].toDate();
			}
			else if (idx.column() == HDistTypeB) {
				if (index(idx.row(), DistributorTableModel::HDistType).data().toInt() == RMeal) {
					QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, index(idx.row(), HBatchId).data(RRaw));
					if (!qmil.isEmpty()) {
						return QString(tr("Invoice: ") % db->CachedBatch()->index(qmil.first().row(), BatchTableModel::HInvoice).data(Qt::DisplayRole).toString());
					}
				}
			}
			break;
	}
	return records[idx.row()].arr[idx.column()];
// 	return QSqlTableModel::data(idx, Qt::DisplayRole);
*/
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

	if (idx.column() == HBatchId) {
		QModelIndexList qmil = db->CachedBatch()->match(db->CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, idx.data(DistributorTableModel::RRaw));
		if (!qmil.isEmpty()) {
			return db->CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);
		}
		
	} else

	if (idx.column() == HDistDate) {
// 		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate();//.toString("dd-MM-yyyy");
		return records.at(row).arr[0][col].toDate();
	}

	return records.at(row).arr[0][col];
}

/**
 * @brief Wprowadzamu nowe/zmieniamy dane do/w tabeli
 *
 * @param index indeks danych
 * @param value wartość
 * @param role przypisana im rola
 * @return bool stan dodania/aktualizacji
 **/
bool DistributorTableModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	switch (role) {
		case Qt::EditRole:
			if (index.column() == HDistDate) {
// 				qdate date;
// 				if (dataparser::date(value.tostring(), date)) {
// 					return qsqltablemodel::setdata(index, date.tostring(qt::isodate), qt::editrole);
// 				} else {
// 					inputerrormsgbox(value.tostring());
// 					return false;
// 				}
// 				return qsqltablemodel::setdata(index, value.todate().tostring(qt::isodate), qt::editrole);
			}

			if (index.column() == HEntryDate) {
// 				QDate date;
// 				if (!DataParser::date(value.toString(), date)) {
// 					inputErrorMsgBox(value.toString());
// 					return false;
// 				}
// 				return QSqlTableModel::setData(index, value.toDate().toString(Qt::ISODate), Qt::EditRole);
			}

			if (index.column() == HQty) {
				BatchTableModel * btm = db->CachedBatch();
				int bidrow = -1;

				QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, this->index(index.row(), HBatchId).data(RRaw));
				if (qmil.count() != 1) {
					return false;
				}
				bidrow = qmil.at(0).row();

				int used = db->CachedBatch()->index(bidrow, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
				int total = db->CachedBatch()->index(bidrow, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
				int fake = index.data(Qt::EditRole).toInt();

				int free = total - used + fake;

				if (free < value.toInt()) {
					inputErrorMsgBox(value.toString());
					return false;
				}
// 				return QSqlTableModel::setData(index, value.toInt(), role);
			}
			break;
	}
//     return QSqlTableModel::setData(index, value, role);

	return true;
}

bool DistributorTableModel::addRecord(unsigned int bid, int qty, const QDate& ddate, const QDate& rdate, int disttype, const QString& dt_a, const QString& dt_b, bool autoupdate) {
	QSqlQuery q;
	q.prepare("INSERT INTO distributor VALUES (NULL, ?, ?, ?, ?, ?, ?, ?);");
	q.bindValue(0, bid);
	q.bindValue(1, qty);
	q.bindValue(2, ddate.toString(Qt::ISODate));
	q.bindValue(3, rdate.toString(Qt::ISODate));
	q.bindValue(4, disttype);
	q.bindValue(5, dt_a);
	q.bindValue(6, dt_b);
	bool status = q.exec();

	if (!status)
		return false;

	q.prepare("SELECT id FROM distributor ORDER BY id DESC LIMIT 1");
	q.exec();

	int id = -1;

	if (q.next())
		id = q.value(0).toInt();
	else
		return false;

	int n = records.count();

	d_record rec(this);

	rec.arr[0][HId]			= id;
	rec.arr[0][HBatchId]	= bid;
	rec.arr[0][HQty]		= qty;
	rec.arr[0][HDistDate]	= ddate;
	rec.arr[0][HEntryDate]	= rdate;
	rec.arr[0][HDistType]	= disttype;
	rec.arr[0][HDistTypeA]	= dt_a;
	rec.arr[0][HDistTypeB]	= dt_b;
	
	rec.arr[1][HId]			= id;
	rec.arr[1][HBatchId]	= prepareBatch(bid);
	rec.arr[1][HQty]		= prepareQty(qty);
	rec.arr[1][HDistDate]	= prepareDate(ddate);
	rec.arr[1][HEntryDate]	= prepareDate(rdate);
	rec.arr[1][HDistType]	= disttype;
	rec.arr[1][HDistTypeA]	= dt_a;
	rec.arr[1][HDistTypeB]	= prepareDistTypeB(id);

	beginInsertRows(QModelIndex(), n, n);
	records.push_back(rec);
	emit dataChanged(this->index(rowCount(), HId), this->index(rowCount(), HDistTypeB));
	endInsertRows();

	return true;
}

bool DistributorTableModel::updateRecord(int row, unsigned int bid, int qty, const QDate& ddate, const QDate& rdate, int disttype, const QString& dt_a, const QString& dt_b) {
	int id = records.at(row).arr[0][HId].toInt();

	QSqlQuery q;
	q.prepare("UPDATE distributor SET batch_id=?, quantity=?, distdate=?, entrydate=?, disttype=?, disttype_a=?, disttype_b? WHERE id=?;");
	q.bindValue(0, bid);
	q.bindValue(1, qty);
	q.bindValue(2, ddate.toString(Qt::ISODate));
	q.bindValue(3, rdate.toString(Qt::ISODate));
	q.bindValue(4, disttype);
	q.bindValue(5, dt_a);
	q.bindValue(6, dt_b);
	q.bindValue(7, id);
	bool status = q.exec();
	
	if (!status)
		return false;

	d_record rec(this);
	
	rec.arr[0][HId]			= id;
	rec.arr[0][HBatchId]	= bid;
	rec.arr[0][HQty]		= qty;
	rec.arr[0][HDistDate]	= ddate;
	rec.arr[0][HEntryDate]	= rdate;
	rec.arr[0][HDistType]	= disttype;
	rec.arr[0][HDistTypeA]	= dt_a;
	rec.arr[0][HDistTypeB]	= dt_b;
	
	rec.arr[1][HId]			= id;
	rec.arr[1][HBatchId]	= prepareBatch(bid);
	rec.arr[1][HQty]		= prepareQty(qty);
	rec.arr[1][HDistDate]	= prepareDate(ddate);
	rec.arr[1][HEntryDate]	= prepareDate(rdate);
	rec.arr[1][HDistType]	= disttype;
	rec.arr[1][HDistTypeA]	= dt_a;
	rec.arr[1][HDistTypeB]	= prepareDistTypeB(id);

	records[row] = rec;
// 	records.push_back(d_record{{ id, bid, qty, ddate, rdate, disttype, dt_a, dt_b }});

	return true;
}

bool DistributorTableModel::removeRecord(int row) {
	int id = records.at(row).arr[0][HId].toInt();

	QSqlQuery q;
	q.prepare("DELETE FROM distributor WHERE id=?;");
	q.bindValue(0, id);
	bool status = q.exec();
	if (status) {
		beginRemoveRows(QModelIndex(), row, row);
		records.remove(row);
		endRemoveRows();
	}

	return status;
}

bool DistributorTableModel::setIndexData(const QModelIndex& idx, const QVariant& data) {
	return setIndexData(idx.row(), idx.column(), data);
}

bool DistributorTableModel::setIndexData(int row, int column, const QVariant& data) {
	QSqlQuery q;
	q.prepare("UPDATE distributor SET ?=? WHERE id=?");
	bool status = false;
	switch (column) {
		case HDistDate:
		case HEntryDate:
			q.bindValue(0, columns[column]);
			q.bindValue(1, data.toDate().toString(Qt::ISODate));
			q.bindValue(2, records.at(row).arr[0][HId]);
			status = q.exec();
			break;
		default:
			q.bindValue(0, columns[column]);
			q.bindValue(1, data.toInt());
			q.bindValue(2, records.at(row).arr[0][HId]);
			status = q.exec();
			break;
	}
	if (status) {
		records[row].arr[0][column] = data;
	}PR(status);
	return status;
}

bool DistributorTableModel::selectColumn(int column) {
	QString query("SELECT :column: FROM :table:");
	query.replace(":column:", columns[column]);
	query.replace(":table:", table);
	QSqlQuery q;
	q.prepare(query);
	q.exec();
	
	PR(q.size());
	int i = -1;
	while (q.next()) {
		records[++i].arr[0][column] = q.value(0);
	}
	
	return true;
}

bool DistributorTableModel::selectRow(int row) {
	QString query("SELECT * FROM :table: LIMIT 1 OFFSET :offset:");
	query.replace(":table:", table);
	query.replace(":offset:", QString("%d").arg(row));

	QSqlQuery q;
	q.prepare(query);
	q.exec();
	
	while (q.next()) {
		d_record rec(this);
		
		rec.arr[0][HId]			= q.value(HId);
		rec.arr[0][HBatchId]	= q.value(HBatchId);
		rec.arr[0][HQty]		= q.value(HQty);
		rec.arr[0][HDistDate]	= q.value(HDistDate);
		rec.arr[0][HEntryDate]	= q.value(HEntryDate);
		rec.arr[0][HDistType]	= q.value(HDistType);
		rec.arr[0][HDistTypeA]	= q.value(HDistTypeA);
		rec.arr[0][HDistTypeB]	= q.value(HDistTypeB);
		
		rec.arr[1][HId]			= q.value(HId);
		rec.arr[1][HBatchId]	= prepareBatch(q.value(HBatchId));
		rec.arr[1][HQty]		= prepareQty(q.value(HQty));
		rec.arr[1][HDistDate]	= prepareDate(q.value(HDistDate));
		rec.arr[1][HEntryDate]	= prepareDate(q.value(HEntryDate));
		rec.arr[1][HDistType]	= q.value(HDistType);
		rec.arr[1][HDistTypeA]	= q.value(HDistTypeA);
		rec.arr[1][HDistTypeB]	= prepareDistTypeB(q.value(HId));

		records[row] = rec;
	}
	return true;
}

#include "DistributorTableModel.moc"