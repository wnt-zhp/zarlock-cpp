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

#include <iostream>
#include <algorithm>

#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtGui/QMessageBox>

#include <QProgressDialog>

#include <QFile>
#include <QDir>
#include <QStringBuilder>

#include<QDebug>

#include "Database.h"
#include "DataParser.h"
#include "MealManager.h"
#include "globals.h"
#include "config.h"

const int Database::plist_size = 3;
const int Database::blist_size = 7;
const int Database::dlist_size = 4;

const QString Database::dbfilext = ".db";
const QString Database::infofilext = ".info";

/**
 * @brief Instancja klasy Database
 **/
Database * Database::dbi = NULL;

/**
 * @brief Tworzy i zwraca instację do obiektu Database
 *
 * @return Database &
 **/
Database * Database::Instance() {
	if (!dbi) {
		dbi = new Database;
		FI();
		std::cout << "++ Create Database instance\n";
	}

	return dbi;
}

void Database::Destroy() {
	if (dbi) {
		delete dbi; dbi = NULL;
		std::cout << "++ Destroy Database instance\n";
	}
}

Database::Database() : QObject(), camp(NULL),
	model_products(NULL), model_batch(NULL), model_distributor(NULL),
	model_mealday(NULL), model_meal(NULL),
	plist(QVector<QStringList>(plist_size)), blist(QVector<QStringList>(blist_size)),
	dlist(QVector<QStringList>(dlist_size)), locked(false)
{
	CI();
	db = QSqlDatabase::addDatabase("QSQLITE");

	connect(this, SIGNAL(modelsRebuild()), MealManager::Instance(), SLOT(reinit()));
}

Database::Database(const Database & /*db*/) : QObject() {
	CI();
}

Database::~Database() {
	DI();
	close_database();
	db.removeDatabase("QSQLITE");
}

