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

#include <QFile>
#include <QDir>
#include <QStringBuilder>

#include "Database.h"
#include "DataParser.h"
#include "globals.h"
#include "config.h"

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

Database::Database() : QObject(), model_products(NULL), model_batch(NULL), model_distributor(NULL), model_meal(NULL), locked(false) {
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
	model_batch->submitAll();
	model_distributor->submitAll();
	model_meal->submitAll();

	updateBatchQty();

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

		QFile dbresfile(":/resources/database.sql");
		if (!dbresfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			return false;
		}
		while (!dbresfile.atEnd()) {
			QString line = dbresfile.readLine();
			query.exec(line.fromUtf8(line.toStdString().c_str()));
		}

		QFile dbtestfile(":/resources/test_data.sql");
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
			db.commit();

		updateBatchQty();

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
	QSqlQuery qBatch("SELECT id FROM batch;");
	qBatch.exec();
	while (qBatch.next()) {
		updateBatchQty(qBatch.value(0).toInt());
	}
}

void Database::updateBatchQty(const int bid) {
	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		db.transaction();

	QSqlQuery q;
	q.prepare("SELECT quantity FROM distributor WHERE batch_id=?;");
	q.bindValue(0, bid);
	q.exec();
	float qty = 0;
	while (q.next()) {
		qty += q.value(0).toFloat();
	}

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		if (!db.commit())
			db.rollback();

	QModelIndexList idxl = model_batch->match(model_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
	if (idxl.count())
		model_batch->setData(model_batch->index(idxl.at(0).row(), BatchTableModel::HUsedQty), qty);
}

void Database::updateMealCosts() {
	for (int i = 0; i < model_meal->rowCount(); ++i)
		updateMealCosts(model_meal->index(i, MealTableModel::HId));
}

void Database::updateMealCosts(const QModelIndex& idx) {

	int mid = model_meal->index(idx.row(), MealTableModel::HId).data().toInt();

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		db.transaction();

	QSqlQuery q;
	q.prepare("SELECT distdate, scouts, leaders, others FROM meal WHERE id=?;");
	q.bindValue(0, mid);
	q.exec();

	QString distdate;
	float costs = 0.0;
	int mealpersons = 0;

	while (q.next()) {
		mealpersons = q.value(1).toInt() + q.value(2).toInt() + q.value(3).toInt();
		distdate = q.value(0).toString();
	}

	q.prepare("SELECT batch.price,distributor.quantity FROM batch,distributor where distributor.distdate=? AND batch.id=distributor.batch_id;");
	q.bindValue(0, distdate);
	q.exec();
	while (q.next()) {
		double netto, tax;
		DataParser::price(q.value(0).toString(), netto, tax);
		costs += netto*(1.0 + tax/100.0)*q.value(1).toFloat();
	}

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		if (!db.commit())
			db.rollback();

	model_meal->setData(model_meal->index(idx.row(), MealTableModel::HDirty), 0);
	model_meal->setData(model_meal->index(idx.row(), MealTableModel::HAvgCosts), distdate.sprintf("%.2f", costs/mealpersons));
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

// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< edit_name->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << " );" << std::endl;
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
	} else
		return model_products->submitAll();
}

bool Database::updateProductsRecord(int pid, const QString& name, const QString& unit, const QString& expiry, const QString & notes) {
	bool status = true;

// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< edit_name->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << " );" << std::endl;
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

	return model_products->submitAll();
}

// bool Database::removeProductsRecord(int recordid, bool askForConfirmation) {
// 	QString product = model_products->index(recordid, ProductsTableModel::HName).data().toString();
// 	QVariant pid = model_products->index(recordid, ProductsTableModel::HId).data();
// 
// 	bool status = false;
// 	QModelIndexList qmil = model_batch->match(model_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
// 	int count = qmil.count();
// 	if (askForConfirmation)
// 	if (count) {
// 		QString details;
// 		for (int i = 0; i < count; ++i) {
// 			details = details % model_batch->data(model_batch->index(qmil.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
// 		}
// 		status = model_products->productRemoveConfirmation(product, details);
// 	} else {
// 		status = model_products->productRemoveConfirmation(product);
// 	}
// 
// 	if (askForConfirmation & !status )
// 		return false;
// 
// 	removeBatchRecord(qmil);
// 
// 	if (!model_products->removeRow(recordid)) {
// 		model_products->revertAll();
// 		return false;
// 	}
// 
// 	return model_products->submitAll();
// }

bool Database::removeProductsRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;
// 	QModelIndexList bat;

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
// 				bat.append(batches);
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
	rebuild_models();
	
	return status;
}

// bool Database::removeProductsRecord(const QModelIndex & idx, bool askForConfirmation) {
// // 	int bid = model_distributor->index(idx.row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
// // 	model_distributor->removeRow(idx.row());
// // 	model_distributor->submitAll();
// // 	updateBatchQty(bid);
// // 	model_batch->submitAll();
// }

