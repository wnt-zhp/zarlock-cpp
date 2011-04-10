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

#include <QAbstractTableModel>
#include <QDate>
#include <QColor>

#include "ProductsTableModel.h"
#include "DataParser.h"

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
ProductsTableModel::ProductsTableModel(QObject* parent, QSqlDatabase db): QSqlTableModel(parent, db) {
}

/**
 * @brief I tu też nic.
 *
 **/
ProductsTableModel::~ProductsTableModel() {
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
QVariant ProductsTableModel::data(const QModelIndex & idx, int role) const {
	if (role == Qt::EditRole || role == Qt::StatusTipRole)
		return raw(idx);

	if (role == Qt::DisplayRole)
		return display(idx);

	return QSqlTableModel::data(idx, role);
}

/**
 * @brief Zmieniamy dane w tabeli
 *
 * @param index indeks danych
 * @param value wartość
 * @param role przypisana im rola
 * @return bool stan aktualizacji
 **/
bool ProductsTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    return QSqlTableModel::setData(index, value, role);
}

/**
 * @brief Ta funkcja odpowiada za pobranie danych z tabeli i synchronizację.
 * Tutaj możemy ustawić nagłówki dla kolumn
 *
 * @return bool stan otwarcia tabeli
 **/
bool ProductsTableModel::select() {
	setHeaderData(HId, Qt::Horizontal, QObject::tr("ID"));
	setHeaderData(HName, Qt::Horizontal, QObject::tr("Name"));
	setHeaderData(HUnit, Qt::Horizontal, QObject::tr("Unit"));
	setHeaderData(HExpire, Qt::Horizontal, QObject::tr("Expire date"));

    return QSqlTableModel::select();
}

/**
 * @brief Kiedy rządamy wyświetlenia danych, niektóre z nich muszą zostać sparsowane.
 * Korzystamy ze statycznych metod kalsy DataParse
 *
 * @param idx indeks z daną do sparsowania
 * @param role przypisana rola
 * @return QVariant dana po parsowaniu i standaryzacji
 **/
QVariant ProductsTableModel::display(const QModelIndex & idx, const int role) const {
// 	PR(idx.data(Qt::EditRole).toString().toStdString());
	if (idx.column() == HExpire) {
		QString data = idx.data(Qt::EditRole).toString();
		QDate date;
		if (DataParser::date(data, date)) {
			QString var;
			return date.toString("yyyy-MM-dd");
		} else {
			if (role == Qt::BackgroundRole)
				return QColor(Qt::red);
			else
				return QVariant(tr("Parser error!"));
		}
	}
	if (idx.column() == HExpire) {
		QDate data = QDate::currentDate().addDays(idx.data(Qt::EditRole).toInt());
		return data;
	}
	return QSqlTableModel::data(idx, Qt::DisplayRole).toString();
}

/**
 * @brief Zwraca dany przechowywane w roli Qt::DisplayRole
 *
 * @param idx indeks
 * @return QVariant
 **/
QVariant ProductsTableModel::raw(const QModelIndex & idx) const {
	return QSqlTableModel::data(idx, Qt::DisplayRole).toString();
}

/**
 * @brief Tworzy filtr na pole 'name' dla danych z tabeli products.
 * Filtr ma postać 'f*'.
 *
 * @param f filtr
 **/
void ProductsTableModel::filterDB(const QString & f) {
	QString filter = "";
	if (!f.isEmpty())
		filter = "name GLOB '" + f + "*'";
	setFilter(filter);
}

#include "ProductsTableModel.moc"