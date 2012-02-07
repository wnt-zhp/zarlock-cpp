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

#include "DistributorTableModel.h"
#include "BatchTableModel.h"
#include "DataParser.h"
#include "Database.h"
#include <cstdio>
#include <QSqlError>
/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
DistributorTableModel::DistributorTableModel(QObject* parent, QSqlDatabase db) : database(db) {
	headers_h[0] = tr("ID");
	headers_h[HBatchId] = tr("Batch");
	headers_h[HQty] = tr("Quantity");
	headers_h[HDistDate] = tr("Distributing date");
	headers_h[HEntryDate] = tr("Registered");
	headers_h[HDistType] = tr("Distribution type");
	headers_h[HDistTypeA] = tr("Main reason");
	headers_h[HDistTypeB] = tr("Sub reason");

	headers_h[0] = "id";
	headers_h[HBatchId] = "batch_id";
	headers_h[HQty] = "quantity";
	headers_h[HDistDate] = "distdate";
	headers_h[HEntryDate] = "entrydate";
	headers_h[HDistType] = "disttype";
	headers_h[HDistTypeA] = "disttype_a";
	headers_h[HDistTypeB] = "disttype_b";
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
	
	PR(q.size());
	while (q.next()) {
		records.push_back(d_record{{
			q.value(HId),
			q.value(HBatchId),
			q.value(HQty),
			q.value(HDistDate),
			q.value(HEntryDate),
			q.value(HDistType),
			q.value(HDistTypeA),
			q.value(HDistTypeB)
		}});
	}
	PR(records.count());
	// 	if (!QSqlTableModel::select())
	// 		return false;
	// 	while (canFetchMore())
	// 		fetchMore();
	
	return true;
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

// 	PR(headerData(HDistTypeA, Qt::Horizontal).toString().toStdString());
// 	PR(headerData(HDistTypeA));
// 	switch (role) {
// 		case Qt::DisplayRole:
// 		case Qt::EditRole:
// 			return records[row].arr[col];
// 			break;
// // 		case Qt::FontRole:
// // 			return QFont("Arial");
// // 			break;
// // 		case Qt::BackgroundRole:
// // 			return QBrush(Qt::white);
// // 			break;
// // 		case Qt::ForegroundRole:
// // 			return QBrush(Qt::green);
// // 			break;
// 		default:
// 				return QVariant();
// // 				return QSqlTableModel::data(idx, role);
// 			break;
// 	}

	switch (role) {
		case Qt::StatusTipRole:
		case Qt::DisplayRole:
			if (idx.column() == HQty) {
				return raw(idx).toDouble() / 100.0;
			} else
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
			return records[row].arr[col];
// 			return QSqlTableModel::data(idx, Qt::EditRole);
			break;
// 		default:
// 			return records[idx.row()].arr[idx.column()];
// 			return QVariant();
// 			break;
	}

// 	return QVariant();
	return records[idx.row()].arr[idx.column()];
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
	switch (role) {
		case Qt::DisplayRole:
			if (idx.column() == HBatchId) {
				QModelIndexList qmil = Database::Instance().CachedBatch()->match(Database::Instance().CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, idx.data(DistributorTableModel::RRaw));
				if (!qmil.isEmpty()) {
					return Database::Instance().CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);
				}
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
					QModelIndexList qmil = Database::Instance().CachedBatch()->match(Database::Instance().CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, index(idx.row(), HBatchId).data(RRaw));
					if (!qmil.isEmpty()) {
						return QString(tr("Invoice: ") % Database::Instance().CachedBatch()->index(qmil.first().row(), BatchTableModel::HInvoice).data(Qt::DisplayRole).toString());
					}
				}
			}
			break;
	}
	return records[idx.row()].arr[idx.column()];
// 	return QSqlTableModel::data(idx, Qt::DisplayRole);
}

/**
 * @brief Zwraca dany przechowywane w roli Qt::DisplayRole
 *
 * @param idx indeks
 * @return QVariant
 **/
QVariant DistributorTableModel::raw(const QModelIndex & idx) const {
	if (idx.column() == HBatchId) {
		QModelIndexList qmil = Database::Instance().CachedBatch()->match(Database::Instance().CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, idx.data(DistributorTableModel::RRaw));
		if (!qmil.isEmpty()) {
			return Database::Instance().CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);
		}
		
	} else

	if (idx.column() == HDistDate) {
// 		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate();//.toString("dd-MM-yyyy");
		return records[idx.row()].arr[idx.column()].toDate();
	}

	return records[idx.row()].arr[idx.column()];
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
				BatchTableModel * btm = Database::Instance().CachedBatch();
				int bidrow = -1;

				QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, this->index(index.row(), HBatchId).data(RRaw));
				if (qmil.count() != 1) {
					return false;
				}
				bidrow = qmil.at(0).row();

				int used = Database::Instance().CachedBatch()->index(bidrow, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
				int total = Database::Instance().CachedBatch()->index(bidrow, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
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
}


int DistributorTableModel::columnCount(const QModelIndex& parent) const {
	return HDistTypeB+1;
}

int DistributorTableModel::rowCount(const QModelIndex& parent) const {
	return records.count();
}

void DistributorTableModel::setTable(const QString& table) {
	this->table = table;
}

QVariant DistributorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	switch (orientation) {
		case Qt::Horizontal:
			switch (role) {
				case Qt::DisplayRole:
					return headers_h[section];
					break;
				default:
					return QVariant();
				break;
			}
			break;
	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags DistributorTableModel::flags(const QModelIndex& index) const {
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable; 
// 	return QAbstractItemModel::flags(index);
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

	beginInsertRows(QModelIndex(), n, n);
	records.push_back(d_record{{ id, bid, qty, ddate, rdate, disttype, dt_a, dt_b }});
	endInsertRows();
}

bool DistributorTableModel::updateRecord(int row, unsigned int bid, int qty, const QDate& ddate, const QDate& rdate, int disttype, const QString& dt_a, const QString& dt_b) {
	int id = records[row].arr[HId].toInt();

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

	records.push_back(d_record{{ id, bid, qty, ddate, rdate, disttype, dt_a, dt_b }});
}

bool DistributorTableModel::removeRecord(int row) {
	int id = records[row].arr[HId].toInt();

	QSqlQuery q;
	q.prepare("DELETE FROM distributor WHERE id=?;");
	q.bindValue(0, id);
	bool status = q.exec();
	if (status) {
		beginRemoveRows(QModelIndex(), row, row);
		records.remove(row);
		endRemoveRows();
	}
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
			q.bindValue(2, records[row].arr[HId]);
			status = q.exec();
			break;
		default:
			q.bindValue(0, columns[column]);
			q.bindValue(1, data.toInt());
			q.bindValue(2, records[row].arr[HId]);
			status = q.exec();
			break;
	}
	if (status) {
		records[row].arr[column] = data;
	}
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
		records[++i].arr[column] = q.value(0);
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
		records[row] = {{
			q.value(HId),
			q.value(HBatchId),
			q.value(HQty),
			q.value(HDistDate),
			q.value(HEntryDate),
			q.value(HDistType),
			q.value(HDistTypeA),
			q.value(HDistTypeB)
		}};
	}
	return true;
}

#include "DistributorTableModel.moc"