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

#include "AbstractTableModel.h"
#include "BatchTableModel.h"
#include "DataParser.h"

#include "Database.h"

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
AbstractTableModel::AbstractTableModel(QObject* parent, QSqlDatabase sqldb) :
	QAbstractTableModel(parent), database(sqldb),
	sort_column(0), sort_order_asc(true),
	find_column(0), find_role(0) {
	CI();
	db = Database::Instance();
}

AbstractTableModel::~AbstractTableModel() {
	DI();
	clearRecords();
}

/**
 * @brief Ta funkcja odpowiada za pobranie danych z tabeli i synchronizację.
 * Tutaj możemy ustawić nagłówki dla kolumn
 *
 * @return bool stan otwarcia tabeli
 **/
bool AbstractTableModel::select() {
	clearRecords();

	QString query("SELECT * FROM :table: ORDER BY id ASC");
	query.replace(":table:", table);
	QSqlQuery q;
	q.prepare(query);
	q.exec();
	int n = 0;

	if (db->driver()->hasFeature(QSqlDriver::QuerySize))
		n = q.size();
	else {
		while (q.next())
			++n;
	}

	records.reserve(2*n);
	insertRows(0, n);

	int r = -1;

	q.first();
	do {
	fillRow(q, ++r, false, false);
	} while (q.next());

// 	sort(sort_column, sort_order_asc ? Qt::AscendingOrder : Qt::DescendingOrder, true);

	return true;
}

void AbstractTableModel::clearRecords() {
	qDeleteAll(records.begin(), records.end());
}

QVariant AbstractTableModel::prepareDate(const QVariant & v) {
	return v.toDate();
}

QVariant AbstractTableModel::prepareQty(const QVariant & v) {
	return v.toDouble() / 100.0;
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
QVariant AbstractTableModel::data(const QModelIndex & idx, int role) const {
	int col = idx.column();
	int row = idx.row();

	switch (role) {
		case Qt::DisplayRole:
			return records.at(row)->arr[1][col];
			break;
		case Qt::EditRole:
			return records.at(row)->arr[0][col];
			break;
	}
	return QVariant();
}

int AbstractTableModel::columnCount(const QModelIndex& parent) const {
	return headers.size();
}

int AbstractTableModel::rowCount(const QModelIndex& parent) const {
	return records.count();
}

void AbstractTableModel::setTable(const QString& table) {
	this->table = table;
}

QVariant AbstractTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Horizontal) {
		switch (role) {
			case Qt::DisplayRole:
				return headers[section];
				break;
				// 			default:
				// 				return QVariant();
				break;
		}
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}

bool AbstractTableModel::insertRows(int row, int count, const QModelIndex& parent) {
	beginInsertRows(parent, row, row + count - 1);
	for (int c = 0; c < count; ++c) {
		records.insert(row+c, new d_record(this));
	}
	endInsertRows();

	emit rowInserted(row);

	return true;
}

// bool recordLessThan(const AbstractTableModel::d_record * s1, const AbstractTableModel::d_record * s2) {
// 	return ((*s1) < (*s2));
// }

// bool AbstractTableModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {

// }

// void AbstractTableModel::sort(int column, Qt::SortOrder order) {FI();
// 	sort(column, order, true);
// }

// void AbstractTableModel::sort(int column, Qt::SortOrder order, bool emit_signal) {FI();
// 	sort_column = column;
// 	sort_order_asc = ( order == Qt::AscendingOrder ? true : false );

// 	qStableSort(records.begin(), records.end(), recordLessThan);

// 	if (emit_signal)
// 		emit dataChanged(this->index(0, 0), this->index(this->rowCount()-1, this->columnCount()));
// }

QVector< int > AbstractTableModel::find(int column, const QVariant& value, int role, int hits, Qt::MatchFlags flags) {
	find_role = role;
	find_column = column;

	QVector<int> result;
	int hits_found = 0;

	viter it = records.begin();
	viter itb = records.begin();
	viter ite = records.end();

	while (true) {
		it = find(it, ite, value);
		if (it != ite) {
			int row = it - records.begin();
			result.push_back(row);
			if (++hits_found == hits)
				break;
		} else {
			break;
		}
	}

	return result;
}

QVector< int > AbstractTableModel::search(int column, const QVariant& value, int role, int hits, Qt::MatchFlags flags) {
	find_role = role;
	find_column = column;
	
	QVector<int> result;
	int hits_found = 0;
	
	viter it = records.begin();
	viter itb = records.begin();
	viter ite = records.end();

	while (true) {
		while (it != ite) {
			if ((*it)->arr[role][column].toString().contains(value.toString(), Qt::CaseInsensitive))
				break;
			else
				++it;
		}

		if (it != ite) {
			int row = it - records.begin();
			result.push_back(row);
			if (++hits_found == hits)
				break;
		} else {
			break;
		}
	}
	
	return result;
}

Qt::ItemFlags AbstractTableModel::flags(const QModelIndex& index) const {
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable; 
}