QSqlDriver* Database::driver() const {
	return db.driver();
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
	if (qdbv.next()) {
		dbversion = qdbv.value(0).toUInt();
	} else {
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
// 	model_batch->setEditStrategy(QSqlTableModel::OnManualSubmit);

	model_distributor = new DistributorTableModel;
	model_distributor->setTable("distributor");
// 	model_distributor->setEditStrategy(QSqlTableModel::OnManualSubmit);

	model_mealday = new MealDayTableModel;
	model_mealday->setTable("meal_day");
	model_mealday->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_mealday->setSort(MealDayTableModel::HMealDate, Qt::AscendingOrder);

	model_meal = new MealTableModel;
	model_meal->setTable("meal");
	model_meal->setEditStrategy(QSqlTableModel::OnManualSubmit);
// 	model_meal->setSort(MealTableModel::HDistDate, Qt::AscendingOrder);

	delete camp;
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

bool Database::create_database(const QString& dbname) {
	return createDBFile(dbname);
}

bool Database::delete_database(const QString& dbname) {
	if (dbname.isEmpty()) {
		return false;
	}

	QString dbfilenoext = fileFromDBName(dbname);
	QString datafile = dbfilenoext % dbfilext;
	QString infofile = dbfilenoext % infofilext;

	QFile fdatafile(datafile);

	if (fdatafile.exists()) {
		fdatafile.remove();
	}

	QFile finfofile(infofile);

	if (finfofile.exists()) {
		finfofile.remove();
	}

	return false;
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
// PR(1);	model_products->submitAll();
// PR(1);	model_batch->submitAll();
// PR(1);	model_distributor->submitAll();
// PR(1);	model_mealday->submitAll();
// PR(1);	model_meal->submitAll();

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
// 		QMessageBox::critical(0, QObject::tr("Database error"), model_batch->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// distributor
	if (!model_distributor->select()) {
// 		QMessageBox::critical(0, QObject::tr("Database error"), model_distributor->lastError().text(), QMessageBox::Abort);
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

	emit modelsRebuild();

	return true;
}

bool Database::execQueryFromFile(const QString& resource) {
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

		if (line.startsWith("--"))
			continue;

		QSqlQuery query;

		if (!query.exec(line.fromUtf8(line.toStdString().c_str()))) {
// 			PR(query.lastError().text().toStdString());
// 			PR(query.lastQuery().toStdString());
			qDebug() << "Failed to create table:" << query.lastError();
		} else {
// 			PR(query.lastQuery().toStdString());
		}
	}

	if (db.driver()->hasFeature(QSqlDriver::Transactions)) {
		if (!db.commit()) {
			db.rollback();
			return false;
		}
	}
	return true;
}

QString Database::fileFromDBName(const QString& dbname, bool fullpath, bool adddbext) {
	QString safename = dbname;
	safename.replace(QRegExp(QString::fromUtf8("[^a-zA-Z0-9_]")), "_");

	if (adddbext)
		safename.append(dbfilext);

	if (fullpath)
		return QDir::homePath() % QString(ZARLOK_HOME ZARLOK_DB) % safename;
	else
		return safename;
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

	QString dbfile = fileFromDBName(dbname) % dbfilext;

	QFile fdbfile(dbfile);

	if (fdbfile.exists()) {
		QString bp = QDir::homePath() % QString(ZARLOK_HOME ZARLOK_DB) % "/backups/";
		QDir bpd(bp);
		if (!bpd.exists(bp))
			bpd.mkpath(bp);
		fdbfile.copy(bp % "/" % fileFromDBName(dbname, false) % "_backup_" % QDateTime::currentDateTime().toString("yyyyMMdd_hhmmsszzz") % dbfilext);
		db.setDatabaseName(dbfile);
		return db.open();
	}

	int create = QMessageBox::No;
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

	return createDBFile(dbname);
}

/**
 * @brief Tworzy plik bazy danych i inicjuje strukturę bazy danych
 *
 * @param dbname Nazwa bazy danych
 * @param dbfilenoext Nazwa (bez rozszerzenia) pliku bazy danych
 * @return bool true jeśli poprawnie stworzono nowy plik i zainicjowano bazę danych
 **/
bool Database::createDBFile(const QString & dbname) {
	QDir dbsavepath(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	QString dbfilenoext = fileFromDBName(dbname);
	QString datafile = dbfilenoext % dbfilext;
	QString infofile = dbfilenoext % infofilext;

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

	return createDBStructure(datafile);
}

bool Database::createDBStructure(const QString& dbfile) {
	db.setDatabaseName(dbfile);
	if (!db.open())
		return false;

	return execQueryFromFile(":/resources/database_00000301.sql");
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
			execQueryFromFile(":/resources/dbconv_00000301_00000302_part_a.sql");
			return true;

		case dbv_JUL12:
			PR("Database up-to-date!");
			return true;
	}
	return false;
}

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

bool Database::addProductRecord(const QString& name, const QString& unit, const QString& expiry, const QString & notes) {
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

bool Database::updateProductRecord(int pid, const QString& name, const QString& unit, const QString& expiry, const QString & notes) {
	bool status = true;

	model_products->autoSubmit(false);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HName), name);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HUnit), unit);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HExpire), expiry);
	status &= model_products->setData(model_products->index(pid, ProductsTableModel::HNotes), notes);
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

