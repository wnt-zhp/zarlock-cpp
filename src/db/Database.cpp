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

#include <iostream>

#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtGui/QMessageBox>

#include <QProgressDialog>

#include <QFile>
#include <QDir>
#include <QStringBuilder>

#include "Database.h"
#include "DataParser.h"
#include "globals.h"
#include "config.h"

const int Database::plist_size = 3;
const int Database::blist_size = 7;
const int Database::dlist_size = 4;

/**
 * @brief Instancja klasy Database
 **/
Database * Database::dbi = NULL;

/**
 * @brief Tworzy i zwraca instację do obiektu Database
 *
 * @return Database &
 **/
Database & Database::Instance() {
	if (!dbi) {
		dbi = new Database;
		FPR(__func__);
		std::cout << "++ Create Database instance\n";
	}

	return *dbi;
}

void Database::Destroy() {
	if (dbi) {
		delete dbi; dbi = NULL;
		std::cout << "++ Destroy Database instance\n";
	}
}

Database::Database() : QObject(),
	model_products(NULL), model_batch(NULL), model_distributor(NULL),
	model_mealday(NULL), model_meal(NULL),
	plist(QVector<QStringList>(plist_size)), blist(QVector<QStringList>(blist_size)),
	dlist(QVector<QStringList>(dlist_size)), locked(false)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
// 	PR(db.driver()->hasFeature(QSqlDriver::Transactions));
}

Database::Database(const Database & /*db*/) : QObject() {
	FPR(__FUNC__);
}

Database::~Database() {
	FPR(__func__);
	close_database();

// 	QSqlDatabase::removeDatabase("QSQLITE");
}

/**
 * @brief Otwórz bazę danych z pliku
 *
 * @param dbname Nazwa bazy danych
 * @param autoupgrade automatycznie aktualizuj wersję bazy danych
 * @return bool
 **/
bool Database::open_database(const QString & dbname, bool autoupgrade) {
	if (locked) {
		std::cerr << "Database already opened, close it before reopen" << std::endl;
		return false;
	}

	if (!openDBFile(dbname, true)) {// dbfile
		std::cerr << "Unable to find file for database " << dbname.toStdString() << std::endl;

		QMessageBox::critical(0, tr("Cannot create database"),
								tr("Unable to establish a database connection.\n"
								"This application needs SQLite support. Please read "
								"the Qt SQL driver documentation for information how "
								"to build it.\n\n"
								"Click Close to exit."), QMessageBox::Close);
		return false;
	}

	// do upgrade
	QSqlQuery qdbv("SELECT value FROM settings WHERE key='dbversion';");

	// TODO temporary constrain
	autoupgrade = false;

	unsigned int dbversion;
	if (qdbv.next())
		dbversion = qdbv.value(0).toUInt();
	else {
		QMessageBox msgBox;
		msgBox.setText(tr("Your database \"%1\" is corrupted. Unable to find version number.").arg(dbname));
		msgBox.setInformativeText(tr("This is critical error and if your database has important data, "
									"please contact with Zarlok team in WNT GK to help you solve your problem."));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();

		std::cerr << "Database " << dbname.toStdString() << " is corrupted. Unable to find DB database version!\n";
		return false;
	}

	if (dbversion > DBVERSION) {
		QMessageBox msgBox;
		msgBox.setText(tr("Your database \"%1\" is in version %2 and is newer than supported version %3.").arg(dbname).arg(dbversion).arg(DBVERSION));
		msgBox.setInformativeText(tr("Please upgrade Zarlok to the newest version."));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		return false;
	}

	if (dbversion != DBVERSION) {
		if (!autoupgrade) {
			QMessageBox msgBox;
			msgBox.setText(tr("You have been asked for upgrade of database \"%1\" from version %2 to version %3.").arg(dbname).arg(dbversion).arg(DBVERSION));
			msgBox.setInformativeText(tr("Do you want to upgrade?"));
			msgBox.setIcon(QMessageBox::Question);
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::Yes);

			if (msgBox.exec() == QMessageBox::Yes)
				autoupgrade = true;
		}
		if (autoupgrade)
		if (!doDBUpgrade(dbversion)) {
			QMessageBox msgBox;
			msgBox.setText(tr("Database \"%1\" upgrade failed.").arg(dbname).arg(dbversion).arg(DBVERSION));
			msgBox.setInformativeText(tr("This is critical error and if your database has important data, "
									"please contact with Zarlok team in WNT GK to help you solve your problem."));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setStandardButtons(QMessageBox::Ok);
			msgBox.setDefaultButton(QMessageBox::Ok);
			msgBox.exec();

			std::cerr << "Database upgrade failed!\n";
			return false;
		}
	}

	// products
	model_products = new ProductsTableModel;
	model_products->setTable("products");
	model_products->setEditStrategy(QSqlTableModel::OnManualSubmit);

	// batch
	model_batch = new BatchTableModel;
	model_batch->setTable("batch");
	model_batch->setEditStrategy(QSqlTableModel::OnManualSubmit);

	model_distributor = new DistributorTableModel;
	model_distributor->setTable("distributor");
	model_distributor->setEditStrategy(QSqlTableModel::OnManualSubmit);

	model_mealday = new MealDayTableModel;
	model_mealday->setTable("meal_day");
	model_mealday->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_mealday->setSort(MealDayTableModel::HMealDate, Qt::AscendingOrder);

	model_meal = new MealTableModel;
	model_meal->setTable("meal");
	model_meal->setEditStrategy(QSqlTableModel::OnManualSubmit);