bool Database::addBatchRecord(int pid, const QString& spec, const QString& book, const QString& reg, const QString& expiry, float qty, float used, const QString& unit, const QString& price, const QString& invoice, const QString& notes) {
	bool status = true;
	
// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< prod_id << ","
// 		<< edit_spec->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << ","
// 		<< edit_qty->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< unitprice.toStdString() << ","
// 		<< edit_qty->text().toStdString() << ","
// 		<< edit_invoice->text().toStdString() << ","
// 		<< edit_book->text(true).toStdString() << ","
// 		<< QDate::currentDate().toString(Qt::ISODate).toStdString() << ","
// 		<< QString(":)").toStdString() << " );" << std::endl;

	int row = model_batch->rowCount();

	model_batch->autoSubmit(false);
	status &= model_batch->insertRows(row, 1);
// 	model_batch->setData(model_batch->index(row, BatchTableModel::HId), row);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HProdId), pid);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HSpec), spec);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HExpire), expiry);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HStaQty), qty);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUnit), unit);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HPrice), price);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HUsedQty), used);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HInvoice), invoice);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HBook), book);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HRegDate), reg);
	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HNotes), notes);
	model_batch->autoSubmit(true);

	if (!status) {
		model_batch->revertAll();
		return false;
	}
	
	return model_batch->submitAll();
// 	update_model();	
}

bool Database::updateBatchRecord(int bid, int pid, const QString& spec, const QString& book, const QString& reg, const QString& expiry, float qty, float used, const QString& unit, const QString& price, const QString& invoice, const QString& notes) {
	bool status = true;
	
// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< prod_id << ","
// 		<< edit_spec->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << ","
// 		<< edit_qty->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< unitprice.toStdString() << ","
// 		<< edit_qty->text().toStdString() << ","
// 		<< edit_invoice->text().toStdString() << ","
// 		<< edit_book->text(true).toStdString() << ","
// 		<< QDate::currentDate().toString(Qt::ISODate).toStdString() << ","
// 		<< QString(":)").toStdString() << " );" << std::endl;

	model_batch->autoSubmit(false);
// 	status &= model_batch->setData(model_batch->index(row, BatchTableModel::HId), row);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HProdId), pid);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HSpec), spec);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HExpire), expiry);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HStaQty), qty);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HUnit), unit);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HPrice), price);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HUsedQty), used);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HInvoice), invoice);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HBook), book);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HRegDate), reg);
	status &= model_batch->setData(model_batch->index(bid, BatchTableModel::HNotes), notes);
	model_batch->autoSubmit(true);

	if (!status) {
		model_batch->revertAll();
		return false;
	}
	
	return model_batch->submitAll();
}

// bool Database::removeBatchRecord(int recordid, bool askForConfirmation) {
// 	model_batch->removeRow(recordid);
// 	model_batch->submitAll();
// }

bool Database::removeBatchRecord(const QModelIndexList & idxl, bool askForConfirmation) {

// 	QString batch = model_products->index(recordid, ProductsTableModel::HName).data().toString();
// 	QVariant pid = model_products->index(recordid, ProductsTableModel::HId).data();

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
// 			QVariant bid = model_batch->data(model_batch->index(idxl.at(i).row(), BatchTableModel::HId), Qt::EditRole);
// 			QModelIndexList qmild = model_distributor->match(model_distributor->index(0, DistributorTableModel::HBatchId), Qt::EditRole, bid, -1);
// 			if (removeDistributorRecord(qmild, false, false))
				status = model_batch->removeRow(idxl.at(i).row());
		}
	}
	return (status && model_batch->submitAll() && rebuild_models());
}

// bool Database::removeBatchRecord(const QModelIndex & idx, bool askForConfirmation) {
// 	PR(__func__);
// 	QVariant bid = model_batch->data(model_batch->index(idx.row(), BatchTableModel::HId), Qt::EditRole);
// 	PR(bid.toInt());
// 	QModelIndexList qmild = model_distributor->match(model_distributor->index(0, DistributorTableModel::HBatchId), Qt::EditRole, bid, -1);
// 	removeDistributorRecord(qmild);
// 
// 	model_batch->removeRow(idx.row());
// 	model_batch->submitAll();
// }

bool Database::addDistributorRecord(int bid, float qty, const QString& ddate, const QString& rdate, const QString& re1, const QString& re2, DistributorTableModel::Reasons re3) {
	bool status = true;

	int row = model_distributor->rowCount();
// PR(bid);
// PR(qty);
// PR(ddate.toStdString());
// PR(rdate.toStdString());
// PR(re1.toStdString());
// PR(re2.toStdString());
// PR(re3);
// return false;

	model_distributor->autoSubmit(false);
	status &= model_distributor->insertRows(row, 1);
// 	status &= model_distributor->setData(btm->index(row, DistributorTableModel::HId), row);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HBatchId), bid);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HQty), qty);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HDistDate), ddate);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HRegDate), rdate);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HReason), re1);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HReason2), re2);
	status &= model_distributor->setData(model_distributor->index(row, DistributorTableModel::HReason3), re3);
	model_distributor->autoSubmit(true);

	if (!status) {
		model_distributor->revertAll();
		return false;
	} else if (!model_distributor->submitAll())
		return false;

	updateBatchQty(model_distributor->index(row, 1).data(Qt::EditRole).toInt());
	return model_batch->submitAll();
// 	update_model();	
}

