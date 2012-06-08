/*
    This file is part of Zarlok.

    Copyright (C) 2012  Rafał Lalik <rafal.lalik@zhp.net.pl>

    Zarlok is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zarlok is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "globals.h"
#include "ProductsTableView.h"
#include "ProductsTableModel.h"

#include <QHeaderView>

/**
 * @brief Standardowy konstruktor, żaden szał.
 *
 * @param parent niby parent, ale nie wiem po co służy, czasem się ustawia, czesem nie.
 * Obiecuję się doszkolić.
 * Poczytać o signal/slot w Qt
 **/
ProductsTableView::ProductsTableView(QWidget * parent) : AbstractTableView(parent) {
	CI();
	reloadPalette();
}

/**
 * @brief Destruktor. Usuwamy wszystko co stworzone operatorem new.
 *
 **/
ProductsTableView::~ProductsTableView() {
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
void ProductsTableView::setModel(QAbstractItemModel * model) {
    QTableView::setModel(model);

// 	hideColumn(ProductsTableModel::HId);
	hideColumn(ProductsTableModel::HNotes);

	horizontalHeader()->setResizeMode(ProductsTableModel::HId, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(ProductsTableModel::HName, QHeaderView::Stretch);
	horizontalHeader()->setResizeMode(ProductsTableModel::HUnit, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(ProductsTableModel::HExpire, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(ProductsTableModel::HNotes, QHeaderView::ResizeToContents);

	sortByColumn(ProductsTableModel::HId, Qt::AscendingOrder);
	setSortingEnabled(true);

	reloadPalette();
}

#include "ProductsTableView.moc"