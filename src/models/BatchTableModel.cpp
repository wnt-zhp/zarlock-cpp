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
#include <QMessageBox>
#include <QStringBuilder>
#include <QSqlQuery>

#include "BatchTableModel.h"
#include "DataParser.h"
#include "Database.h"
#include <qsqlresult.h>
#include <qsqlrecord.h>


BatchTableModel::BatchTableModel(QObject* parent, QSqlDatabase sqldb): AbstractTableModel(parent, sqldb), autosubmit(true) {
	headers.push_back(tr("ID"));
	headers.push_back(tr("Product"));
	headers.push_back(tr("Specificator"));
	headers.push_back(tr("Price"));
	headers.push_back(tr("Unit"));
	headers.push_back(tr("Quantity"));
	headers.push_back(tr("Used"));
	headers.push_back(tr("Booking"));
	headers.push_back(tr("Expiry"));
	headers.push_back(tr("Entry date"));
	headers.push_back(tr("Notes"));
	headers.push_back(tr("Invoice"));

	columns.push_back("id");
	columns.push_back("prod_id");
	columns.push_back("spec");
	columns.push_back("price");
	columns.push_back("unit");
	columns.push_back("start_qty");
	columns.push_back("used_qty");
	columns.push_back("regdate");
	columns.push_back("expirydate");
	columns.push_back("entrydate");
	columns.push_back("notes");
	columns.push_back("invoice");

	dtypes.push_back(DTInt);
	dtypes.push_back(DTString);
	dtypes.push_back(DTString);
	dtypes.push_back(DTInt);
	dtypes.push_back(DTString);
	dtypes.push_back(DTInt);
	dtypes.push_back(DTInt);
	dtypes.push_back(DTDate);
	dtypes.push_back(DTDate);
	dtypes.push_back(DTDate);
	dtypes.push_back(DTString);
	dtypes.push_back(DTString);

	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(trigDataChanged()));
}

BatchTableModel::~BatchTableModel() {
	FPR(__func__);
}

/**
 * @brief Ta funkcja odpowiada za pobranie danych z tabeli i synchronizację.
 * Tutaj możemy ustawić nagłówki dla kolumn
 *
 * @return bool stan otwarcia tabeli
 **/
bool BatchTableModel::select() {
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

		rec.arr[1][HProdId]			= prepareProduct(q.value(HProdId));
		rec.arr[1][HStaQty]			= prepareQty(q.value(HStaQty));
		rec.arr[1][HUsedQty]		= prepareQty(q.value(HUsedQty));
		rec.arr[1][HRegDate]		= prepareDate(q.value(HRegDate));
		rec.arr[1][HExpiryDate]		= prepareDate(q.value(HExpiryDate));
		rec.arr[1][HEntryDate]		= prepareDate(q.value(HEntryDate));

		records.push_back(rec);
	}

	this->sort(sort_column, sort_order_asc ? Qt::AscendingOrder : Qt::DescendingOrder);
	return true;
}

