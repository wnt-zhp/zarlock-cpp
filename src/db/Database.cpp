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
	model_products(NULL), model_batch(NULL), model_distributor(NULL), model_meal(NULL),
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
		msgBox.setText(tr("Your database \"%1\" is corrupted. Unable to find version number.").arg(dbname).arg(dbversion).arg(DBVERSION));
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

	model_meal = new MealTableModel;
	model_meal->setTable("meal");
	model_meal->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_meal->setSort(MealTableModel::HDistDate, Qt::AscendingOrder);

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

// 	updateBatchQty();
	model_batch->submitAll();

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
	if (!model_meal->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), model_meal->lastError().text(), QMessageBox::Abort);
		return false;
	}

	updateProductsWordList();
	updateBatchWordList();
	updateDistributorWordList();

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

		QSqlQuery query;

		if (db.driver()->hasFeature(QSqlDriver::Transactions))
			db.transaction();

		QFile dbresfile(":/resources/database_3100.sql");   // TODO: Fix it later
		if (!dbresfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return false;
		}
		while (!dbresfile.atEnd()) {
			QString line = dbresfile.readLine();
			query.exec(line.fromUtf8(line.toStdString().c_str()));
		}
// TODO: Fix it later
		QFile dbtestfile(":/resources/test_data_3100.sql");
		if (!dbtestfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return false;
		}
		while (!dbtestfile.atEnd()) {
			QString line = dbtestfile.readLine();
			query.exec(line.fromUtf8(line.toStdString().c_str()));
		}

		QString qdbv("INSERT INTO settings VALUES('dbversion', '%1');");
		query.exec(qdbv.arg(DBVERSION));

		if (db.driver()->hasFeature(QSqlDriver::Transactions))
			if (!db.commit())
				db.rollback();


// 		updateBatchQty();

		return true;
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

void Database::updateBatchQty() {
	int maxval = model_batch->rowCount()+1;

	QProgressDialog progress(tr("Synchronizing quantity..."), tr("&Cancel"), 0, maxval);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(NULL);

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		db.transaction();

	int i = 0;
	QSqlQuery qBatch("SELECT id FROM batch;");
	qBatch.exec();
	while (qBatch.next()) {
		progress.setValue(i++);
		updateBatchQty(qBatch.value(0).toInt());
	}

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		if (!db.commit())
			db.rollback();

	progress.setValue(i);
	model_batch->select();
	progress.setValue(i+1);
}

void Database::updateBatchQty(const int bid) {
	QSqlQuery q;
        q.prepare("SELECT SUM(quantity) FROM distributor WHERE batch_id=?;");
	q.bindValue(0, bid);
	q.exec();

	int qty = 0;
	if (q.next())
        qty = q.value(0).toInt();

	q.prepare("UPDATE batch SET used_qty=? WHERE id=?;");
	q.bindValue(0, qty);
	q.bindValue(1, bid);
	q.exec();
}

void Database::updateMealCosts() {
	int maxval = model_meal->rowCount()+1;

	QProgressDialog progress(tr("Synchronizing costs..."), tr("&Cancel"), 0, maxval);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setCancelButton(NULL);

	for (int i = 0; i < model_meal->rowCount(); ++i) {
		progress.setValue(i++);
		updateMealCosts(model_meal->index(i, MealTableModel::HId));
	}

	progress.setValue(maxval-1);
	PR(model_meal->submitAll());
	progress.setValue(maxval);
}