// 	model_meal->setSort(MealTableModel::HDistDate, Qt::AscendingOrder);

	camp = new CampProperties;
	camp->readCampSettings();

	if (!camp->isCorrect)
		camp->campName = dbname;

// 	connect(model_products, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(rebuild_models()));
// 	connect(model_batch, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(rebuild_models()));
// 	connect(model_distributor, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(rebuild_models()));

	locked = true;

	opened_db = dbname;

	return rebuild_models();
}

/**
 * @brief ...
 *
 * @return bool
 **/
bool Database::close_database() {
	save_database();

	if (locked) {
		if (model_products) delete model_products;
		if (model_batch) delete model_batch;
		if (model_distributor) delete model_distributor;
		if (model_mealday) delete model_mealday;
		if (model_meal) delete model_meal;
		if (camp) delete camp;
	}
	locked = false;

	if (db.isOpen())
		db.close();

	opened_db.clear();

	return true;
}

void Database::save_database() {
	model_products->submitAll();
	model_distributor->submitAll();

	model_batch->submitAll();

	model_mealday->submitAll();
	model_meal->submitAll();

	emit dbSaved();
}

/**
 * @brief ...
 *
 * @return bool
 **/
bool Database::rebuild_models() {
	// products
	if (!model_products->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), model_products->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// batch
	if (!model_batch->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), model_batch->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// distributor
	if (!model_distributor->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), model_distributor->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// meal
	if (!model_mealday->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), model_mealday->lastError().text(), QMessageBox::Abort);
		return false;
	}
	if (!model_meal->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), model_meal->lastError().text(), QMessageBox::Abort);
		return false;
	}

	updateProductsWordList();
	updateBatchWordList();
	updateDistributorWordList();

	return true;
}

bool Database::execQueryFromFile(const QString& resource) {
	QSqlQuery query;

	if (!db.isOpen())
		return false;

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		db.transaction();

	QFile dbresfile(resource);   // TODO: Fix it later
	if (!dbresfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}
	while (!dbresfile.atEnd()) {
		QString line = dbresfile.readLine();
		if (line.trimmed().isEmpty())
			continue;

// 		query.exec(line.fromUtf8(line.toStdString().c_str()));
		if (!query.exec(line.fromUtf8(line.toStdString().c_str()))) {
			PR(query.lastError().text().toStdString());
			PR(query.lastQuery().toStdString());
		} else {
// 			PR(query.lastQuery().toStdString());
		}
	}
	query.finish();

	if (db.driver()->hasFeature(QSqlDriver::Transactions)) {
		if (!db.commit()) {
			db.rollback();
			return false;
		}
	}

	return true;
}

/**
 * @brief Otwórz plik z bazą danych
 *
 * @param dbname Nazwa bazy danych
 * @param createifnotexists Jeślu true automatycznie tworzy bazę danych jeśli nie istnieje
 * @return bool true jeśli poprawnie otwarto plik bazy danych
 **/
bool Database::openDBFile(const QString & dbname, bool createifnotexists) {
	if (dbname.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name is empty"));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return false;
	}

	QString safename = dbname;
	safename.replace(QRegExp(QString::fromUtf8("[^a-zA-Z0-9_]")), "_");

	QString dbfilenoext = QDir::homePath() % QString(ZARLOK_HOME ZARLOK_DB) % safename;
	QString dbfile = dbfilenoext % ".db";

	QFile fdbfile(dbfile);

	int create = QMessageBox::No;

	if (fdbfile.exists()) {
		db.setDatabaseName(dbfile);
		return db.open();
	}

	if (!createifnotexists) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name \"%1\" doesn't exists!").arg(dbname));
		msgBox.setInformativeText(tr("Do you want to create new database with name \"%1\"?").arg(dbname));
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		create = msgBox.exec();
		if (create != QMessageBox::Yes)
			return false;
	}	

	if (createDBFile(dbname, dbfilenoext)) {
		db.setDatabaseName(dbfile);
		if (!db.open())
			return false;

		execQueryFromFile(":/resources/database_00000301.sql");
// TODO: Fix it later
		execQueryFromFile(":/resources/test_data_3100.sql");

	}

	return false;
}