bool AbstractTableModel::removeRecord(int row) {
	int id = records.at(row)->arr[0][0].toInt();

	QString query("DELETE FROM :table: WHERE id=?");
	query.replace(":table:", table);

	QSqlQuery q;
	q.prepare(query);
	q.bindValue(0, id);

	bool status = q.exec();
	if (status) {
		beginRemoveRows(QModelIndex(), row, row);
		records.remove(row);
		endRemoveRows();
	}

	return status;
}

bool AbstractTableModel::setIndexData(const QModelIndex& idx, const QVariant& data) {
	return setIndexData(idx.row(), idx.column(), data);
}

bool AbstractTableModel::setIndexData(int row, int column, const QVariant& data) {
	QSqlQuery q;

	QString query("UPDATE :table: SET ?=? WHERE id=?");
	query.replace(":table:", table);

	q.prepare(query);

	bool status = false;
	switch (dtypes[0][column]) {
		case QVariant::Date:
			q.bindValue(0, columns[column]);
			q.bindValue(1, data.toDate().toString(Qt::ISODate));
			q.bindValue(2, records.at(row)->arr[0][HId]);
			status = q.exec();
			break;
		default:
			q.bindValue(0, columns[column]);
			q.bindValue(1, data.toInt());
			q.bindValue(2, records.at(row)->arr[0][HId]);
			status = q.exec();
			break;
	}
	if (status) {
		records[row]->arr[0][column] = data;
	}PR(status);
	return status;
}

bool AbstractTableModel::pushRow(const QSqlQuery& q, bool emit_signal) {
	d_record * r = new d_record(this);
	records.push_back(r);

	return fillRow(q, records.count()-1, emit_signal);
}

int AbstractTableModel::getRowById(int id) throw (int) {
	find_column = HId;

	viter f = find(records.begin(), records.end(), id);

	if (f == records.end()) throw id;

	return (f - records.begin());
}

AbstractTableModel::viter AbstractTableModel::find(AbstractTableModel::viter begin, AbstractTableModel::viter end, const QVariant & val) const {
	while (begin != end && !(**begin == val))
		++begin;
	return begin;
}

AbstractTableModel::d_record::d_record(AbstractTableModel * m) : model(m) {
	arr[0].resize(m->columnCount());
	arr[1].resize(m->columnCount());
}

bool AbstractTableModel::d_record::operator<(const d_record * rhs) const {
	QVariant v1 = this->arr[1][model->sort_column];
	QVariant v2 = rhs->arr[1][rhs->model->sort_column];
	bool res = false;
PR(1);
	switch (v1.type()) {
		case QVariant::LongLong:
			res = !( ( v1.toLongLong() < v2.toLongLong() ) xor model->sort_order_asc);
			break;
		case QVariant::Int:
			res = !( ( v1.toInt() < v2.toInt() ) xor model->sort_order_asc);
			break;
		case QVariant::Date:
			res = !( ( v1.toDate() < v2.toDate() ) xor model->sort_order_asc);
			break;
		case QVariant::String:
// 			v1 = this->arr[1][model->sort_column];
// 			v2 = rhs->arr[1][rhs->model->sort_column];
			res = !( ( v1.toString() < v2.toString() ) xor model->sort_order_asc);
			break;
	}
	return res;
}

bool AbstractTableModel::d_record::operator<(const d_record & rhs) const {
	QVariant v1 = this->arr[1][model->sort_column];
	QVariant v2 = rhs.arr[1][rhs.model->sort_column];
	bool res = false;

	switch (v1.type()) {
		case QVariant::LongLong:
			res = !( ( v1.toLongLong() < v2.toLongLong() ) xor model->sort_order_asc);
			break;
		case QVariant::Int:
			res = !( ( v1.toInt() < v2.toInt() ) xor model->sort_order_asc);
			break;
		case QVariant::Date:
			res = !( ( v1.toDate() < v2.toDate() ) xor model->sort_order_asc);
			break;
		case QVariant::String:
// 			v1 = this->arr[1][model->sort_column];
// 			v2 = rhs.arr[1][rhs.model->sort_column];
			res = !( ( v1.toString() < v2.toString() ) xor model->sort_order_asc);
			break;
	}
	return res;
}

bool AbstractTableModel::d_record::operator==(const QVariant & rhs) const {
	QVariant v1 = this->arr[model->find_role][model->find_column];

	return ( v1 == rhs );
}

// bool AbstractTableModel::d_record::operator==(const d_record & rhs) const {
// 	QVariant v1 = this->arr[model->find_role][model->find_column];
// 	QVariant v2 = rhs->arr[model->find_role][rhs.model->find_column];
// 	bool res = false;
// 	
// 	switch (this->model->dtypes[model->find_column]) {
// 		case AbstractTableModel::DTInt:
// 			res = ( v1.toInt() == v2.toInt() );
// 			break;
// 		case AbstractTableModel::DTDate:
// 			res = ( v1.toString() == v2.toString() );
// 			break;
// 		case AbstractTableModel::DTString:
// 			res = ( v1.toString() == v2.toString() );
// 			break;
// 	}
// 	return res;
// }

QVariant & AbstractTableModel::d_record::operator*() {
	return this->arr[model->find_role][model->find_column];
}

#include "AbstractTableModel.moc"