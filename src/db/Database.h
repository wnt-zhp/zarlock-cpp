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
#include <QVector>
#include <QStringList>

#include "ProductsTableModel.h"
#include "BatchTableModel.h"
#include "DistributorTableModel.h"
#include "MealDayTableModel.h"
#include "MealTableModel.h"

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

// 	void updateBatchQty();
// 	void updateBatchQty(const int);

// 	void updateMealCosts();
// 	void updateMealCosts(const QModelIndex& idx);

	void updateProductsWordList();
	void updateBatchWordList();
	void updateDistributorWordList();

	virtual bool addProductsRecord(const QString& name, const QString& unit, const QString & expiry, const QString & notes);
	virtual bool updateProductsRecord(int pid, const QString& name, const QString& unit, const QString & expiry, const QString & notes);
	virtual bool removeProductsRecord(const QModelIndexList & idxl, bool askForConfirmation = true);

	virtual bool addBatchRecord(unsigned int pid, const QString& spec, const QString& price, const QString& unit, double qty, double used, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes);
	virtual bool updateBatchRecord(const QModelIndex & idx, unsigned int pid, const QString& spec, const QString& price, const QString& unit, double qty, /*double used,*/ const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes);
	virtual bool removeBatchRecord(const QModelIndexList & idxl, bool askForConfirmation = true);
	virtual bool getBatchRecord(const QModelIndex & idx, unsigned int & pid, QString& spec, QString& price, QString& unit, double & qty, double & used, QDate&reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes);

	virtual bool addDistributorRecord(unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b, bool autoupdate = true);
	virtual bool updateDistributorRecord(const QModelIndex & idx, unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b);
	virtual bool removeDistributorRecord(const QModelIndexList & idxl, bool askForConfirmation = true, bool submitBatches = true);
	virtual bool getDistributorRecord(const QModelIndex & idx, unsigned int & bid, unsigned int & qty, QDate & distdate, QDate & entrydate, DistributorTableModel::Reasons & disttype, QString & disttypea, QString & disttypeb);

	virtual bool addMealDayRecord(const QDate & mealday, int avgcost);
	virtual bool updateMealDayRecord(const QModelIndex & idx, const QDate & mealday, int avgcost);
	virtual bool removeMealDayRecord(const QModelIndexList & idxl, bool askForConfirmation);
	virtual bool getMealDayRecord(const QModelIndex & idx, unsigned int & mdid, QDate & mealday, int & avgcost);

	virtual bool addMealRecord(int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes);
	virtual bool updateMealRecord(int mid, int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes);
	virtual bool removeMealRecord(const QModelIndexList & idxl, bool askForConfirmation = true);

	inline ProductsTableModel * CachedProducts() { return model_products; }
	inline BatchTableModel * CachedBatch() { return model_batch; }
	inline DistributorTableModel * CachedDistributor() { return model_distributor; }
	inline MealDayTableModel * CachedMealDay() { return model_mealday; }
	inline MealTableModel * CachedMeal() { return model_meal; }

	inline const QVector<QStringList> & ProductsWordList()		{ return plist; }
	inline const QVector<QStringList> & BatchWordList()			{ return blist; }
	inline const QVector<QStringList> & DistributorWordList()	{ return dlist; }

	inline CampProperties * cs() { return camp; };
	inline const QString & openedDatabase() { return opened_db; }

private:
	bool openDBFile(const QString& dbname, bool createifnotexists = false);
	bool createDBFile(const QString& dbname, const QString& dbfilenoext);

signals:
	void databaseDirty();
	void dbSaved();

private slots:
	bool rebuild_models();

signals:
	void productsWordListUpdated();
	void batchWordListUpdated();
	void distributorWordListUpdated();

public:
	static const int plist_size;
	static const int blist_size;
	static const int dlist_size;
	enum ProductsWL		{ PWname, PWunit, PWexpire };
	enum BatchWL		{ BWspec, BWunit, BWprice, BWqty, BWinvoice, BWbooking, BWexpire };
	enum DistributorWL	{ DWqty, DWdist, DWreason, DWoptional };

private:
	static Database * dbi;
	QSqlDatabase db;

	CampProperties * camp;

	ProductsTableModel * model_products;
	BatchTableModel * model_batch;
	DistributorTableModel * model_distributor;
	MealDayTableModel * model_mealday;
	MealTableModel * model_meal;

	QVector<QStringList> plist;
	QVector<QStringList> blist;
	QVector<QStringList> dlist;

	bool doDBUpgrade(unsigned int version);
	bool execQueryFromFile(const QString & resource);

	bool locked;

	QString opened_db;

	enum DBVERSIONS {	dbv_INIT	= 0x0000000,
						dbv_AUG11	= 0x0000030,
						dbv_JAN12	= 0x0000301
	};
	static const unsigned int DBVERSION = dbv_JAN12;
};

#endif // DATABASE_H
