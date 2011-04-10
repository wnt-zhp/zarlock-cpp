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

#include "BatchTableModel.h"
#include "DataParser.h"

/**
 * @brief Konstruktor - nic się nie dzieje.
 *
 * @param parent rodzic
 * @param db Połączenie do bazy danych, z których model będzie pobierał dane
 **/
BatchTableModel::BatchTableModel(QObject* parent, QSqlDatabase db): QSqlRelationalTableModel(parent, db) {
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(trigDataChanged(QModelIndex,QModelIndex)));
}

/**
 * @brief Luck, I'm your father!
 *
 **/
BatchTableModel::~BatchTableModel() {
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
	if (role == Qt::EditRole or role == Qt::StatusTipRole)
		return raw(idx);

	if (role == Qt::DisplayRole or role == Qt::BackgroundRole)
		return display(idx, role);

	int col = idx.column();
	if (role == Qt::TextAlignmentRole and (col == HPrice or col == HUnit or col == HStaQty))
		return Qt::AlignRight;
	if (role == Qt::TextAlignmentRole and (col == HBook or col == HExpire))
		return Qt::AlignCenter;

	return QSqlRelationalTableModel::data(idx, role);
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
    return QSqlRelationalTableModel::setData(index, value, role);
}

/**
 * @brief Ta funkcja odpowiada za pobranie danych z tabeli i synchronizację.
 * Tutaj możemy ustawić nagłówki dla kolumn
 *
 * @return bool stan otwarcia tabeli
 **/
bool BatchTableModel::select() {
	setHeaderData(HId,		Qt::Horizontal, QObject::tr("ID"));
	setHeaderData(HProdId,	Qt::Horizontal, QObject::tr("Product"));
	setHeaderData(HSpec,	Qt::Horizontal, QObject::tr("Specificator"));
	setHeaderData(HPrice,	Qt::Horizontal, QObject::tr("Price"));
	setHeaderData(HUnit,	Qt::Horizontal, QObject::tr("Unit"));
	setHeaderData(HStaQty,	Qt::Horizontal, QObject::tr("Quantity"));
	setHeaderData(HBook,	Qt::Horizontal, QObject::tr("Booking"));
	setHeaderData(HExpire,	Qt::Horizontal, QObject::tr("Expiry"));
	setHeaderData(HUsedQty,	Qt::Horizontal, QObject::tr("Used"));
	setHeaderData(HRegDate,	Qt::Horizontal, QObject::tr("Registered"));
	setHeaderData(HDesc,	Qt::Horizontal, QObject::tr("Desc"));
	setHeaderData(HInvoice,	Qt::Horizontal, QObject::tr("Invoice No."));

    return QSqlTableModel::select();
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
	switch (role) {
		case Qt::DisplayRole:
			if (idx.column() == HSpec) {
				return QString(this->data(this->index(idx.row(), HProdId), Qt::DisplayRole).toString() % " " % idx.data(Qt::EditRole).toString());
			}

			if (idx.column() == HPrice) {
				QString data = idx.data(Qt::EditRole).toString(); 
				double price, tax;
				if (DataParser::price(data, price, tax)) {
					QString var;
					return var.sprintf("%.2f zl", price*(1.0+tax/100.0));
				} else {
					if (role == Qt::BackgroundRole)
						return QColor(Qt::red);
					else
						return QVariant(tr("Parser error!"));
				}
			}

			if (idx.column() == HBook) {
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
				QString data = idx.data(Qt::EditRole).toString();
				QDate date;
				if (DataParser::date(data, date, QDate::fromString(index(idx.row(), HBook).data(Qt::DisplayRole).toString(), "yyyy-MM-dd"))) {
					QString var;
					return date.toString("yyyy-MM-dd");
				} else {
					if (role == Qt::BackgroundRole)
						return QColor(Qt::red);
					else
						return QVariant(tr("Parser error!"));
				}
			}

			if (idx.column() == HStaQty) {
				float used = index(idx.row(), int(HUsedQty)).data().toFloat();
				float total = raw(idx).toFloat();
				float free = total - used;
				QString qty;
				return qty.sprintf("%.2f of %.2f", free, total);
			}
			break;
		case Qt::BackgroundRole:
// 			if (idx.row() % 2) {
// 				return QColor(Qt::magenta);
// 			} else {
// 				return QColor(Qt::cyan);
// 			}
			QString today = QDate::currentDate().toString("yyyy-MM-dd");
			QString expdate = QDate::fromString(index(idx.row(), HExpire).data().toString(), "yyyy-MM-dd").toString("yyyy-MM-dd");
			if (today > expdate) {
				return globals::item_expired;
			} else if (today == expdate) {
				return globals::item_aexpired;
			} else {
				return globals::item_nexpired;
			}
			break;
	}
	return QSqlRelationalTableModel::data(idx, Qt::DisplayRole);
}

/**
 * @brief Zwraca dany przechowywane w roli Qt::DisplayRole
 *
 * @param idx indeks
 * @return QVariant
 **/
QVariant BatchTableModel::raw(const QModelIndex & idx) const {
// 	if (idx.column() == HSpec) {
// 		return QSqlRelationalTableModel::data(this->index(idx.row(), HProdId), Qt::DisplayRole).toString();
// 	}
	return QSqlRelationalTableModel::data(idx, Qt::DisplayRole);
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
	setFilter(filter);
}

void BatchTableModel::trigDataChanged(QModelIndex topleft, QModelIndex bottomright) {
	PR(topleft.row()); PR(topleft.column());
	PR(bottomright.row()); PR(bottomright.column());
}

#include "BatchTableModel.moc"