/**
 * @brief Tworzy plik bazy danych i inicjuje strukturę bazy danych
 *
 * @param dbname Nazwa bazy danych
 * @param dbfilenoext Nazwa (bez rozszerzenia) pliku bazy danych
 * @return bool true jeśli poprawnie stworzono nowy plik i zainicjowano bazę danych
 **/
bool Database::createDBFile(const QString & dbname, const QString & dbfilenoext) {
	QDir dbsavepath(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	QString datafile = dbfilenoext % ".db";
	QString infofile = dbfilenoext % ".info";

	QFile file(datafile);
	if (file.exists()) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name '%1' already exists.").arg(dbname));
		msgBox.setInformativeText(tr("Do you want to overwrite existing database?"));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);

		if (msgBox.exec() == QMessageBox::No)
			return false;
	}
		
	file.open(QIODevice::ReadWrite | QIODevice::Truncate);
	file.close();

	file.setFileName(infofile);
	file.open(QIODevice::ReadWrite | QIODevice::Truncate);
	file.write(dbname.toUtf8());
	file.close();

	return true;
}

// void Database::updateBatchQty() {
// 	if (db.driver()->hasFeature(QSqlDriver::Transactions))
// 		db.transaction();
// 
// 	QSqlQuery qBatch("UPDATE batch SET used_qty=(SELECT sum(quantity) FROM distributor WHERE batch_id=batch.id);");
// 	qBatch.exec();
// 
// 	if (db.driver()->hasFeature(QSqlDriver::Transactions))
// 		if (!db.commit())
// 			db.rollback();
// 
// 	model_batch->select();
// }

// void Database::updateBatchQty(const int bid) {
// 	QSqlQuery q;
//         q.prepare("SELECT SUM(quantity) FROM distributor WHERE batch_id=?;");
// 	q.bindValue(0, bid);
// 	q.exec();
// 
// 	int qty = 0;
// 	if (q.next())
//         qty = q.value(0).toInt();
// 
// 	q.prepare("UPDATE batch SET used_qty=? WHERE id=?;");
// 	q.bindValue(0, qty);
// 	q.bindValue(1, bid);
// 	q.exec();
// }

// void Database::updateMealCosts() {
// 	QSqlQuery q;
// // 	q.exec("UPDATE meal_day SET avcosts=(SELECT (sum(d.quantity*b.price)/(m.scouts+m.leaders+m.others)/10000.0) "
// // 		   "FROM meal AS m, distributor AS d, batch as b WHERE m.mealday=meal_day.id AND d.disttype_a=m.id AND b.id=d.batch_id);");
// // 	q.exec("UPDATE meal_day SET avcosts=CAST(round((SELECT (sum(d.quantity*b.price)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal AS m WHERE mealday=meal_day.id)/100) "
// // 			"FROM distributor AS d, batch as b WHERE d.disttype_a=meal_day.id AND d.batch_id=b.id)) AS INTEGER);");
// 	q.exec("UPDATE meal_day SET avcosts=round(CAST((SELECT sum(d.quantity*b.price) FROM batch AS b, distributor AS d, meal AS m WHERE b.id=d.batch_id AND d.disttype=2 AND d.disttype_a=m.id AND m.mealday=meal_day.id) AS DOUBLE)/"
// 			"(SELECT sum(m.scouts+m.leaders+m.others) FROM meal as m WHERE mealday=meal_day.id)) WHERE id=(SELECT md.id FROM meal_day AS md, meal AS m, distributor AS d WHERE d.disttype=2 AND m.id=d.disttype_a AND md.id=m.mealday);");
// 	model_mealday->select();
// }

// void Database::updateMealCosts(const QModelIndex& idx) {
// 	int mdid = idx.model()->data(idx.model()->index(idx.row(), MealDayTableModel::HId), Qt::EditRole).toInt();
// 	QSqlQuery q;
// 	q.prepare("UPDATE meal_day SET avcosts=CAST(round((SELECT (sum(d.quantity*b.price)/(SELECT sum(m.scouts+m.leaders+m.others) FROM meal AS m WHERE mealday=meal_day.id)/100) "
// 			"FROM distributor AS d, batch as b WHERE d.disttype_a=m.id AND b.id=d.batch_id)) AS INTEGER) WHERE id=?;");
// 	q.bindValue(0, mdid);
// 	q.exec();
// 
// 	model_mealday->select();
// }

void Database::updateProductsWordList() {
	QSqlQuery qsq("SELECT name, unit, expire FROM products;");

	while(qsq.next()) {
		for (int i = 0; i < plist_size; ++i) {
			plist[i] << qsq.value(i).toString();
		}
	}

	for (int i = 0; i < plist_size; ++i)
		plist[i].removeDuplicates();

	emit productsWordListUpdated();
}