bool Database::removeProductRecord(const QVector<int> & ids, bool /*askForConfirmation*/) {
	bool status = false;
	QString details;

// 	qSort(ids);

	int rowsnum = ids.count();
	int counter = 0;

	for (int i = 0; i < rowsnum; ++i) {
		QVariant pid = ids.at(i);
		QModelIndexList idx = model_products->match(model_products->index(0, ProductsTableModel::HId), Qt::EditRole, pid, 1, Qt::MatchExactly);

		QString prod = model_products->index(idx.at(0).row(), ProductsTableModel::HName).data().toString();
		++counter;
		details = details % prod % "\n";
		QModelIndexList batches = model_batch->match(model_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
		for (int i = 0; i < batches.count(); ++i) {
			details = details % "  \\--  " % model_batch->data(model_batch->index(batches.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
		}
	}
	status = model_products->productRemoveConfirmation(counter, details);

	if (status ) {
		for (int i = 0; i < rowsnum; ++i) {
			QModelIndexList idx = model_products->match(model_products->index(0, ProductsTableModel::HId), Qt::EditRole, ids.at(i), 1, Qt::MatchExactly);
			if (!model_products->removeRow(idx.at(0).row())) {
				model_products->revertAll();
				return false;
			}
		}

		status = model_products->submitAll();
	}

	return status;
}

bool Database::getProductRecord(int row, int& pid, QString& name, QString& unit, QString& expiry, QString& notes) {
	pid			= model_products->index(row, ProductsTableModel::HId).data(Qt::EditRole).toUInt();
	name		= model_products->index(row, ProductsTableModel::HName).data(Qt::EditRole).toString();
	unit		= model_products->index(row, ProductsTableModel::HUnit).data(Qt::EditRole).toString();
	expiry		= model_products->index(row, ProductsTableModel::HExpire).data(Qt::EditRole).toString();
	notes		= model_products->index(row, ProductsTableModel::HNotes).data(Qt::EditRole).toString();

	return true;
}

bool Database::addBatchRecord(int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	if (model_batch->addRecord(pid, spec, price, unit, qty, reg, expiry, entry, invoice, notes)) {
		updateBatchWordList();
		return true;
	}
	return false;
}

bool Database::getBatchRecord(int row, int& pid, QString& spec, int & price, QString& unit, int & qty, int & used, QDate& reg, QDate& expiry, QDate& entry, QString& invoice, QString& notes) {
	return model_batch->getRecord(row, pid, spec, price, unit, qty, used, reg, expiry, entry, invoice, notes);
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

bool Database::updateBatchRecord(int row, int pid, const QString& spec, int price, const QString& unit, int qty, const QDate& reg, const QDate& expiry, const QDate& entry, const QString& invoice, const QString& notes) {
	bool status = true;

	if (model_batch->updateRecord(row, pid, spec, price, unit, qty, reg, expiry, entry, invoice, notes)) {
		updateBatchWordList();
		return true;
	}

	return status;
}

bool Database::removeBatchRecord(const QVector<int> & ids, bool askForConfirmation) {
	bool status = false;
	QString details;

	int counter = ids.count();

// 	qSort(rows);

	if (askForConfirmation) {
		for (int i = 0; i < counter; ++i) {
			details = details % model_batch->index(model_batch->getRowById(ids.at(i)), BatchTableModel::HSpec).data(Qt::DisplayRole).toString() % "\n";
		}
		status = model_batch->batchRemoveConfirmation(counter, details);
	}

	if (askForConfirmation & !status )
		return false;

	for (int i = 0; i < counter; ++i) {
		status = model_batch->removeRow(model_batch->getRowById(ids.at(i)));
	}

	return status;
}

bool Database::addDistributorRecord(int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b) {
	QModelIndexList qmil = model_batch->match(model_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
	if (qmil.count() != 1) {
		return false;
	}
	int bidrow = qmil.at(0).row();

	int used = this->CachedBatch()->index(bidrow, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
	int total = this->CachedBatch()->index(bidrow, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
	int fake = 0;

	int free = total - used + fake;

	if (free < qty) {
// 			inputErrorMsgBox(value.toString());
		return false;
	}

	if (model_distributor->addRecord(bid, qty, ddate, rdate, disttype, dt_a, dt_b)) {
		model_batch->selectRow(model_batch->getRowById(bid));
		updateDistributorWordList();
		return true;
	}

	return false;
}

bool Database::updateDistributorRecord(int row, int bid, int qty, const QDate & ddate, const QDate & rdate, int disttype, const QString & dt_a, const QString & dt_b) {
	QModelIndexList qmil = model_batch->match(model_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
	if (qmil.count() != 1) {
		return false;
	}
	int bidrow = qmil.at(0).row();

	int used = this->CachedBatch()->index(bidrow, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
	int total = this->CachedBatch()->index(bidrow, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
	int fake = this->CachedDistributor()->index(row, DistributorTableModel::HQty).data(Qt::EditRole).toInt();

	int free = total - used + fake;

	if (free < qty) {
		return false;
	}

// 	int id = this->CachedDistributor()->index(row, DistributorTableModel::HId).data(Qt::EditRole).toInt();

	int oldbid = model_distributor->index(row, DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
	bool status = model_distributor->updateRecord(row, bid, qty, ddate, rdate, disttype, dt_a, dt_b);

	if (status) {
		model_batch->selectRow(model_batch->getRowById(bid));
		if (oldbid != bid)
			model_batch->selectRow(model_batch->getRowById(oldbid));
		updateDistributorWordList();
	}

	return status;
}

bool Database::removeDistributorRecord(const QVector<int> & ids, bool askForConfirmation) {
	bool status = true;
	QString details;

	int counter = ids.count();

	if (askForConfirmation) {
		for (int i = 0; i < counter; ++i) {
			details = details % model_distributor->index(model_distributor->getRowById(ids.at(i)), DistributorTableModel::HBatchId).data(Qt::DisplayRole).toString() % "\n";
		}
		status = model_batch->distributeRemoveConfirmation(counter, details);
	}

	if (!status)
		return false;

	for (int i = 0; i < counter; ++i) {
		model_distributor->removeRecord(model_distributor->getRowById(ids.at(i)));
	}

	return status;
}

bool Database::getDistributorRecord(int row, int& bid, int& qty, QDate& distdate, QDate& entrydate, int& disttype, QString& dt_a, QString& dt_b) {
	bid			= model_distributor->index(row, DistributorTableModel::HBatchId).data(Qt::EditRole).toUInt();
	qty			= model_distributor->index(row, DistributorTableModel::HQty).data(Qt::EditRole).toUInt();
	distdate	= model_distributor->index(row, DistributorTableModel::HDistDate).data(Qt::EditRole).toDate();
	entrydate	= model_distributor->index(row, DistributorTableModel::HEntryDate).data(Qt::EditRole).toDate();
	disttype	= model_distributor->index(row, DistributorTableModel::HDistType).data(Qt::EditRole).toInt();
	dt_a		= model_distributor->index(row, DistributorTableModel::HDistTypeA).data(Qt::EditRole).toString();
	dt_b		= model_distributor->index(row, DistributorTableModel::HDistTypeB).data(Qt::EditRole).toString();

	return true;
}

QString Database::getLastExecutedQuery(const QSqlQuery& query) {
	QString str = query.lastQuery();
	QMapIterator<QString, QVariant> it(query.boundValues());

	str.replace("?", "\"?\"");
	while (it.hasNext()) {
		it.next();
		int idx = str.indexOf('?');
		if (idx != -1) {
			str.replace(idx, 1, it.value().toString());
		}
	}
	return str;
}

#include "Database.moc"

// |1| QSqlDriver::Transactions					0	Whether the driver supports SQL transactions.
// |0| QSqlDriver::QuerySize					1	Whether the database is capable of reporting the size of a query. Note that some databases do not support returning the size (i.e. number of rows returned) of a query, in which case QSqlQuery::size() will return -1.
// |1| QSqlDriver::BLOB							2	Whether the driver supports Binary Large Object fields.
// |1| QSqlDriver::Unicode						3	Whether the driver supports Unicode strings if the database server does.
// |1| QSqlDriver::PreparedQueries				4	Whether the driver supports prepared query execution.
// |0| QSqlDriver::NamedPlaceholders			5	Whether the driver supports the use of named placeholders.
// |1| QSqlDriver::PositionalPlaceholders		6	Whether the driver supports the use of positional placeholders.
// |1| QSqlDriver::LastInsertId					7	Whether the driver supports returning the Id of the last touched row.
// |0| QSqlDriver::BatchOperations				8	Whether the driver supports batched operations, see QSqlQuery::execBatch()
// |1| QSqlDriver::SimpleLocking				9	Whether the driver disallows a write lock on a table while other queries have a read lock on it.
// |1| QSqlDriver::LowPrecisionNumbers			10	Whether the driver allows fetching numerical values with low precision.
// |0| QSqlDriver::EventNotifications			11	Whether the driver supports database event notifications.
// |1| QSqlDriver::FinishQuery					12	Whether the driver can do any low-level resource cleanup when QSqlQuery::finish() is called.
// |0| QSqlDriver::MultipleResultSets			13	Whether the driver can access multiple result sets returned from batched statements or stored procedures.