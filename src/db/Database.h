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

#include "CampProperties.h"

/**
 * @brief Klasa obsługuję całą komunikację z bazą danych oraz tworzenie/otwieranie/zamykanie. Jest singletonem.
 **/
class Database : public QObject {
Q_OBJECT
public:
	static Database & Instance();
	static void Destroy();
	virtual ~Database();
private:
	Database();
	Database(const Database &);

public:
	bool open_database(const QString& dbname, bool autoupgrade = true);
	bool close_database();
	void save_database();

	void updateBatchQty();
	void updateBatchQty(const int);

	void updateMealCosts();
	void updateMealCosts(const int);

	void writeCampSettings();
	void readCampSettings();

	virtual bool addProductsRecord(const QString& name, const QString& unit, const QString & expiry, const QString & notes);
	virtual bool updateProductsRecord(int pid, const QString& name, const QString& unit, const QString & expiry, const QString & notes);
// 	virtual bool removeProductsRecord(int recordid, bool askForConfirmation = true);
// 	virtual bool removeProductsRecord(const QModelIndex & idx, bool askForConfirmation = true);
	virtual bool removeProductsRecord(const QModelIndexList & idxl, bool askForConfirmation = true);

	virtual bool addBatchRecord(int pid, const QString& spec, const QString& book, const QString& reg, const QString& expiry, float qty, float used, const QString& unit, const QString& price, const QString& invoice, const QString& notes);
	virtual bool updateBatchRecord(int bid, int pid, const QString& spec, const QString& book, const QString& reg, const QString& expiry, float qty, float used, const QString& unit, const QString& price, const QString& invoice, const QString& notes);
// 	virtual bool removeBatchRecord(int recordid, bool askForConfirmation = true);
// 	virtual bool removeBatchRecord(const QModelIndex & idx, bool askForConfirmation = true);
	virtual bool removeBatchRecord(const QModelIndexList & idxl, bool askForConfirmation = true);

	virtual bool addDistributorRecord(int, float, const QString&, const QString&, const QString&, const QString&, DistributorTableModel::Reasons);
	virtual bool updateDistributorRecord(int, int, float, const QString&, const QString&, const QString&, const QString&, DistributorTableModel::Reasons);
	virtual bool removeDistributorRecord(int recordid, bool askForConfirmation = true, bool submitBatches = true);
// 	virtual bool removeDistributorRecord(const QModelIndex & idx, bool askForConfirmation = true, bool submitBatches = true);
	virtual bool removeDistributorRecord(const QModelIndexList & idxl, bool askForConfirmation = true, bool submitBatches = true);
	
	inline ProductsTableModel * CachedProducts() { return tab_products; }
	inline BatchTableModel * CachedBatch() { return tab_batch; }
	inline DistributorTableModel * CachedDistributor() { return tab_distributor; }

	inline CampProperties * cs() { return camp; };

private:
	bool openDBFile(const QString& dbname, bool createifnotexists = false);
	bool createDBFile(const QString& dbname, const QString& dbfilenoext);

signals:
	void databaseDirty();
	void dbSaved();

private slots:
	bool rebuild_models();

private:
	static Database * dbi;
	QSqlDatabase db;

	CampProperties * camp;

	ProductsTableModel * tab_products;
	BatchTableModel * tab_batch;
	DistributorTableModel * tab_distributor;

	bool doDBUpgrade(unsigned int & version);

	bool locked;

	static const unsigned int DBVERSION = 0x000030;
};

#endif // DATABASE_H