void Database::updateBatchWordList() {
	QSqlQuery qsq("SELECT spec, unit, price, start_qty, invoice_no, booking, expire FROM batch;");

	while(qsq.next()) {
		for (int i = 0; i < blist_size; ++i) {
			blist[i] << qsq.value(i).toString();
		}
	}

	for (int i = 0; i < blist_size; ++i)
		blist[i].removeDuplicates();

	emit batchWordListUpdated();
}

void Database::updateDistributorWordList() {
	QSqlQuery qsq("SELECT quantity, distdate, reason, reason2, reason3 FROM distributor;");

	while(qsq.next()) {
		for (int i = 0; i < dlist_size; ++i) {
			if ((i < 2) or (qsq.value(4).toInt() != DistributorTableModel::RMeal))
				dlist[i] << qsq.value(i).toString();
		}
	}

	for (int i = 0; i < dlist_size; ++i)
		dlist[i].removeDuplicates();

	emit distributorWordListUpdated();
}

/** @brief Ta funkcja zawiera aktualizacje wersji baz danych. Funkcja powinna być wywoływana rekurencyjnie.
 *  @param version wersja bazy danych do aktualizacji
 *  @return zwraca true jeśliaktualiazcja zakończona sukcesem, w przeciwnym wypadku zmienna version zawiera
 * numer ostatniej wersji dla której aktualizacja zakończyla się sukcesem.
 */
bool Database::doDBUpgrade(unsigned int version) {
	QSqlQuery qdbup, q;
	QString str;
	QProgressDialog progress(tr("Updating database..."), tr("&Cancel"), 0, 0);
	int pos = 0;

	switch (version) {
		case dbv_INIT:
			PR("Upgrade from 0");
			str = "UPDATE settings SET value='%1' WHERE key='dbversion';";
			qdbup.exec(str.arg(dbv_AUG11));
			return doDBUpgrade((unsigned int)dbv_AUG11);
			break;
		case dbv_AUG11:
			PR("Upgrade to"); PR(dbv_JAN12);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);
			progress.setCancelButton(NULL);

			q.exec("SELECT count(id) FROM batch;");			
			progress.setMaximum(5+q.value(0).toInt());

			progress.setValue(pos++);
			execQueryFromFile(":/resources/dbconv_00000030_00000301_part_a.sql");
			progress.setValue(pos++);
			execQueryFromFile(":/resources/database_00000301.sql");
			progress.setValue(pos++);
			execQueryFromFile(":/resources/dbconv_00000030_00000301_part_b.sql");
			progress.setValue(pos++);

			q.exec("SELECT id,start_qty,used_qty,expirydate,price,regdate FROM batch;");

			if (db.driver()->hasFeature(QSqlDriver::Transactions))
				db.transaction();
			while (q.next()) {
				progress.setValue(pos++);
				int bid = q.value(0).toInt();
				QString expdate = q.value(3).toString();
				QString price = q.value(4).toString();
				QString regdate = q.value(5).toString();

				double netto, vat;
				DataParser::price(price, netto, vat);

				QDate regdate_n, expdate_n, entrydate_n;
				regdate_n = QDate::fromString(regdate, Qt::ISODate);
				DataParser::date(expdate, expdate_n, regdate_n);

				qdbup.prepare("UPDATE batch SET expirydate=?,price=? WHERE id=?;");/*start_qty=?,used_qty=?,*/
				qdbup.bindValue(0, expdate_n.toString(Qt::ISODate));
				qdbup.bindValue(1, int(netto*(vat+100)));
				qdbup.bindValue(2, bid);
				qdbup.exec();
				qdbup.finish();
			}
			q.finish();
			if (db.driver()->hasFeature(QSqlDriver::Transactions)) {
				if (!db.commit()) {
					db.rollback();
					return false;
				}
			}
			progress.setValue(pos++);

// 			while (q.isActive() or qdbup.isActive()) {}
			execQueryFromFile(":/resources/dbconv_00000030_00000301_part_c.sql");
			progress.setValue(pos++);

			return true;

		case dbv_JAN12:
			PR("Database up-to-date!");
			return true;
	}
	return false;
}

bool Database::addProductsRecord(const QString& name, const QString& unit, const QString& expiry, const QString & notes) {
	bool status = true;

	model_products->autoSubmit(false);
	int row = model_products->rowCount();
	status &= model_products->insertRows(row, 1);
	status &= model_products->setData(model_products->index(row, ProductsTableModel::HName), name);
	status &= model_products->setData(model_products->index(row, ProductsTableModel::HUnit), unit);
	status &= model_products->setData(model_products->index(row, ProductsTableModel::HExpire), expiry);
	status &= model_products->setData(model_products->index(row, ProductsTableModel::HNotes), notes);
	model_products->autoSubmit(true);

	if (!status) {
		model_products->revertAll();
		return false;
	}

	status = model_products->submitAll();
	if (status)
		updateProductsWordList();
	
	return status;
}

