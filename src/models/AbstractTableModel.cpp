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
	sort_column(0), sort_order_asc(0),
	find_column(0), find_role(0) {
	db = Database::Instance();
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
			return records.at(row).arr[1][col];
			break;
		case Qt::EditRole:
			return records.at(row).arr[0][col];
			break;
	}
	return QVariant();
}

int AbstractTableModel::columnCount(const QModelIndex& parent) const {
	if (records.size() > 0)
		return records[0].arr[0].size();
	else
		return 0;
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

void AbstractTableModel::sort(int column, Qt::SortOrder order) {
	sort_column = column;
	sort_order_asc = ( order == Qt::AscendingOrder ? true : false );

	qStableSort(records);

	emit dataChanged(this->index(0, 0), this->index(this->rowCount(), this->columnCount()));
}

QVector< int > AbstractTableModel::find(int column, const QVariant& value, int role, int hits, Qt::MatchFlags flags) {
	find_role = role;
	find_column = column;
	find_value = value;

	QVector<int> result;
	int hits_found = 0;

	QVector<AbstractTableModel::d_record>::const_iterator it = records.begin();
	QVector<AbstractTableModel::d_record>::const_iterator itb = records.begin();
	QVector<AbstractTableModel::d_record>::const_iterator ite = records.end();

	while (true) {
		it = qFind(it, ite, 0);
		if (it != ite) {
			int d = it - itb;
			PR(d);
			result.push_back(d);
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
// 	return QAbstractItemModel::flags(index);
}

bool AbstractTableModel::removeRecord(int row) {
	int id = records.at(row).arr[0][0].toInt();

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
	switch (dtypes[column]) {
		case DTDate:
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

AbstractTableModel::d_record::d_record(AbstractTableModel * m) : model(m) {
	arr[0].resize(m->columnCount());
	arr[1].resize(m->columnCount());
}

bool AbstractTableModel::d_record::operator<(const d_record & rhs) const {
	QVariant v1 = this->arr[0][model->sort_column];
	QVariant v2 = rhs.arr[0][rhs.model->sort_column];
	bool res = false;
	
	switch (this->model->dtypes[model->sort_column]) {
		case AbstractTableModel::DTInt:
			res = !( ( v1.toInt() < v2.toInt() ) xor model->sort_order_asc);
			break;
		case AbstractTableModel::DTDate:
			res = !( ( v1.toDate() < v2.toDate() ) xor model->sort_order_asc);
			break;
		case AbstractTableModel::DTString:
			v1 = this->arr[1][model->sort_column];
			v2 = rhs.arr[1][rhs.model->sort_column];
			res = !( ( v1.toString() < v2.toString() ) xor model->sort_order_asc);
			break;
	}
	return res;
}

bool AbstractTableModel::d_record::operator==(const QVariant & rhs) const {
	QVariant v1 = this->arr[model->find_role][model->find_column];
	bool res = false;
	
	switch (this->model->dtypes[model->find_column]) {
		case AbstractTableModel::DTInt:
			res = ( v1.toInt() == rhs.toInt() );
			break;
		case AbstractTableModel::DTDate:
			res = ( v1.toString() == rhs.toString() );
			break;
		case AbstractTableModel::DTString:
			res = ( v1.toString() == rhs.toString() );
			break;
	}
	return res;
}

// bool AbstractTableModel::d_record::operator==(const d_record & rhs) const {
// 	QVariant v1 = this->arr[model->find_role][model->find_column];
// 	QVariant v2 = rhs.arr[model->find_role][rhs.model->find_column];
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

#include "AbstractTableModel.moc"