bool Database::updateDistributorRecord(int id, int bid, float qty, const QString& ddate, const QString& rdate, const QString& re1, const QString& re2, DistributorTableModel::Reasons re3) {
	bool status = true;

	model_distributor->autoSubmit(false);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HBatchId), bid);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HQty), qty);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HDistDate), ddate);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HRegDate), rdate);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HReason), re1);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HReason2), re2);
	status &= model_distributor->setData(model_distributor->index(id, DistributorTableModel::HReason3), re3);
	model_distributor->autoSubmit(true);

	if (!status) {
		model_distributor->revertAll();
		return false;
	} else if (!model_distributor->submitAll())
		return false;

	updateBatchQty(model_distributor->index(id, 1).data(Qt::EditRole).toInt());
	return model_batch->submitAll();
// 	update_model();	
}

bool Database::removeDistributorRecord(const QModelIndexList & idxl, bool askForConfirmation, bool submitBatches) {
	bool status = true;
	int counter = 0;
	QString details;
	QMap<int, int> idmap;

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

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == DistributorTableModel::HBatchId) {
			int bid = model_distributor->index(idxl.at(i).row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
			if (model_distributor->removeRow(idxl.at(i).row()))
				idmap[bid]=1;
		}
	}
	status = model_distributor->submitAll();
	if (status && submitBatches) {
		QMap<int, int>::iterator it = idmap.begin();
		while(it != idmap.end()) {
			updateBatchQty((it++).key());
		}
		status &= model_batch->submitAll();
	}
	return status;
}

// bool Database::removeDistributorRecord(const QModelIndex & idx, bool askForConfirmation, bool submitBatches) {
// 	int bid = model_distributor->index(idx.row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
// 	model_distributor->removeRow(idx.row());
// 	model_distributor->submitAll();
// 	updateBatchQty(bid);
// 	if (submitBatches) model_batch->submitAll();
// }

bool Database::removeDistributorRecord(int recordid, bool askForConfirmation, bool submitBatches) {
	QString batch;
	QVariant bid = model_distributor->index(recordid, DistributorTableModel::HBatchId).data(Qt::EditRole);
	bool status = true;

	if (askForConfirmation) {
		QModelIndexList batchl = model_batch->match(model_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
		if (batchl.count())
			batch = model_batch->data(model_batch->index(batchl.at(0).row(), BatchTableModel::HSpec)).toString();
		status = model_distributor->distributeRemoveConfirmation(1, batch);
	}

	if (status) {
		if (!model_distributor->removeRow(recordid)) {
			model_distributor->revertAll();
			return false;
		}
		status = model_distributor->submitAll();
		updateBatchQty(bid.toInt());
		if (submitBatches) model_batch->submitAll();
	}
	return status;
}

bool Database::addMealRecord(const QString& date, bool dirty, int scouts, int leaders, int others, double avgcosts, const QString & notes) {
	bool status = true;

	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
		<< 0 << ","
		<< date.toStdString() << ","
		<< dirty << ","
		<< scouts << ","
		<< leaders << ","
		<< others << ","
		<< avgcosts << ","
		<< notes.toStdString() << " );" << std::endl;
	
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

// bool Database::removeProductsRecord(int recordid, bool askForConfirmation) {
// 	QString product = model_meal->index(recordid, ProductsTableModel::HName).data().toString();
// 	QVariant pid = model_meal->index(recordid, ProductsTableModel::HId).data();
// 
// 	bool status = false;
// 	QModelIndexList qmil = model_batch->match(model_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
// 	int count = qmil.count();
// 	if (askForConfirmation)
// 	if (count) {
// 		QString details;
// 		for (int i = 0; i < count; ++i) {
// 			details = details % model_batch->data(model_batch->index(qmil.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
// 		}
// 		status = model_meal->productRemoveConfirmation(product, details);
// 	} else {
// 		status = model_meal->productRemoveConfirmation(product);
// 	}
// 
// 	if (askForConfirmation & !status )
// 		return false;
// 
// 	removeBatchRecord(qmil);
// 
// 	if (!model_meal->removeRow(recordid)) {
// 		model_meal->revertAll();
// 		return false;
// 	}
// 
// 	return model_meal->submitAll();
// }

bool Database::removeMealRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;
// 	QModelIndexList bat;

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

// 	removeBatchRecord(bat, false);

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == ProductsTableModel::HName) {
			if (!model_meal->removeRow(idxl.at(i).row())) {
				model_meal->revertAll();
				return false;
			}
		}
	}
	
	status = model_meal->submitAll();
// 	rebuild_models();
	
	return status;
}

#include "Database.moc"