QVariant BatchTableModel::prepareProduct(const QVariant& v) {
	QModelIndexList qmil = db->CachedProducts()->match(db->CachedProducts()->index(0, ProductsTableModel::HId), Qt::EditRole, v.toInt());
	if (!qmil.isEmpty()) {
		return db->CachedProducts()->index(qmil.first().row(), ProductsTableModel::HName).data(Qt::DisplayRole);
	}
	return QVariant();
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
QVariant BatchTableModel::data(const QModelIndex & idx, int role) const {
	int row = idx.row();
	int col = idx.column();

	switch (role) {
		case Qt::DisplayRole:
		case Qt::BackgroundRole:
		case Qt::StatusTipRole:
			return display(idx, role);
			break;
		case Qt::EditRole:
				return raw(idx);
			break;
		case Qt::TextAlignmentRole:
			if (col == HPrice or col == HUnit or col == HStaQty)
				return Qt::AlignRight + Qt::AlignCenter;
			if (col == HRegDate or col == HExpiryDate)
				return Qt::AlignCenter;
			return QVariant();
			break;
		case Qt::CheckStateRole:
			return QVariant();
			break;
		case RRaw:
			return records[row].arr[0][col];
			break;
		case RFreeQty:
			if (col == HUsedQty) {
				int used = records[row].arr[0][col].toInt();
				int total = records[row].arr[0][HStaQty].toInt();
				return QVariant(double(total - used)/100.0);
			}
			break;
		case RNameQty:
			if (col == HSpec) {
				return QString(
					records[row].arr[1][HProdId].toString() % " " %
					records[row].arr[0][col].toString() %
					tr(" (quantity: %1)").arg(this->data(this->index(row, HStaQty), RFreeQty).toDouble())
				);
			}
			break;
	}

// 	return QSqlTableModel::data(idx, role);
// 	return records[row].arr[0][col];
	return QVariant();
}

/**
 * @brief Kiedy rządamy wyświetlenia danych, niektóre z nich muszą zostać sparsowane.
 * Korzystamy ze statycznych metod klasy DataParse
 *
 * @param idx indeks z daną do sparsowania
 * @param role przypisana rola
 * @return QVariant dana po parsowaniu i standaryzacji
 **/
QVariant BatchTableModel::display(const QModelIndex & idx, const int role) const {
	int row = idx.row();
	int col = idx.column();

	switch (role) {
		case Qt::StatusTipRole:
		case Qt::DisplayRole:
			if (col == HProdId) {
				QModelIndexList qmil = db->CachedProducts()->match(
					db->CachedProducts()->index(0, ProductsTableModel::HId),
					Qt::EditRole, idx.data(RRaw).toInt(), 1, Qt::MatchExactly);
				if (!qmil.isEmpty()) {
					return db->CachedProducts()->index(qmil.at(0).row(), ProductsTableModel::HName).data(Qt::DisplayRole);
				}
			}

			else if (col == HSpec) {
				return QString(
					records[row].arr[1][HProdId].toString() % " " % records[row].arr[0][col].toString());
			}
			
			else if (col == HPrice) {
				return idx.data(Qt::EditRole).toInt()/100.0;
			}
			
			else if (col == HUnit) {
				QString unitf;
				if (DataParser::unit(records[row].arr[0][HUnit].toString(), unitf)) {
					return unitf;
				} else {
					if (role == Qt::BackgroundRole)
						return QColor(Qt::red);
					else
						return QVariant(tr("Parser error!"));
				}
			}
			
			else if (col == HRegDate) {
				return records[row].arr[1][col].toDate().toString(Qt::DefaultLocaleShortDate);
			}
			
			else if (col == HExpiryDate) {
				if (idx.data(Qt::EditRole).isNull())
					return QString(new QChar(0x221e), 1);
				
// 				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
				return records[row].arr[1][col].toDate().toString(Qt::DefaultLocaleShortDate);
				// 				QDate date;
				// 				if (DataParser::date(idx.data(Qt::EditRole).toString(), date, QSqlTableModel::data(index(row, HRegDate), Qt::DisplayRole).toDate())) {
					// 					return date.toString(Qt::DefaultLocaleShortDate);
				// 				} else {
					// 					if (role == Qt::BackgroundRole)
				// 						return QColor(Qt::red);
				// 					else
				// 						return QVariant(tr("Parser error!"));
				// 				}
			}
			
			else if (col == HStaQty) {
				double used = index(row, int(HUsedQty)).data().toDouble();
				double total = raw(idx).toDouble();
				double free = total - used;
				QString qty;
				return tr("%1 of %2").arg(free/100.0, 0, 'f', 2).arg(total/100.0, 0, 'f', 2);
			}
			
// 			else if (col == HENameQty) {
// 				return QString(
// 					records[row].arr[1][HProdId].toString() % " " %
// 					records[row].arr[1][col].toString() %
// 					tr(" (quantity: %1)").arg(this->data(this->index(row, HStaQty), RFreeQty).toDouble())
// 					);
// 			}
			break;
		case Qt::BackgroundRole:
			QDate expd = records[row].arr[1][HExpiryDate].toDate();

			if (!expd.isValid())
				return globals::item_nexpired;
			
			int daystoexp = expd.daysTo(QDate::currentDate());
			
			if (daystoexp > 0) {
				return globals::item_expired;
			} else if (daystoexp == 0) {
				return globals::item_aexpired;
			}
			return globals::item_nexpired;
			
			break;
	}

	return records[row].arr[1][col];
// 	return QSqlTableModel::data(idx, Qt::DisplayRole);
}

/**
	* @brief Zwraca dany przechowywane w roli Qt::DisplayRole
	*
	* @param idx indeks
	* @return QVariant
	**/
QVariant BatchTableModel::raw(const QModelIndex & idx) const {
	int row = idx.row();
	int col = idx.column();

	if (col == HRegDate) {
		return records[row].arr[0][col].toDate();
// 		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate()/*.toString("dd-MM-yyyy")*/;
	} else
	if (col == HExpiryDate) {
		return records[row].arr[0][col].toDate();
// 		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate()/*.toString("dd-MM-yyyy")*/;
	} else
	if (col == HEntryDate) {
		return records[row].arr[0][col].toDate();
// 		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate()/*.toString("dd-MM-yyyy")*/;
	}

	return records[row].arr[0][col];
// 	return QSqlTableModel::data(idx, Qt::DisplayRole);
}

/**
 * @brief Wprowadzamu nowe/zmieniamy dane do/w tabeli
 *
 * @param index indeks danych
 * @param value wartość
 * @param role przypisana im rola
 * @return bool stan dodania/aktualizacji
 **/
bool BatchTableModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	switch (role) {
		case Qt::EditRole:
			if (index.column() == HPrice) {
				double netto, vat;
				if (!DataParser::price(value.toString(), netto, vat)) {
					inputErrorMsgBox(value.toString());
					return false;
				}
// 				FIXME:
// 				return QSqlTableModel::setData(index, int(netto*(100+vat)), role);
			}

			if (index.column() == HUnit) {
				QString unitf;
				if (!DataParser::unit(value.toString(), unitf)) {
					inputErrorMsgBox(value.toString());
					return false;
				}
			}

			if (index.column() == HRegDate) {
// 				FIXME:
// 				return QSqlTableModel::setData(index, value.toDate().toString(Qt::ISODate), Qt::EditRole);
			}

			if (index.column() == HExpiryDate) {
// 				FIXME:
// 				return QSqlTableModel::setData(index, value.toDate().toString(Qt::ISODate), Qt::EditRole);
			}

			if (index.column() == HStaQty) {
				int used = this->index(index.row(), HUsedQty).data().toInt();
				int total = value.toDouble()*100;
// 				int free = total - used;

				if (total < used) {
					inputErrorMsgBox(value.toString());
					return false;
				}
// 				FIXME:
// 				return QSqlTableModel::setData(index, total, role);
			}
			break;
	}
// 	FIXME:
//	return QSqlTableModel::setData(index, value, role);

	return false;
}

