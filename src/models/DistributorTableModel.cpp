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
DistributorTableModel::DistributorTableModel(QObject* parent, QSqlDatabase db): QSqlTableModel(parent, db) {
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(trigDataChanged(QModelIndex,QModelIndex)));
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
	if (role == Qt::StatusTipRole)
		return raw(idx);

	if (role == Qt::EditRole or role == Qt::DisplayRole or role == Qt::BackgroundRole)
		return display(idx, role);

	int col = idx.column();
	if (role == Qt::TextAlignmentRole and (col == HQty))
		return Qt::AlignRight + Qt::AlignCenter;
	if (role == Qt::TextAlignmentRole and (col == HDistDate))
		return Qt::AlignCenter;

	return QSqlTableModel::data(idx, role);
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
				QDate date;
				if (DataParser::date(value.toString(), date)) {
					return QSqlTableModel::setData(index, date.toString(Qt::ISODate), Qt::EditRole);
				} else {
					inputErrorMsgBox(value.toString());
					return false;
				}
			}

			if (index.column() == HRegDate) {
				QDate date;
				if (!DataParser::date(value.toString(), date)) {
					inputErrorMsgBox(value.toString());
					return false;
				}
			}

			if (index.column() == HQty) {
				BatchTableModel * btm = Database::Instance().CachedBatch();
				int bidrow = -1;

				QModelIndexList qmil = btm->match(btm->index(0, BatchTableModel::HId), Qt::DisplayRole, this->index(index.row(), HBatchId).data(Qt::EditRole));
				if (!qmil.count()) {
					return false;
				}
				bidrow = qmil.at(0).row();

				float used = Database::Instance().CachedBatch()->index(bidrow, BatchTableModel::HUsedQty).data().toFloat();
	
// 				float used = ->index()
// 				this->index(index.row(), HUsedQty).data().toFloat();
				float total = Database::Instance().CachedBatch()->index(bidrow, BatchTableModel::HStaQty).data(Qt::EditRole).toFloat();
				float fake = index.data().toFloat();

				float free = total - used + fake;
				if (free < value.toFloat()) {
					inputErrorMsgBox(value.toString());
					return false;
				}
				return QSqlTableModel::setData(index, value, role);
			}
			break;
	}
    return QSqlTableModel::setData(index, value, role);
}

/**
 * @brief Ta funkcja odpowiada za pobranie danych z tabeli i synchronizację.
 * Tutaj możemy ustawić nagłówki dla kolumn
 *
 * @return bool stan otwarcia tabeli
 **/
bool DistributorTableModel::select() {
	setHeaderData(HId,		Qt::Horizontal, tr("ID"));
	setHeaderData(HBatchId,	Qt::Horizontal, tr("Batch"));
	setHeaderData(HQty,		Qt::Horizontal, tr("Quantity"));
	setHeaderData(HDistDate,Qt::Horizontal, tr("Distributing date"));
	setHeaderData(HRegDate,	Qt::Horizontal, tr("Registered"));
	setHeaderData(HReason,	Qt::Horizontal, tr("Main reason"));
	setHeaderData(HReason2,	Qt::Horizontal, tr("Sub reason"));
	setHeaderData(HReason3,	Qt::Horizontal, tr("Distribution type"));

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
QVariant DistributorTableModel::display(const QModelIndex & idx, const int role) const {
	switch (role) {
		case Qt::EditRole:
			if (idx.column() == HDistDate) {
				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
			}
			break;
		case Qt::DisplayRole:
			if (idx.column() == HBatchId) {
				QModelIndexList qmil = Database::Instance().CachedBatch()->match(Database::Instance().CachedBatch()->index(0, BatchTableModel::HId), Qt::EditRole, idx.data(Qt::EditRole));
				if (!qmil.isEmpty()) {
					return Database::Instance().CachedBatch()->index(qmil.first().row(), 2).data(Qt::DisplayRole);
				}
			}

			if (idx.column() == HRegDate) {
				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
			}
			
			if (idx.column() == HDistDate) {
				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
			}
			break;
		case Qt::BackgroundRole:
			break;
	}
	return QSqlTableModel::data(idx, Qt::DisplayRole);
}

/**
 * @brief Zwraca dany przechowywane w roli Qt::DisplayRole
 *
 * @param idx indeks
 * @return QVariant
 **/
QVariant DistributorTableModel::raw(const QModelIndex & idx) const {
// 	if (idx.column() == HSpec) {
// 		return QSqlTableModel::data(this->index(idx.row(), HProdId), Qt::DisplayRole).toString();
// 	}
	return QSqlTableModel::data(idx, Qt::DisplayRole);
}

/**
 * @brief Tworzy filtr na pole 'name' dla danych z tabeli products.
 * Filtr ma postać 'f*'.
 *
 * @param f filtr
 **/
void DistributorTableModel::filterDB(const QString & f) {
	QString filter = "";
	if (!f.isEmpty())
		filter = "spec GLOB '" % f % "*'";
	setFilter(filter);
}

void DistributorTableModel::trigDataChanged(QModelIndex topleft, QModelIndex bottomright) {
// 	if (submitAll()) {;
// 		revertAll();
// 		return;
// 	}
	this->submitAll();
	if (topleft.column() == HQty) {
		for (int i = topleft.row(); i <= bottomright.row(); ++i)
			Database::Instance().updateBatchQty(index(i, HBatchId).data(Qt::EditRole).toInt());

		Database::Instance().CachedBatch()->submitAll();
	}
}

#include "DistributorTableModel.moc"