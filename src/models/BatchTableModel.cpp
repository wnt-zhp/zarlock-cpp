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
	headers.push_back(tr("Invoice"));
	headers.push_back(tr("Notes"));

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
	columns.push_back("invoice");
	columns.push_back("notes");

	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::Int);
	dtypes[Qt::DisplayRole].push_back(QVariant::Date);
	dtypes[Qt::DisplayRole].push_back(QVariant::Date);
	dtypes[Qt::DisplayRole].push_back(QVariant::Date);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);
	dtypes[Qt::DisplayRole].push_back(QVariant::String);

	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);
	dtypes[Qt::DecorationRole].push_back(QVariant::Icon);

	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::String);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::String);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::Int);
	dtypes[Qt::EditRole].push_back(QVariant::Date);
	dtypes[Qt::EditRole].push_back(QVariant::Date);
	dtypes[Qt::EditRole].push_back(QVariant::Date);
	dtypes[Qt::EditRole].push_back(QVariant::String);
	dtypes[Qt::EditRole].push_back(QVariant::String);
}

BatchTableModel::~BatchTableModel() {
	DI();
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

	if (col == HSpec and role == Qt::UserRole) {
		return filter_data[row];
	}

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
			return records[row]->arr[Qt::EditRole][col];
			break;
		case RFreeQty:
			if (col == HUsedQty) {
				int used = records[row]->arr[Qt::EditRole][col].toInt();
				int total = records[row]->arr[Qt::EditRole][HStaQty].toInt();
				return QVariant(double(total - used)/100.0);
			}
			break;
		case RNameQty:
			if (col == HSpec) {
				return QString(
					records[row]->arr[Qt::DisplayRole][HProdId].toString() % " " %
					records[row]->arr[Qt::EditRole][col].toString() %
					tr(" (quantity: %1)").arg(this->data(this->index(row, HStaQty), RFreeQty).toDouble())
				);
			}
			break;
	}

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
			if (col == HUnit) {
				QString unitf;
				if (DataParser::unit(records[row]->arr[Qt::EditRole][HUnit].toString(), unitf)) {
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
				int used = records[row]->arr[Qt::EditRole][HUsedQty].toInt();
				int total = records[row]->arr[Qt::EditRole][HStaQty].toInt();
				int free = total - used;
				QString qty;
				return tr("   %1 of %2   ").arg(free/100.0, 0, 'f', 2).arg(total/100.0, 0, 'f', 2);
			}
			
			return records[row]->arr[Qt::DisplayRole][col];

			break;
	}

	return QVariant();
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
		return records[row]->arr[Qt::EditRole][col].toDate();
	} else
	if (col == HExpiryDate) {
		return records[row]->arr[Qt::EditRole][col].toDate();
	} else
	if (col == HEntryDate) {
		return records[row]->arr[Qt::EditRole][col].toDate();
	}

	return records[row]->arr[Qt::EditRole][col];
}

bool BatchTableModel::addRecord(int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
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

bool BatchTableModel::updateRecord(int row, int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	int id = records[row]->arr[Qt::EditRole][HId].toInt();

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

	fillRow(q, row, true);

	return true;
}

bool BatchTableModel::getRecord(int row, int& pid, QString& spec, int& price, QString& unit, int& qty, int& used, QDate& reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes) {
	if (row < records.count()) {
		pid			= records[row]->arr[Qt::EditRole][HId].toUInt();
		spec		= records[row]->arr[Qt::EditRole][HSpec].toString();
		price		= records[row]->arr[Qt::EditRole][HPrice].toInt();
		unit		= records[row]->arr[Qt::EditRole][HUnit].toString();
		qty			= records[row]->arr[Qt::EditRole][HStaQty].toInt();
		used		= records[row]->arr[Qt::EditRole][HUsedQty].toInt();
		reg			= records[row]->arr[Qt::EditRole][HRegDate].toDate();
		expiry		= records[row]->arr[Qt::EditRole][HExpiryDate].toDate();
		entry		= records[row]->arr[Qt::EditRole][HEntryDate].toDate();
		invoice		= records[row]->arr[Qt::EditRole][HInvoice].toString();
		notes		= records[row]->arr[Qt::EditRole][HNotes].toString();

		return true;
	}

	return false;
}

bool BatchTableModel::fillRow(const QSqlQuery& q, int row, bool emit_signal) {
	d_record * rec = records[row];

	rec->arr[Qt::EditRole].resize(DummyHeadersSize);
	rec->arr[Qt::DisplayRole].resize(DummyHeadersSize);
	
	for (int r = 0; r < DummyHeadersSize; ++r) {
		rec->arr[Qt::DisplayRole][r]			= q.value(r);
		rec->arr[Qt::DisplayRole][r].convert(dtypes[Qt::DisplayRole][r]);

		rec->arr[Qt::DecorationRole][r]			= QVariant(dtypes[Qt::DecorationRole][r]);

		rec->arr[Qt::EditRole][r]				= q.value(r);
		rec->arr[Qt::EditRole][r].convert(dtypes[Qt::EditRole][r]);
	}

	rec->arr[Qt::DisplayRole][HProdId]			= prepareProduct(q.value(HProdId));
	rec->arr[Qt::DisplayRole][HSpec]			= QVariant(rec->arr[Qt::DisplayRole][HProdId].toString() % " " % rec->arr[Qt::EditRole][HSpec].toString());
	rec->arr[Qt::DisplayRole][HPrice]			= prepareQty(q.value(HPrice));
	rec->arr[Qt::DisplayRole][HStaQty]			= prepareQty(q.value(HStaQty));
	rec->arr[Qt::DisplayRole][HUsedQty]			= prepareQty(q.value(HUsedQty));
	rec->arr[Qt::DisplayRole][HRegDate]			= prepareDate(q.value(HRegDate));
	rec->arr[Qt::DisplayRole][HExpiryDate]		= prepareDate(q.value(HExpiryDate));
	rec->arr[Qt::DisplayRole][HEntryDate]		= prepareDate(q.value(HEntryDate));

	filter_data[row] = rec->arr[Qt::DisplayRole][HSpec].toString() % " " % rec->arr[Qt::DisplayRole][HInvoice].toString();

	if (emit_signal)
		emit dataChanged(this->index(row, HId), this->index(row, this->columnCount()));

	return true;
}

void BatchTableModel::autoSubmit(bool asub) {
	autosubmit = asub;
}

bool BatchTableModel::insertRows(int row, int count, const QModelIndex& parent) {
	filter_data.insert(row, count, QString());

	return AbstractTableModel::insertRows(row, count, parent);
}

bool BatchTableModel::removeRows(int row, int count, const QModelIndex& parent) {
	filter_data.remove(row, count);

	return AbstractTableModel::removeRows(row, count, parent);
}


bool BatchTableModel::pushRow(const QSqlQuery& rec, bool emit_signal) {
	filter_data.push_back(QString());

	return AbstractTableModel::pushRow(rec, emit_signal);
}

#include "BatchTableModel.moc"