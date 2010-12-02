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


#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>

#include "ProductsTableModel.h"
#include "BatchTableModel.h"

/**
 * @brief Klasa obsługuję całą komunikację z bazą danych oraz tworzenie/otwieranie/zamykanie. Jest singletonem.
 **/
class Database {
public:
	static Database & Instance();
	virtual ~Database();
private:
	Database();
	Database(const Database &) {};

public:
	bool open_database(const QString & dbfile, bool recreate = false);
	bool close_database();

	ProductsTableModel * CachedProducts() { return tab_products; }
	BatchTableModel * CachedBatch() { return tab_batch; }

private:
	bool rebuild_models();

private:
	static Database * dbi;
	QSqlDatabase db;
	ProductsTableModel * tab_products;
	BatchTableModel * tab_batch;
};

#endif // DATABASE_H