bool Database::updateProductsRecord(int pid, const QString& name, const QString& unit, const QString& expiry, const QString & notes) {
	bool status = true;

	model_products->autoSubmit(false);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HName), name);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HUnit), unit);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HExpire), expiry);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HName), notes);
	model_products->autoSubmit(true);

	if (!status) {
		model_products->revertAll();
		return false;
	}

	status = model_products->submitAll();
	if (status)
		updateProductsWordList();

	return status;
}

bool Database::removeProductsRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;

	int counter = 0;
	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == ProductsTableModel::HName) {
				QVariant pid = model_products->index(idxl.at(i).row(), ProductsTableModel::HId).data();
				QString prod = model_products->index(idxl.at(i).row(), ProductsTableModel::HName).data().toString();
				++counter;
				details = details % prod % "\n";
				QModelIndexList batches = model_batch->match(model_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
				for (int i = 0; i < batches.count(); ++i) {
					details = details % "  \\--  " % model_batch->data(model_batch->index(batches.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
				}
			}
		}
		status = model_products->productRemoveConfirmation(counter, details);
	}

	if (askForConfirmation & !status )
		return false;

// 	removeBatchRecord(bat, false);

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == ProductsTableModel::HName) {
			if (!model_products->removeRow(idxl.at(i).row())) {
				model_products->revertAll();
				return false;
			}
		}
	}
	status = model_products->submitAll();

	return status;
}

bool Database::addBatchRecord(unsigned int pid, const QString& spec, const QString& price, const QString& unit, double qty, double used, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	bool status = true;
TD
TM
	int row = model_batch->rowCount();

	model_batch->autoSubmit(false);
	status &= model_batch->insertRows(row, 1);
// 	model_batch->setData(model_batch->index(row, BatchTableModel::HId), row);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HProdId), pid);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HSpec), spec);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HPrice), price);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUnit), unit);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HStaQty), qty);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUsedQty), used);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HRegDate), reg);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HExpiryDate), expiry);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HEntryDate), entry);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HInvoice), invoice);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HNotes), notes);
	model_batch->autoSubmit(true);

	if (!status) {
		model_batch->revertAll();
		return false;
	}

	status = model_batch->submitAll();
	if (status)
		updateBatchWordList();
TM
	return status;
}

bool Database::getBatchRecord(const QModelIndex & idx, unsigned int& pid, QString& spec, QString& price, QString& unit, double& qty, double& used, QDate& reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes) {
	int row = idx.row();
	pid			= model_batch->index(row, BatchTableModel::HProdId).data().toUInt();
	spec		= model_batch->index(row, BatchTableModel::HSpec).data(Qt::EditRole).toString();
	price		= model_batch->index(row, BatchTableModel::HPrice).data().toString();
	unit		= model_batch->index(row, BatchTableModel::HUnit).data().toString();
	qty			= model_batch->index(row, BatchTableModel::HStaQty).data(Qt::EditRole).toDouble()/100.0;
	used		= model_batch->index(row, BatchTableModel::HUsedQty).data().toDouble()/100.0;
	reg			= model_batch->index(row, BatchTableModel::HRegDate).data(Qt::EditRole).toDate();
	expiry		= model_batch->index(row, BatchTableModel::HExpiryDate).data(Qt::EditRole).toDate();
	entry		= model_batch->index(row, BatchTableModel::HEntryDate).data(Qt::EditRole).toDate();
	invoice		= model_batch->index(row, BatchTableModel::HInvoice).data().toString();
	notes		= model_batch->index(row, BatchTableModel::HNotes).data().toString();
	return true;
}

/** Updates Batch record in database. Change on read-only member used_qty is not allowed
 * @param idx record index
 * @param pid product ID
 * @param spec product specification
 * @param price price (int value, currency unit is "grosz")
 * @param unit batch unit
 * @param qty quantity
 * @param reg date of registration in storage
 * @param expiry date of product expiry
 * @param entry date of database entry insert or last modification
 * @param invoice invoice number
 * @param notes extra notes
 * @return result of record update
 **/

bool Database::updateBatchRecord(const QModelIndex & idx, unsigned int pid, const QString& spec, const QString& price, const QString& unit, double qty, /*double used,*/ const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	bool status = true;

	int row = idx.row();

	model_batch->autoSubmit(false);
// 	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HId), row);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HProdId), pid);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HSpec), spec);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HPrice), price);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUnit), unit);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HStaQty), qty);
// 	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUsedQty), int(used*100));
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HRegDate), reg);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HExpiryDate), expiry);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HEntryDate), entry);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HInvoice), invoice);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HNotes), notes);
	model_batch->autoSubmit(true);

	if (!status) {
		model_batch->revertAll();
		return false;
	}
	
	status = model_batch->submitAll();
	if (status)
		updateBatchWordList();

	return status;
}

