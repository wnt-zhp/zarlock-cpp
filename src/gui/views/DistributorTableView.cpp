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
#include "DistributorTableView.h"
#include "DistributorTableModel.h"

#include "Database.h"

#include <QHeaderView>

/**
 * @brief Standardowy konstruktor, żaden szał.
 *
 * @param parent niby parent, ale nie wiem po co służy, czasem się ustawia, czesem nie.
 * Obiecuję się doszkolić.
 * Poczytać o signal/slot w Qt
 **/
DistributorTableView::DistributorTableView(QWidget * parent) : AbstractTableView(parent) {
	CI();
	reloadPalette();
}

/**
 * @brief Destruktor. Usuwamy wszystko co stworzone operatorem new.
 *
 **/
DistributorTableView::~DistributorTableView() {
	DI();
}

/**
 * @brief Ta funkcja odpowiada za ustawianie modelu dla widoku (poczytać w qtassistanto o model/view in Qt).
 * Najpierw wywołujemy standarowy QTableView::setModel() a następnie wprowadzamy do naszego widoku małe zmiany.
 * Robimy je tutaja nie w konstruktorze, gdyż konstruktor nie posiada jeszcze żadnych informacji o modelu,
 * więc np.nie możemy ukryć kolumn
 *
 * @param model model z danycmi do wyświetlania
 * @return void
 **/
void DistributorTableView::setModel(QAbstractItemModel * model) {
    QTableView::setModel(model);

// 	hideColumn(DistributorTableModel::HId);
	hideColumn(DistributorTableModel::HEntryDate);
// 	hideColumn(DistributorTableModel::HDistType);
// 	hideColumn(DistributorTableModel::HDistTypeA);
// 	hideColumn(DistributorTableModel::HDistTypeB);

	horizontalHeader()->setResizeMode(DistributorTableModel::HId, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(DistributorTableModel::HBatchId, QHeaderView::Stretch);
	horizontalHeader()->setResizeMode(DistributorTableModel::HQty, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(DistributorTableModel::HDistDate, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(DistributorTableModel::HDistType, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(DistributorTableModel::HDistTypeA, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(DistributorTableModel::HDistTypeB, QHeaderView::ResizeToContents);

	sortByColumn(DistributorTableModel::HId, Qt::AscendingOrder);
	setSortingEnabled(true);

	setAlternatingRowColors(true);
	reloadPalette();
}


#include "DistributorTableView.moc"