bool BatchTableModel::addRecord(unsigned int pid, const QString& spec, const QString& price, const QString& unit, double qty, double used, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
// 	QSqlQuery q;
// 	q.prepare("INSERT INTO distributor VALUES (NULL, ?, ?, ?, ?, ?, ?, ?);");
// 	q.bindValue(0, bid);
// 	q.bindValue(1, qty);
// 	q.bindValue(2, ddate.toString(Qt::ISODate));
// 	q.bindValue(3, rdate.toString(Qt::ISODate));
// 	q.bindValue(4, disttype);
// 	q.bindValue(5, dt_a);
// 	q.bindValue(6, dt_b);
// 	bool status = q.exec();
// 	
// 	if (!status)
// 		return false;
// 	
// 	q.prepare("SELECT id FROM distributor ORDER BY id DESC LIMIT 1");
// 	q.exec();
// 	
// 	int id = -1;
// 	
// 	if (q.next())
// 		id = q.value(0).toInt();
// 	else
// 		return false;
// 	
// 	int n = records.count();
// 	
// 	d_record rec(this);
// 	
// 	rec.arr[0][HId]			= id;
// 	rec.arr[0][HBatchId]	= bid;
// 	rec.arr[0][HQty]		= qty;
// 	rec.arr[0][HDistDate]	= ddate;
// 	rec.arr[0][HEntryDate]	= rdate;
// 	rec.arr[0][HDistType]	= disttype;
// 	rec.arr[0][HDistTypeA]	= dt_a;
// 	rec.arr[0][HDistTypeB]	= dt_b;
// 	
// 	rec.arr[1][HId]			= id;
// 	rec.arr[1][HBatchId]	= prepareBatch(bid);
// 	rec.arr[1][HQty]		= prepareQty(qty);
// 	rec.arr[1][HDistDate]	= prepareDate(ddate);
// 	rec.arr[1][HEntryDate]	= prepareDate(rdate);
// 	rec.arr[1][HDistType]	= disttype;
// 	rec.arr[1][HDistTypeA]	= dt_a;
// 	rec.arr[1][HDistTypeB]	= prepareDistTypeB(id);
// 	
// 	beginInsertRows(QModelIndex(), n, n);
// 	records.push_back(rec);
// 	emit dataChanged(this->index(rowCount(), HId), this->index(rowCount(), HDistTypeB));
// 	endInsertRows();

	return true;
}