bool Database::removeBatchRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;
	int counter = 0;

	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == BatchTableModel::HProdId) {
				++counter;
				details = details % model_batch->data(model_batch->index(idxl.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
			}
		}
		status = model_batch->batchRemoveConfirmation(counter, details);
	}

	if (askForConfirmation & !status )
		return false;

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == BatchTableModel::HProdId) {
			status = model_batch->removeRow(idxl.at(i).row());
		}
	}
	return (status && model_batch->submitAll() && rebuild_models());
}

bool Database::addDistributorRecord(unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b, bool autoupdate) {
GTD
GTM
	QModelIndexList qmil = model_batch->match(model_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
	if (qmil.count() != 1) {
		return false;
	}
	int bidrow = qmil.at(0).row();

	int used = Database::Instance().CachedBatch()->index(bidrow, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
	int total = Database::Instance().CachedBatch()->index(bidrow, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
	int fake = 0;

	int free = total - used + fake;

	if (free < qty) {
// 			inputErrorMsgBox(value.toString());
		return false;
	}
GTM
	QSqlQuery q;
	q.prepare("INSERT INTO distributor VALUES (NULL, ?, ?, ?, ?, ?, ?, ?);");
	q.bindValue(0, bid);
	q.bindValue(1, qty);
	q.bindValue(2, ddate.toString(Qt::ISODate));
	q.bindValue(3, rdate.toString(Qt::ISODate));
	q.bindValue(4, disttype);
	q.bindValue(5, dt_a);
	q.bindValue(6, dt_b);
	bool status = q.exec();
GTM
	if (autoupdate) {
// 		updateBatchQty(bid);
		model_batch->select();
GTM
		model_distributor->select();
GTM
		updateDistributorWordList();
	}
GTM	
	return status;

// 	bool status = true;
// GTD
// GTM
// 	int row = model_distributor->rowCount();
// 
// 	model_distributor->autoSubmit(false);
// 	status &= model_distributor->insertRows(row, 1);
// 	// 	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HId), row);
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HBatchId), bid);
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HQty), int(qty));
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistDate), ddate.toString(Qt::ISODate));
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HEntryDate), rdate.toString(Qt::ISODate));
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistType), disttype);
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistTypeA), dt_a);
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistTypeB), dt_b);
// 
// 	model_distributor->autoSubmit(true);
// GTM
// 	if (!status) {
// 		model_distributor->revertAll();
// 		return false;
// 	}
// GTM
// 	status = model_distributor->submitAll();
// 	// 	if (status)
// 	// 		updateBatchWordList();
// GTM
// 	return status;
}

bool Database::updateDistributorRecord(const QModelIndex & idx, unsigned int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b) {
	bool status = true;
// GTD
// GTM
// 	QSqlQuery q;
// 	q.prepare("UPDATE distributor SET batch_id=?, quantity=?, distdate=?, registered=?, reason=?, reason2=?, reason3=? WHERE id=?;");
// 	q.bindValue(0, bid);
// 	q.bindValue(1, int(qty*100));
// 	q.bindValue(2, ddate.toString(Qt::ISODate));
// 	q.bindValue(3, rdate.toString(Qt::ISODate));
// 	q.bindValue(4, re1);
// 	q.bindValue(5, re2);
// 	q.bindValue(6, re3);
// 	q.bindValue(7, model_distributor->index(idx.row(), DistributorTableModel::HId).data().toInt());
// 	q.exec();
// 
// GTM
// updateBatchQty(model_distributor->index(idx.row(), DistributorTableModel::HBatchId).data(DistributorTableModel::RRaw).toInt());
// GTM
// 	return status;
	unsigned int row = idx.row();
	model_distributor->autoSubmit(false);
	// 	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HId), row);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HBatchId), bid);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HQty), int(qty));
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistDate), ddate.toString(Qt::ISODate));
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HEntryDate), rdate.toString(Qt::ISODate));
// 	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistType), disttype);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistTypeA), dt_a);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistTypeB), dt_b);

	model_distributor->autoSubmit(true);

	if (!status) {
		model_distributor->revertAll();
		return false;
	}

	status = model_distributor->submitAll();
// 	if (status)
// 		updateBatchWordList();

	return status;
}

bool Database::removeDistributorRecord(const QModelIndexList & idxl, bool askForConfirmation, bool submitBatches) {EGTD
	bool status = true;
	int counter = 0;
	QString details;
	QMap<int, int> idmap;
GTM
	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == DistributorTableModel::HBatchId) {
				++counter;
				details = details % model_distributor->data(model_distributor->index(idxl.at(i).row(), DistributorTableModel::HBatchId), Qt::DisplayRole).toString() % "\n";
			}
		}
		status = model_batch->distributeRemoveConfirmation(counter, details);
	}

	if (!status)
		return false;
