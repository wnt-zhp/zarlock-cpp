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


#ifndef PRODUCTSTABLEVIEW_H
#define PRODUCTSTABLEVIEW_H

#include <QtGui/QTableView>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

#include "AbstractTableView.h"

/**
 * @brief Klasa dziedziczy po QTableView i odpowiada za wyświetlanie
 * naszych danych z tabeli 'products'. Ta klasa jest przykładem, jak można
 * w standardowym widoku tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class ProductsTableView : public AbstractTableView {
Q_OBJECT
public:
	ProductsTableView(QWidget * parent = NULL);
	virtual ~ProductsTableView();

	virtual void setModel(QAbstractItemModel* model);
};

#endif // PRODUCTSTABLEVIEW_H
