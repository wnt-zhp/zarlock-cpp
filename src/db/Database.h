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
#include "DistributorTableModel.h"

/**
 * @brief Klasa obsługuję całą komunikację z bazą danych oraz tworzenie/otwieranie/zamykanie. Jest singletonem.
 **/
class Database : public QObject {
Q_OBJECT
public:
	static Database & Instance();
	virtual ~Database();
private:
	Database();
	Database(const Database &) : QObject(NULL) {};

public:
	bool open_database(const QString & dbfile, bool recreate = false);
	bool close_database();
	void save_database();

	void updateBatchQty();
	void updateBatchQty(const int);

	void updateMealCosts();
	void updateMealCosts(const int);

	inline ProductsTableModel * CachedProducts() { return tab_products; }
	inline BatchTableModel * CachedBatch() { return tab_batch; }
	inline DistributorTableModel * CachedDistributor() { return tab_distributor; }

signals:
	void databaseDirty();
	void dbSaved();

private slots:
	bool rebuild_models();

private:
	static Database * dbi;
	QSqlDatabase db;

	ProductsTableModel * tab_products;
	BatchTableModel * tab_batch;
	DistributorTableModel * tab_distributor;
};

#endif // DATABASE_H