void Database::updateMealCosts(const QModelIndex& idx) {

	int mid = model_meal->index(idx.row(), MealTableModel::HId).data().toInt();

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		db.transaction();

	QSqlQuery q;
	q.prepare("SELECT distdate, scouts+leaders+others FROM meal WHERE id=?;");
	q.bindValue(0, mid);
	q.exec();

	QString distdate;
	double costs = 0.0;
	int mealpersons = 0;

	while (q.next()) {
		mealpersons = q.value(1).toInt();
		distdate = q.value(0).toString();
	}

	q.prepare("SELECT batch.price, distributor.quantity FROM batch,distributor WHERE distributor.distdate=? AND batch.id=distributor.batch_id;");
	q.bindValue(0, distdate);
	q.exec();
	while (q.next()) {
		double netto, tax;
		DataParser::price(q.value(0).toString(), netto, tax);
		costs += netto*(1.0 + tax/100.0)*q.value(1).toDouble();
	}

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		if (!db.commit())
			db.rollback();

	model_meal->setData(model_meal->index(idx.row(), MealTableModel::HDirty), 0);
	model_meal->setData(model_meal->index(idx.row(), MealTableModel::HAvgCosts), distdate.sprintf("%.2f", costs/mealpersons));
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

/** @brief Ta funkcja zawiera aktualizacje wersji baz danych. Funkcja powinna być wywoływana rekurencyjnie.
 *  @param version wersja bazy danych do aktualizacji
 *  @return zwraca true jeśliaktualiazcja zakończona sukcesem, w przeciwnym wypadku zmienna version zawiera
 * numer ostatniej wersji dla której aktualizacja zakończyla się sukcesem.
 */
bool Database::doDBUpgrade(unsigned int & version) {
	QSqlQuery qdbup;
	QString str;

	switch (version) {
		case 0:
			PR("Upgrade from 0");
			str = "UPDATE settings SET value='%1' WHERE key='dbversion';";
			qdbup.exec(str.arg(DBVERSION));
			version = DBVERSION;
			return doDBUpgrade(version);
			break;
		case DBVERSION:
			PR("Upgrade to");
			PR(DBVERSION);
			return true;
		default:
			return false;
	}
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
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HStaQty), int(qty*100));
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUsedQty), int(used*100));
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
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HStaQty), int(qty*100));
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

bool Database::addDistributorRecord(int bid, double qty, const QString& ddate, const QString& rdate, const QString& re1, const QString& re2, DistributorTableModel::Reasons re3, bool autoupdate) {
GTD
GTM
	QSqlQuery q;
	q.prepare("INSERT INTO distributor VALUES (NULL, ?, ?, ?, ?, ?, ?, ?);");
	q.bindValue(0, bid);
	q.bindValue(1, int(qty*100.0));
	q.bindValue(2, ddate);
	q.bindValue(3, rdate);
	q.bindValue(4, re1);
	q.bindValue(5, re2);
	q.bindValue(6, re3);
	bool status = q.exec();

	if (autoupdate) {
		updateBatchQty(bid);
		model_batch->select();
		model_distributor->select();
	}
GTM	
	return status;
}

bool Database::updateDistributorRecord(const QModelIndex & idx, unsigned int bid, double qty, const QDate & ddate, const QDate & rdate, const QString& re1, const QString& re2, DistributorTableModel::Reasons re3) {
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
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistDate), ddate);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HEntryDate), rdate);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistType), re1);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistTypeA), re2);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistTypeB), re3);

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
	if (status && submitBatches) {
		QMap<int, int>::iterator it = idmap.begin();
		while(it != idmap.end()) {
			updateBatchQty((it++).key());
		}
		status &= model_batch->submitAll();
	}
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
		updateBatchQty(bid.toInt());GTM
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
	disttypeb	= model_distributor->index(row, DistributorTableModel::HDistTypeB).data().toString();
// PR(model_batch->index(row, DistributorTableModel::HDistDate).data(Qt::EditRole).toDate().toString().toStdString().c_str());
// PR(model_batch->index(row, DistributorTableModel::HDistDate).data(Qt::DisplayRole).toDate().toString().toStdString().c_str());
	GTM	
	return true;
}

bool Database::addMealRecord(const QString& date, bool dirty, int scouts, int leaders, int others, double avgcosts, const QString & notes) {
	bool status = true;

	int row = model_meal->rowCount();

	model_meal->autoSubmit(false);
	status &= model_meal->insertRows(row, 1);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HDistDate), date);
	status &= model_meal->setData(model_meal->index(row, MealTableModel::HDirty), (int)dirty);
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

bool Database::updateMealRecord(int mid, const QString& date, bool dirty, int scouts, int leaders, int others, double avgcosts, const QString & notes) {
	bool status = true;

	model_meal->autoSubmit(false);
	status &= model_meal->setData(model_meal->index(mid, 1), date);
	status &= model_meal->setData(model_meal->index(mid, 2), (int)dirty);
	status &= model_meal->setData(model_meal->index(mid, 3), scouts);
	status &= model_meal->setData(model_meal->index(mid, 4), leaders);
	status &= model_meal->setData(model_meal->index(mid, 5), others);
	status &= model_meal->setData(model_meal->index(mid, 6), avgcosts);
	status &= model_meal->setData(model_meal->index(mid, 7), notes);
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
