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
#include <QStyle>
#include <QStringBuilder>

#include "MealDayTableModel.h"
#include "DataParser.h"
#include "Database.h"

MealDayTableModel::MealDayTableModel(QObject* parent, QSqlDatabase db): QSqlTableModel(parent, db), autosubmit(true) {
// 	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(trigDataChanged()));
}

MealDayTableModel::~MealDayTableModel() {
	FPR(__func__);
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
QVariant MealDayTableModel::data(const QModelIndex & idx, int role) const {
	if (role == Qt::BackgroundRole) {
		QDate d = QSqlTableModel::data(idx, Qt::DisplayRole).toDate();
		Database * db = Database::Instance();
		if ((d > db->cs()->campDateBegin) and (d < db->cs()->campDateEnd))
			return Qt::yellow;
	}

	if (role == Qt::TextAlignmentRole and idx.column() == HAvgCost)
			return Qt::AlignRight + Qt::AlignVCenter;

	if (role == Qt::DisplayRole) {
		return display(idx, role);
	}

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
bool MealDayTableModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	switch (role) {
		case Qt::DisplayRole:
			if (index.column() == HMealDate) {
				QDate date;
				if (!DataParser::date(value.toString(), date)) {
					inputErrorMsgBox(value.toString());
					return false;
				}
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
bool MealDayTableModel::select() {
	setHeaderData(HId,			Qt::Horizontal, tr("ID"));
	setHeaderData(HMealDate,	Qt::Horizontal, tr("Date"));
	setHeaderData(HAvgCost,		Qt::Horizontal, tr("Avg Costs"));

	if (!QSqlTableModel::select())
		return false;
	while (canFetchMore())
		fetchMore();

	return true;
}

/**
 * @brief Kiedy rządamy wyświetlenia danych, niektóre z nich muszą zostać sparsowane.
 * Korzystamy ze statycznych metod klasy DataParse
 *
 * @param idx indeks z daną do sparsowania
 * @param role przypisana rola
 * @return QVariant dana po parsowaniu i standaryzacji
 **/
QVariant MealDayTableModel::display(const QModelIndex & idx, const int role) const {
	switch (role) {
		case Qt::DisplayRole:
			if (idx.column() == HMealDate) {
				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::LocalDate);
			}
			if (idx.column() == HAvgCost) {
				return QSqlTableModel::data(idx, Qt::DisplayRole).toDouble()/10000.0;
			}
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
QVariant MealDayTableModel::raw(const QModelIndex & idx) const {
	if (idx.column() == HMealDate) {
		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
	}

	return QSqlTableModel::data(idx, Qt::DisplayRole);
}

void MealDayTableModel::autoSubmit(bool asub) {
	autosubmit = asub;
}

#include "MealDayTableModel.moc"