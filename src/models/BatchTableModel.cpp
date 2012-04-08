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
	CI();
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

	dtypes[0].push_back(QVariant::Int);
	dtypes[0].push_back(QVariant::Int);
	dtypes[0].push_back(QVariant::String);
	dtypes[0].push_back(QVariant::Int);
	dtypes[0].push_back(QVariant::String);
	dtypes[0].push_back(QVariant::Int);
	dtypes[0].push_back(QVariant::Int);
	dtypes[0].push_back(QVariant::Date);
	dtypes[0].push_back(QVariant::Date);
	dtypes[0].push_back(QVariant::Date);
	dtypes[0].push_back(QVariant::String);
	dtypes[0].push_back(QVariant::String);

	dtypes[1].push_back(QVariant::Int);
	dtypes[1].push_back(QVariant::String);
	dtypes[1].push_back(QVariant::String);
	dtypes[1].push_back(QVariant::Int);
	dtypes[1].push_back(QVariant::String);
	dtypes[1].push_back(QVariant::Int);
	dtypes[1].push_back(QVariant::Int);
	dtypes[1].push_back(QVariant::Date);
	dtypes[1].push_back(QVariant::Date);
	dtypes[1].push_back(QVariant::Date);
	dtypes[1].push_back(QVariant::String);
	dtypes[1].push_back(QVariant::String);
}

BatchTableModel::~BatchTableModel() {
	FPR(__func__);
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
			return records[row]->arr[0][col];
			break;
		case RFreeQty:
			if (col == HUsedQty) {
				int used = records[row]->arr[0][col].toInt();
				int total = records[row]->arr[0][HStaQty].toInt();
				return QVariant(double(total - used)/100.0);
			}
			break;
		case RNameQty:
			if (col == HSpec) {
				return QString(
					records[row]->arr[1][HProdId].toString() % " " %
					records[row]->arr[0][col].toString() %
					tr(" (quantity: %1)").arg(this->data(this->index(row, HStaQty), RFreeQty).toDouble())
				);
			}
			break;
	}

// 	return records[row]->arr[0][col];
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
					records[row]->arr[1][HProdId].toString() % " " % records[row]->arr[0][col].toString());
			}

			else if (col == HPrice) {
				return idx.data(Qt::EditRole).toInt()/100.0;
			}

			else if (col == HUnit) {
				QString unitf;
				if (DataParser::unit(records[row]->arr[0][HUnit].toString(), unitf)) {
					return unitf;
				} else {
					if (role == Qt::BackgroundRole)
						return QColor(Qt::red);
					else
						return QVariant(tr("Parser error!"));
				}
			}

			else if (col == HExpiryDate) {
				if (idx.data(Qt::EditRole).isNull())
					return QString(new QChar(0x221e), 1);
			}

			else if (col == HStaQty) {
				int used = records[row]->arr[0][HUsedQty].toInt();
				int total = records[row]->arr[0][HStaQty].toInt();
				int free = total - used;
				QString qty;
				return tr("%1 of %2").arg(free/100.0, 0, 'f', 2).arg(total/100.0, 0, 'f', 2);
			}

			break;
		case Qt::BackgroundRole:
			QDate expd = records[row]->arr[1][HExpiryDate].toDate();

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

	return records[row]->arr[1][col];
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
		return records[row]->arr[0][col].toDate();
	} else
	if (col == HExpiryDate) {
		return records[row]->arr[0][col].toDate();
	} else
	if (col == HEntryDate) {
		return records[row]->arr[0][col].toDate();
	}

	return records[row]->arr[0][col];
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

