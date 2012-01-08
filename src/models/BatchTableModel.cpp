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


BatchTableModel::BatchTableModel(QObject* parent, QSqlDatabase db): QSqlTableModel(parent, db), autosubmit(true) {
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(trigDataChanged()));
}

BatchTableModel::~BatchTableModel() {
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
QVariant BatchTableModel::data(const QModelIndex & idx, int role) const {
	switch (role) {
		case RFreeQty:
			if (idx.column() == HUsedQty) {
				double used = raw(idx).toDouble();
				double total = index(idx.row(), BatchTableModel::HStaQty).data(Qt::EditRole).toDouble();
				return QVariant(total - used);
			}
			break;
		case RNameQty:
			if (idx.column() == HSpec) {
				return QString(
					this->data(this->index(idx.row(), HProdId), Qt::DisplayRole).toString() % " " %
					QSqlTableModel::data(idx, Qt::EditRole).toString() % tr(" (quantity: %1)").arg(this->data(this->index(idx.row(), HStaQty), RFreeQty).toDouble())
				);
			}
			break;
		case Qt::EditRole:
				return raw(idx);
			break;
		case Qt::DisplayRole:
		case Qt::BackgroundRole:
		case Qt::StatusTipRole:
			return display(idx, role);
			break;
		case RRaw:
			return QSqlTableModel::data(idx, Qt::EditRole);
			break;
	}

	int col = idx.column();
	if (role == Qt::TextAlignmentRole and (col == HPrice or col == HUnit or col == HStaQty))
		return Qt::AlignRight + Qt::AlignVCenter;
	if (role == Qt::TextAlignmentRole and (col == HRegDate or col == HExpiryDate))
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
bool BatchTableModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	switch (role) {
		case Qt::EditRole:
			if (index.column() == HPrice) {
				double netto, vat;
				if (!DataParser::price(value.toString(), netto, vat)) {
					inputErrorMsgBox(value.toString());
					return false;
				}
				return QSqlTableModel::setData(index, int(netto*(100+vat)), role);
			}

			if (index.column() == HUnit) {
				QString unitf;
				if (!DataParser::unit(value.toString(), unitf)) {
					inputErrorMsgBox(value.toString());
					return false;
				}
			}

			if (index.column() == HRegDate) {
				return QSqlTableModel::setData(index, value.toDate().toString(Qt::ISODate), Qt::EditRole);
			}

			if (index.column() == HExpiryDate) {
				return QSqlTableModel::setData(index, value.toDate().toString(Qt::ISODate), Qt::EditRole);
			}

			if (index.column() == HStaQty) {
				int used = this->index(index.row(), HUsedQty).data().toInt();
				int total = value.toDouble()*100;
// 				int free = total - used;

				if (total < used) {
					inputErrorMsgBox(value.toString());
					return false;
				}
				return QSqlTableModel::setData(index, total, role);
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
bool BatchTableModel::select() {
	setHeaderData(HId,			Qt::Horizontal, tr("ID"));
	setHeaderData(HProdId,		Qt::Horizontal, tr("Product"));
	setHeaderData(HSpec,		Qt::Horizontal, tr("Specificator"));
	setHeaderData(HPrice,		Qt::Horizontal, tr("Price"));
	setHeaderData(HUnit,		Qt::Horizontal, tr("Unit"));
	setHeaderData(HStaQty,		Qt::Horizontal, tr("Quantity"));
	setHeaderData(HRegDate,		Qt::Horizontal, tr("Booking"));
	setHeaderData(HExpiryDate,		Qt::Horizontal, tr("Expiry"));
	setHeaderData(HUsedQty,		Qt::Horizontal, tr("Used"));
	setHeaderData(HEntryDate,	Qt::Horizontal, tr("Entry date"));
	setHeaderData(HNotes,		Qt::Horizontal, tr("Notes"));
	setHeaderData(HInvoice,		Qt::Horizontal, tr("Invoice"));

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
QVariant BatchTableModel::display(const QModelIndex & idx, const int role) const {
	switch (role) {
		case Qt::DisplayRole:
			if (idx.column() == HProdId) {
				QModelIndexList qmil = Database::Instance().CachedProducts()->match(
										Database::Instance().CachedProducts()->index(0, ProductsTableModel::HId),
										Qt::EditRole, idx.data(RRaw).toInt(), 1, Qt::MatchExactly);
				if (!qmil.isEmpty()) {
					return Database::Instance().CachedProducts()->index(qmil.at(0).row(), ProductsTableModel::HName).data(Qt::DisplayRole);
				}
			}

			else if (idx.column() == HSpec) {
				return QString(
					this->data(this->index(idx.row(), HProdId), Qt::DisplayRole).toString() %
					" " %
					QSqlTableModel::data(idx, Qt::EditRole).toString());
			}

			else if (idx.column() == HPrice) {
				return idx.data(Qt::EditRole).toInt()/100.0;
			}

			else if (idx.column() == HUnit) {
				QString unitf;
				if (DataParser::unit(idx.data(Qt::EditRole).toString(), unitf)) {
					return unitf;
				} else {
					if (role == Qt::BackgroundRole)
						return QColor(Qt::red);
					else
						return QVariant(tr("Parser error!"));
				}
			}

			else if (idx.column() == HRegDate) {
				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
			}

			else if (idx.column() == HExpiryDate) {
				if (idx.data(Qt::EditRole).isNull())
					return QString(new QChar(0x221e), 1);

				return QSqlTableModel::data(idx, Qt::DisplayRole).toDate().toString(Qt::DefaultLocaleShortDate);
// 				QDate date;
// 				if (DataParser::date(idx.data(Qt::EditRole).toString(), date, QSqlTableModel::data(index(idx.row(), HRegDate), Qt::DisplayRole).toDate())) {
// 					return date.toString(Qt::DefaultLocaleShortDate);
// 				} else {
// 					if (role == Qt::BackgroundRole)
// 						return QColor(Qt::red);
// 					else
// 						return QVariant(tr("Parser error!"));
// 				}
			}

			else if (idx.column() == HStaQty) {
				double used = index(idx.row(), int(HUsedQty)).data().toDouble();
				double total = raw(idx).toDouble();
				double free = total - used;
				QString qty;
				return tr("%1 of %2").arg(free/100.0, 0, 'f', 2).arg(total/100.0, 0, 'f', 2);
			}

			else if (idx.column() == HENameQty) {
				return QString(
					this->data(this->index(idx.row(), HProdId), Qt::DisplayRole).toString() % " " %
					QSqlTableModel::data(idx, Qt::EditRole).toString() % tr(" (quantity: %1)").arg(this->data(this->index(idx.row(), HStaQty), RFreeQty).toDouble())
				);
			}
			break;
		case Qt::BackgroundRole:
			QModelIndex expidx = index(idx.row(), BatchTableModel::HExpiryDate);
			QDate expd = QDate::fromString(data(expidx, Qt::DisplayRole).toString(), Qt::DefaultLocaleShortDate);
			int daystoexp = expd.daysTo(QDate::currentDate());

			if (daystoexp > 0) {
				return globals::item_expired;
			} else if (daystoexp == 0) {
				return globals::item_aexpired;
			} else {
				return globals::item_nexpired;
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
QVariant BatchTableModel::raw(const QModelIndex & idx) const {
	if (idx.column() == HRegDate) {
		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate()/*.toString("dd-MM-yyyy")*/;
	} else
	if (idx.column() == HExpiryDate) {
		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate()/*.toString("dd-MM-yyyy")*/;
	} else
	if (idx.column() == HEntryDate) {
		return QSqlTableModel::data(idx, Qt::DisplayRole).toDate()/*.toString("dd-MM-yyyy")*/;
	}

	return QSqlTableModel::data(idx, Qt::DisplayRole);
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

void BatchTableModel::autoSubmit(bool asub) {
	autosubmit = asub;
}

void BatchTableModel::trigDataChanged() {
	if (autosubmit) {
		this->submitAll();
		Database::Instance().updateBatchWordList();
	}
}
 
#include "BatchTableModel.moc"