GTM
	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == DistributorTableModel::HBatchId) {
			int bid = model_distributor->index(idxl.at(i).row(), DistributorTableModel::HBatchId).data(DistributorTableModel::RRaw).toInt();
			if (model_distributor->removeRow(idxl.at(i).row()))
				idmap[bid]=1;
		}
	}
GTM
	status = model_distributor->submitAll();
// 	if (status && submitBatches) {
// 		QMap<int, int>::iterator it = idmap.begin();
// 		while(it != idmap.end()) {
// 			updateBatchQty((it++).key());
// 		}
// 		status &= model_batch->submitAll();
// 	}
GTM
	return status;
}

bool Database::removeDistributorRecord(int recordid, bool askForConfirmation, bool submitBatches) {EGTD
	QString batch;
	QVariant bid = model_distributor->index(recordid, DistributorTableModel::HBatchId).data(DistributorTableModel::RRaw);
	bool status = true;
GTM
	if (askForConfirmation) {
		QModelIndexList batchl = model_batch->match(model_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
		if (batchl.count())
			batch = model_batch->data(model_batch->index(batchl.at(0).row(), BatchTableModel::HSpec)).toString();
		status = model_distributor->distributeRemoveConfirmation(1, batch);
	}
GTM
	if (status) {GTM
		if (!model_distributor->removeRow(recordid)) {
			model_distributor->revertAll();
			return false;
		}GTM
		status = model_distributor->submitAll();GTM
		/*updateBatchQty(bid.toInt());*/GTM
// 		if (submitBatches) model_batch->submitAll();
		model_batch->select();
		GTM
	}GTM
	return status;
}

bool Database::getDistributorRecord(const QModelIndex & idx, unsigned int & bid, unsigned int & qty, QDate & distdate, QDate & entrydate, DistributorTableModel::Reasons & disttype, QString & disttypea, QString & disttypeb) {
	GTD
	GTM
	int row = idx.row();
	bid			= model_distributor->index(row, DistributorTableModel::HBatchId).data().toUInt();
	qty			= model_distributor->index(row, DistributorTableModel::HQty).data(Qt::EditRole).toUInt();
	distdate	= model_distributor->index(row, DistributorTableModel::HDistDate).data(Qt::EditRole).toDate();
	entrydate	= model_distributor->index(row, DistributorTableModel::HEntryDate).data(Qt::EditRole).toDate();
	disttype	= (DistributorTableModel::Reasons)model_distributor->index(row, DistributorTableModel::HDistType).data(Qt::EditRole).toInt();
	disttypea	= model_distributor->index(row, DistributorTableModel::HDistTypeA).data(Qt::EditRole).toString();
	disttypeb	= model_distributor->index(row, DistributorTableModel::HDistTypeB).data(Qt::EditRole).toString();
// PR(model_batch->index(row, DistributorTableModel::HDistDate).data(Qt::EditRole).toDate().toString().toStdString().c_str());
// PR(model_batch->index(row, DistributorTableModel::HDistDate).data(Qt::DisplayRole).toDate().toString().toStdString().c_str());
	GTM	
	return true;
}

/** @brief Insert MealDay record to database
 * @param date meal day
 **/
bool Database::addMealDayRecord(const QDate & mealday, int avgcost) {
	bool status = true;

	int row = model_mealday->rowCount();
PR(row);
	model_mealday->autoSubmit(false);
	status &= model_mealday->insertRows(row, 1);
	status &= model_mealday->setData(model_mealday->index(row, MealDayTableModel::HMealDate), mealday.toString(Qt::ISODate));
	status &= model_mealday->setData(model_mealday->index(row, MealDayTableModel::HAvgCost), avgcost);
	model_mealday->autoSubmit(true);
	
	if (!status) {
		model_mealday->revertAll();
		return false;
	} else
		return model_mealday->submitAll();
}

bool Database::updateMealDayRecord(const QModelIndex & idx, const QDate & mealday, int avgcost) {
	bool status = true;
	unsigned int row = idx.row();
	model_distributor->autoSubmit(false);
	// 	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HId), row);
	status &= model_meal->setData(model_mealday->index(row, MealDayTableModel::HMealDate), mealday.toString(Qt::ISODate));
	status &= model_meal->setData(model_mealday->index(row, MealDayTableModel::HAvgCost), avgcost);	
	model_distributor->autoSubmit(true);
	
	if (!status) {
		model_meal->revertAll();
		return false;
	}
	
	status = model_meal->submitAll();
	// 	if (status)
	// 		updateBatchWordList();
	
	return status;
}

bool Database::removeMealDayRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;
	
	int counter = 0;
	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == MealDayTableModel::HId) {
// 				QVariant mdid = model_mealday->index(idxl.at(i).row(), MealDayTableModel::HId).data();
// 				QString mdday = model_mealday->index(idxl.at(i).row(), MealDayTableModel::HMealDate).data().toDate().toString(Qt::DefaultLocaleShortDate);
// 				++counter;
// 				details = details % mdday % "\n";
// 				QModelIndexList mkinds = model_mealkind->match(model_mealkind->index(0, MealKindTableModel::HMealDate), Qt::EditRole, mdid, -1);
// 				for (int i = 0; i < mkinds.count(); ++i) {
// 					details = details % "  \\--  " % model_batch->data(model_batch->index(mkinds.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
// 				}
				// 				bat.append(batches);
			}
		}
		status = model_mealday->productRemoveConfirmation(counter, details);
	}
	