bool BatchTableModel::addRecord(unsigned int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	QString query("INSERT INTO :table: VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
	query.replace(":table:", table);
	
	QSqlQuery q;
	q.prepare(query);

	q.bindValue(0, pid);
	q.bindValue(1, spec);
	q.bindValue(2, price);
	q.bindValue(3, unit);
	q.bindValue(4, qty);
	q.bindValue(5, 0);
	q.bindValue(6, reg.toString(Qt::ISODate));
	q.bindValue(7, expiry.toString(Qt::ISODate));
	q.bindValue(8, entry.toString(Qt::ISODate));
	q.bindValue(9, invoice);
	q.bindValue(10, notes);
	bool status = q.exec();

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

bool BatchTableModel::updateRecord(int row, unsigned int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	int id = records[row]->arr[0][HId].toInt();

	QString query("UPDATE :table: SET prod_id=?, spec=?, price=?, unit=?, start_qty=?, regdate=?, expirydate=?, entrydate=?, invoice=?, notes=? WHERE id=?;");
	query.replace(":table:", table);

 	QSqlQuery q;
	q.prepare(query);

	q.bindValue(0, pid);
	q.bindValue(1, spec);
	q.bindValue(2, price);
	q.bindValue(3, unit);
	q.bindValue(4, qty);
	q.bindValue(5, reg.toString(Qt::ISODate));
	q.bindValue(6, expiry.toString(Qt::ISODate));
	q.bindValue(7, entry.toString(Qt::ISODate));
	q.bindValue(8, invoice);
	q.bindValue(9, notes);
	q.bindValue(10, id);

	bool status = q.exec();

	qInfo(globals::VLevel1, db->getLastExecutedQuery(q).toUtf8());

	if (!status)
		return false;

	query = "SELECT * FROM :table: WHERE id = ?";
	query.replace(":table:", table);

	q.prepare(query);
	q.bindValue(0, id);
	q.exec();

	if (!q.next())
		return false;

	fillRow(q, row, true, true);

	return true;
}

bool BatchTableModel::getRecord(int row, unsigned int& pid, QString& spec, int& price, QString& unit, int& qty, int& used, QDate& reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes) {
	if (row < records.count()) {
		pid			= records[row]->arr[0][HId].toUInt();
		spec		= records[row]->arr[0][HSpec].toString();
		price		= records[row]->arr[0][HPrice].toInt();
		unit		= records[row]->arr[0][HUnit].toString();
		qty			= records[row]->arr[0][HStaQty].toInt();
		used		= records[row]->arr[0][HUsedQty].toInt();
		reg			= records[row]->arr[0][HRegDate].toDate();
		expiry		= records[row]->arr[0][HExpiryDate].toDate();
		entry		= records[row]->arr[0][HEntryDate].toDate();
		invoice		= records[row]->arr[0][HInvoice].toString();
		notes		= records[row]->arr[0][HNotes].toString();

		return true;
	}

	return false;
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
// 			q.bindValue(2, records[row]->arr[0][HId]);
// 			status = q.exec();
// 			break;
// 		default:
// 			q.bindValue(0, columns[column]);
// 			q.bindValue(1, data.toInt());
// 			q.bindValue(2, records[row]->arr[0][HId]);
// 			status = q.exec();
// 			break;
// 	}
// 	if (status) {
// 		records[row]->arr[0][column] = data;
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
		records[++i]->arr[0][column] = q.value(0);
	}
	
	return true;
}

bool BatchTableModel::selectRow(int row) {
	QString query("SELECT * FROM :table: LIMIT 1 OFFSET :offset:");
	query.replace(":table:", table);
	query.replace(":offset:", QString("%d").arg(row));
	
	QSqlQuery q;
	q.prepare(query);
	q.exec();
	
	while (q.next()) {
		fillRow(q, row, true, true);
	}
	return true;
}

bool BatchTableModel::fillRow(const QSqlQuery& q, int row, bool do_sort, bool emit_signal) {
	d_record * rec = records[row];

	rec->arr[0].resize(DummyHeadersSize);
	rec->arr[1].resize(DummyHeadersSize);
	
	for (int r = 0; r < DummyHeadersSize; ++r) {
		rec->arr[0][r]				= q.value(r);
		rec->arr[1][r]				= q.value(r);
		rec->arr[0][r].convert(dtypes[0][r]);
		rec->arr[1][r].convert(dtypes[1][r]);
	}

	rec->arr[1][HProdId]			= prepareProduct(q.value(HProdId));
	rec->arr[1][HSpec]				= QVariant(rec->arr[1][HProdId].toString() % " " % rec->arr[0][HSpec].toString());
	rec->arr[1][HStaQty]			= prepareQty(q.value(HStaQty));
	rec->arr[1][HUsedQty]			= prepareQty(q.value(HUsedQty));
	rec->arr[1][HRegDate]			= prepareDate(q.value(HRegDate));
	rec->arr[1][HExpiryDate]		= prepareDate(q.value(HExpiryDate));
	rec->arr[1][HEntryDate]			= prepareDate(q.value(HEntryDate));

// 	if (do_sort)
// 		sort(sort_column, sort_order_asc ? Qt::AscendingOrder : Qt::DescendingOrder, emit_signal);

	emit dataChanged(this->index(row, HId), this->index(row, this->columnCount()));

	return true;
}

void BatchTableModel::autoSubmit(bool asub) {
	autosubmit = asub;
}

#include "BatchTableModel.moc"