bool BatchTableModel::updateRecord(int row, unsigned int pid, const QString& spec, const QString& price, const QString& unit, double qty, double used, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
// 	int id = records[row].arr[0][HId].toInt();
// 	
// 	QSqlQuery q;
// 	q.prepare("UPDATE distributor SET batch_id=?, quantity=?, distdate=?, entrydate=?, disttype=?, disttype_a=?, disttype_b? WHERE id=?;");
// 	q.bindValue(0, bid);
// 	q.bindValue(1, qty);
// 	q.bindValue(2, ddate.toString(Qt::ISODate));
// 	q.bindValue(3, rdate.toString(Qt::ISODate));
// 	q.bindValue(4, disttype);
// 	q.bindValue(5, dt_a);
// 	q.bindValue(6, dt_b);
// 	q.bindValue(7, id);
// 	bool status = q.exec();
// 	
// 	if (!status)
// 		return false;
// 	
// 	d_record rec(this);
// 	
// 	rec.arr[0][HId]			= id;
// 	rec.arr[0][HBatchId]	= bid;
// 	rec.arr[0][HQty]		= qty;
// 	rec.arr[0][HDistDate]	= ddate;
// 	rec.arr[0][HEntryDate]	= rdate;
// 	rec.arr[0][HDistType]	= disttype;
// 	rec.arr[0][HDistTypeA]	= dt_a;
// 	rec.arr[0][HDistTypeB]	= dt_b;
// 	
// 	rec.arr[1][HId]			= id;
// 	rec.arr[1][HBatchId]	= prepareBatch(bid);
// 	rec.arr[1][HQty]		= prepareQty(qty);
// 	rec.arr[1][HDistDate]	= prepareDate(ddate);
// 	rec.arr[1][HEntryDate]	= prepareDate(rdate);
// 	rec.arr[1][HDistType]	= disttype;
// 	rec.arr[1][HDistTypeA]	= dt_a;
// 	rec.arr[1][HDistTypeB]	= prepareDistTypeB(id);
// 	
// 	records[row] = rec;
// 	// 	records.push_back(d_record{{ id, bid, qty, ddate, rdate, disttype, dt_a, dt_b }});

	return true;
}

bool BatchTableModel::removeRecord(int row) {
	int id = records[row].arr[0][HId].toInt();
	
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

bool BatchTableModel::setIndexData(const QModelIndex& idx, const QVariant& data) {
	return setIndexData(idx.row(), idx.column(), data);
}

bool BatchTableModel::setIndexData(int row, int column, const QVariant& data) {
	QSqlQuery q;
	q.prepare("UPDATE distributor SET ?=? WHERE id=?");
	bool status = false;
// 	switch (column) {
// 		case HDistDate:
// 		case HEntryDate:
// 			q.bindValue(0, columns[column]);
// 			q.bindValue(1, data.toDate().toString(Qt::ISODate));
// 			q.bindValue(2, records[row].arr[0][HId]);
// 			status = q.exec();
// 			break;
// 		default:
// 			q.bindValue(0, columns[column]);
// 			q.bindValue(1, data.toInt());
// 			q.bindValue(2, records[row].arr[0][HId]);
// 			status = q.exec();
// 			break;
// 	}
// 	if (status) {
// 		records[row].arr[0][column] = data;
// 	}PR(status);
	return status;
}

bool BatchTableModel::selectColumn(int column) {
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

bool BatchTableModel::selectRow(int row) {
// 	QString query("SELECT * FROM :table: LIMIT 1 OFFSET :offset:");
// 	query.replace(":table:", table);
// 	query.replace(":offset:", QString("%d").arg(row));
// 	
// 	QSqlQuery q;
// 	q.prepare(query);
// 	q.exec();
// 	
// 	while (q.next()) {
// 		d_record rec(this);
// 		
// 		rec.arr[0][HId]			= q.value(HId);
// 		rec.arr[0][HBatchId]	= q.value(HBatchId);
// 		rec.arr[0][HQty]		= q.value(HQty);
// 		rec.arr[0][HDistDate]	= q.value(HDistDate);
// 		rec.arr[0][HEntryDate]	= q.value(HEntryDate);
// 		rec.arr[0][HDistType]	= q.value(HDistType);
// 		rec.arr[0][HDistTypeA]	= q.value(HDistTypeA);
// 		rec.arr[0][HDistTypeB]	= q.value(HDistTypeB);
// 		
// 		rec.arr[1][HId]			= q.value(HId);
// 		rec.arr[1][HBatchId]	= prepareBatch(q.value(HBatchId));
// 		rec.arr[1][HQty]		= prepareQty(q.value(HQty));
// 		rec.arr[1][HDistDate]	= prepareDate(q.value(HDistDate));
// 		rec.arr[1][HEntryDate]	= prepareDate(q.value(HEntryDate));
// 		rec.arr[1][HDistType]	= q.value(HDistType);
// 		rec.arr[1][HDistTypeA]	= q.value(HDistTypeA);
// 		rec.arr[1][HDistTypeB]	= prepareDistTypeB(q.value(HId));
// 		
// 		records[row] = rec;
// 	}
	return true;
}

/**
 * @brief Tworzy filtr na pole 'name' dla danych z tabeli products.
 * Filtr ma postać 'f*'.
 *
 * @param f filtr
 **/
void BatchTableModel::filterDB(const QString & f) {
	QString filter = "";
	if (!f.isEmpty())
		filter = "spec GLOB '" % f % "*'";
// 	setFilter(filter);
}

void BatchTableModel::autoSubmit(bool asub) {
	autosubmit = asub;
}

void BatchTableModel::trigDataChanged() {
// 	if (autosubmit) {
// 		this->submitAll();
// 		Database * db = Database::Instance();
// 		db->updateBatchWordList();
// 	}
}
 
#include "BatchTableModel.moc"