// 	if (askForConfirmation & !status )
// 		return false;
// 	
// 	// 	QSqlQuery q;
// 		// 	q.prepare("DELETE FROM meals WHERE id=?;");
// 		
// 		// 	removeBatchRecord(bat, false);
// 		// 	if (db.driver()->hasFeature(QSqlDriver::Transactions))
// 		// 		db.transaction();
// 		
// 		for (int i = 0; i < idxl.count(); ++i) {
// 			if (idxl.at(i).column() == ProductsTableModel::HName) {
// 				if (!model_mealday->removeRow(idxl.at(i).row())) {
// 					model_mealday->revertAll();
// 					return false;
// 				}
// 			}
// 		}
// 		
// 		// 	if (db.driver()->hasFeature(QSqlDriver::Transactions))
// 		// 		if (!db.commit())
// 		// 			db.rollback();
		
		status = model_mealday->submitAll();
		
		return status;
}

bool Database::getMealDayRecord(const QModelIndex & idx, unsigned int & mdid, QDate & mealday, int & avgcost) {
	GTD
	GTM
	int row = idx.row();
	mdid		= model_mealday->index(row, MealDayTableModel::HId).data().toUInt();
	mealday		= model_mealday->index(row, MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();
	avgcost		= model_mealday->index(row, MealDayTableModel::HAvgCost).data(Qt::EditRole).toInt();
	GTM	
	return true;
}

bool Database::addMealRecord(int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes) {
	bool status = true;

	int row = model_meal->rowCount();

	model_meal->autoSubmit(false);
	status &= model_meal->insertRows(row, 1);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealDay), mealday);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealKind), mealkind);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealName), name);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HScouts), scouts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HLeaders), leaders);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HOthers), others);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HAvgCosts), avgcosts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HNotes), notes);
	model_meal->autoSubmit(true);

	if (!status) {
		model_meal->revertAll();
		return false;
	} else
		return model_meal->submitAll();
}

bool Database::updateMealRecord(int mid, int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes) {
	bool status = true;

	int row = mid;
	model_meal->autoSubmit(false);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealDay), mealday);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealKind), mealkind);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HMealName), name);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HScouts), scouts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HLeaders), leaders);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HOthers), others);
// 	status &= model_meal->setData(model_meal->index(row, MealTableModel::HAvgCosts), avgcosts);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HNotes), notes);
	model_meal->autoSubmit(true);

	if (!status) {
		model_meal->revertAll();
		return false;
	}

	return model_meal->submitAll();
}

bool Database::removeMealRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;

	int counter = 0;
	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == ProductsTableModel::HName) {
				QVariant pid = model_meal->index(idxl.at(i).row(), ProductsTableModel::HId).data();
				QString prod = model_meal->index(idxl.at(i).row(), ProductsTableModel::HName).data().toString();
				++counter;
				details = details % prod % "\n";
				QModelIndexList batches = model_batch->match(model_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
				for (int i = 0; i < batches.count(); ++i) {
					details = details % "  \\--  " % model_batch->data(model_batch->index(batches.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
				}
// 				bat.append(batches);
			}
		}
		status = model_meal->productRemoveConfirmation(counter, details);
	}

	if (askForConfirmation & !status )
		return false;

// 	QSqlQuery q;
// 	q.prepare("DELETE FROM meals WHERE id=?;");

// 	removeBatchRecord(bat, false);
// 	if (db.driver()->hasFeature(QSqlDriver::Transactions))
// 		db.transaction();

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == ProductsTableModel::HName) {
			
			if (!model_meal->removeRow(idxl.at(i).row())) {
				model_meal->revertAll();
				return false;
			}
		}
	}

// 	if (db.driver()->hasFeature(QSqlDriver::Transactions))
// 		if (!db.commit())
// 			db.rollback();

	status = model_meal->submitAll();

	return status;
}

#include "Database.moc"
