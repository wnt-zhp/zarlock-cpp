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

Database::Database() : QObject(), tab_products(NULL), tab_batch(NULL), tab_distributor(NULL), locked(false) {
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
	tab_products = new ProductsTableModel;
	tab_products->setTable("products");
	tab_products->setEditStrategy(QSqlTableModel::OnManualSubmit);

	// batch
	tab_batch = new BatchTableModel;
	tab_batch->setTable("batch");
	tab_batch->setEditStrategy(QSqlTableModel::OnManualSubmit);

	tab_distributor = new DistributorTableModel;
	tab_distributor->setTable("distributor");
	tab_distributor->setEditStrategy(QSqlTableModel::OnManualSubmit);

	camp = new CampProperties;
	readCampSettings();

	if (!camp->isCorrect)
		camp->campName = dbname;

// 	connect(tab_products, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(rebuild_models()));
// 	connect(tab_batch, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(rebuild_models()));
// 	connect(tab_distributor, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(rebuild_models()));

	locked = true;

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
		if (tab_products) delete tab_products;
		if (tab_batch) delete tab_batch;
		if (tab_distributor) delete tab_distributor;
		if (camp) delete camp;
	}
	locked = false;

	if (db.isOpen())
		db.close();
	return true;
}

void Database::save_database() {
	tab_products->submitAll();
	tab_batch->submitAll();
	tab_distributor->submitAll();
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
	if (!tab_products->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), tab_products->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// batch
	if (!tab_batch->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), tab_batch->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// distributor
	if (!tab_distributor->select()) {
		QMessageBox::critical(0, QObject::tr("Database error"), tab_distributor->lastError().text(), QMessageBox::Abort);
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

void Database::updateBatchQty(const int pid) {
	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		db.transaction();

	QSqlQuery q;
	q.prepare("SELECT quantity FROM distributor WHERE batch_id=?;");
	q.bindValue(0, pid);
	q.exec();
	float qty = 0;
	while (q.next()) {
		qty += q.value(0).toFloat();
	}

	q.prepare("UPDATE batch SET used_qty=? WHERE id=?;");
	q.bindValue(0, qty);
	q.bindValue(1, pid);
	q.exec();

	if (db.driver()->hasFeature(QSqlDriver::Transactions))
		if (!db.commit())
			db.rollback();
}

void Database::updateMealCosts() {
	QSqlQuery qMeal("SELECT id FROM meal;");
	qMeal.exec();
	while (qMeal.next()) {
		updateMealCosts(qMeal.value(0).toInt());
	}
}

void Database::updateMealCosts(const int mid) {
	QSqlQuery q("SELECT price FROM distributor WHERE reason=?;");
	q.bindValue(0, mid);
	q.exec();
	int price = 0;
	while (q.next()) {
		price += q.value(0).toInt();
	}

// 	QSqlQuery qBatch("UPDATE batch SET used_qty=? WHERE id=?;");
// 	qBatch.bindValue(0, price);
// 	qBatch.bindValue(1, pid);
// 	qBatch.exec();
}

void Database::writeCampSettings() {
	QSqlQuery csq;
	QString query("UPDATE settings SET value=\"%2\" WHERE key=\"%1\";");

	csq.exec(query.arg(CampProperties::HisCorrect).arg(camp->isCorrect));
	csq.exec(query.arg(CampProperties::HcampName).arg(camp->campName));
	csq.exec(query.arg(CampProperties::HcampDateBegin).arg(camp->campDateBegin.toString(Qt::ISODate)));
	csq.exec(query.arg(CampProperties::HcampDateEnd).arg(camp->campDateEnd.toString(Qt::ISODate)));
	csq.exec(query.arg(CampProperties::HscoutsNo).arg(camp->scoutsNo));
	csq.exec(query.arg(CampProperties::HleadersNo).arg(camp->leadersNo));
	csq.exec(query.arg(CampProperties::HcampLeader).arg(camp->campLeader));
	csq.exec(query.arg(CampProperties::HcampQuarter).arg(camp->campQuarter));
	csq.exec(query.arg(CampProperties::HcampOthers).arg(camp->campOthers));
}

void Database::readCampSettings() {
	QSqlQuery csq;
	csq.exec("SELECT * FROM settings;");
	while(csq.next()) {
		switch (csq.value(0).toInt()) {
			case CampProperties::HisCorrect:
				camp->isCorrect = csq.value(1).toBool();
				break;
			case CampProperties::HcampName:
				camp->campName = csq.value(1).toString();
				break;
			case CampProperties::HcampDateBegin:
				camp->campDateBegin = csq.value(1).toDate();
				break;
			case CampProperties::HcampDateEnd:
				camp->campDateEnd = csq.value(1).toDate();
				break;
			case CampProperties::HscoutsNo:
				camp->scoutsNo = csq.value(1).toInt();
				break;
			case CampProperties::HleadersNo:
				camp->leadersNo = csq.value(1).toInt();
				break;
			case CampProperties::HcampLeader:
				camp->campLeader = csq.value(1).toString();
				break;
			case CampProperties::HcampQuarter:
				camp->campQuarter = csq.value(1).toString();
				break;
			case CampProperties::HcampOthers:
				camp->campOthers = csq.value(1).toString();
				break;
		}
	}
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

bool Database::addProductsRecord(const QString& name, const QString& unit, const QString& expiry) {
	bool status = true;

// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< edit_name->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << " );" << std::endl;
	int row = tab_products->rowCount();
	status &= tab_products->insertRows(row, 1);
	status &= tab_products->setData(tab_products->index(row, 1), name);
	status &= tab_products->setData(tab_products->index(row, 2), unit);
	status &= tab_products->setData(tab_products->index(row, 3), expiry);

	if (!status) {
		tab_products->revertAll();
		return false;
	} else
		return tab_products->submitAll();
}

bool Database::updateProductsRecord(int pid, const QString& name, const QString& unit, const QString& expiry) {
	bool status = true;

// 	INFO std::cout << "QUERY: INSERT INTO products VALUES ( "
// 		<< row << ","
// 		<< edit_name->text().toStdString() << ","
// 		<< edit_unit->text().toStdString() << ","
// 		<< edit_expiry->text().toStdString() << " );" << std::endl;

	status &= tab_products->setData(tab_products->index(pid, 1), name);
	status &= tab_products->setData(tab_products->index(pid, 2), unit);
	status &= tab_products->setData(tab_products->index(pid, 3), expiry);

	if (!status) {
		tab_products->revertAll();
		return false;
	}

	return tab_products->submitAll();
}

// bool Database::removeProductsRecord(int recordid, bool askForConfirmation) {
// 	QString product = tab_products->index(recordid, ProductsTableModel::HName).data().toString();
// 	QVariant pid = tab_products->index(recordid, ProductsTableModel::HId).data();
// 
// 	bool status = false;
// 	QModelIndexList qmil = tab_batch->match(tab_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
// 	int count = qmil.count();
// 	if (askForConfirmation)
// 	if (count) {
// 		QString details;
// 		for (int i = 0; i < count; ++i) {
// 			details = details % tab_batch->data(tab_batch->index(qmil.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
// 		}
// 		status = tab_products->productRemoveConfirmation(product, details);
// 	} else {
// 		status = tab_products->productRemoveConfirmation(product);
// 	}
// 
// 	if (askForConfirmation & !status )
// 		return false;
// 
// 	removeBatchRecord(qmil);
// 
// 	if (!tab_products->removeRow(recordid)) {
// 		tab_products->revertAll();
// 		return false;
// 	}
// 
// 	return tab_products->submitAll();
// }

bool Database::removeProductsRecord(const QModelIndexList & idxl, bool askForConfirmation) {
	bool status = false;
	QString details;
	QModelIndexList bat;

	int counter = 0;
	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == ProductsTableModel::HName) {
				QVariant pid = tab_products->index(idxl.at(i).row(), ProductsTableModel::HId).data();
				QString prod = tab_products->index(idxl.at(i).row(), ProductsTableModel::HName).data().toString();
				++counter;
				details = details % prod % "\n";
				QModelIndexList batches = tab_batch->match(tab_batch->index(0, BatchTableModel::HProdId), Qt::EditRole, pid, -1);
				for (int i = 0; i < batches.count(); ++i) {
					details = details % "  \\--  " % tab_batch->data(tab_batch->index(batches.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
				}
				bat.append(batches);
			}
		}
		status = tab_products->productRemoveConfirmation(counter, details);
	}

	if (askForConfirmation & !status )
		return false;

	removeBatchRecord(bat);

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == ProductsTableModel::HName) {
			if (!tab_products->removeRow(idxl.at(i).row())) {
				tab_products->revertAll();
				return false;
			}
		}
	}
	
	return tab_products->submitAll();
}

// bool Database::removeProductsRecord(const QModelIndex & idx, bool askForConfirmation) {
// // 	int bid = tab_distributor->index(idx.row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
// // 	tab_distributor->removeRow(idx.row());
// // 	tab_distributor->submitAll();
// // 	updateBatchQty(bid);
// // 	tab_batch->submitAll();
// }

bool Database::addBatchRecord(int pid, const QString& spec, const QString& book, const QString& reg, const QString& expiry, float qty, float used, const QString& unit, const QString& price, const QString& invoice, const QString& desc) {
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

	int row = tab_batch->rowCount();
	status &= tab_batch->insertRows(row, 1);
// 	tab_batch->setData(tab_batch->index(row, BatchTableModel::HId), row);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HProdId), pid);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HSpec), spec);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HExpire), expiry);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HStaQty), qty);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HUnit), unit);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HPrice), price);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HUsedQty), used);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HInvoice), invoice);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HBook), book);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HRegDate), reg);
	status &= tab_batch->setData(tab_batch->index(row, BatchTableModel::HDesc), desc);

	if (!status) {
		tab_batch->revertAll();
		return false;
	}
	
	return tab_batch->submitAll();
// 	update_model();	
}

bool Database::updateBatchRecord(int bid, int pid, const QString& spec, const QString& book, const QString& reg, const QString& expiry, float qty, float used, const QString& unit, const QString& price, const QString& invoice, const QString& desc) {
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

// 	tab_batch->setData(tab_batch->index(row, BatchTableModel::HId), row);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HProdId), pid);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HSpec), spec);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HExpire), expiry);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HStaQty), qty);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HUnit), unit);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HPrice), price);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HUsedQty), used);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HInvoice), invoice);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HBook), book);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HRegDate), reg);
	status &= tab_batch->setData(tab_batch->index(bid, BatchTableModel::HDesc), desc);

	if (!status) {
		tab_batch->revertAll();
		return false;
	}
	
	return tab_batch->submitAll();
}

// bool Database::removeBatchRecord(int recordid, bool askForConfirmation) {
// 	tab_batch->removeRow(recordid);
// 	tab_batch->submitAll();
// }

bool Database::removeBatchRecord(const QModelIndexList & idxl, bool askForConfirmation) {

// 	QString batch = tab_products->index(recordid, ProductsTableModel::HName).data().toString();
// 	QVariant pid = tab_products->index(recordid, ProductsTableModel::HId).data();

	bool status = false;
	QString details;
	int counter = 0;

	if (askForConfirmation) {
		for (int i = 0; i < idxl.count(); ++i) {
			if (idxl.at(i).column() == BatchTableModel::HProdId) {
				++counter;
				details = details % tab_batch->data(tab_batch->index(idxl.at(i).row(), BatchTableModel::HSpec), Qt::DisplayRole).toString() % "\n";
			}
		}
		status = tab_batch->batchRemoveConfirmation(counter, details);
	}

	if (askForConfirmation & !status )
		return false;

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == BatchTableModel::HProdId) {
			QVariant bid = tab_batch->data(tab_batch->index(idxl.at(i).row(), BatchTableModel::HId), Qt::EditRole);
			QModelIndexList qmild = tab_distributor->match(tab_distributor->index(0, DistributorTableModel::HBatchId), Qt::EditRole, bid, -1);
			if (removeDistributorRecord(qmild, false, false))
				status = tab_batch->removeRow(idxl.at(i).row());
		}
	}
	return (status && tab_batch->submitAll());
}

// bool Database::removeBatchRecord(const QModelIndex & idx, bool askForConfirmation) {
// 	PR(__func__);
// 	QVariant bid = tab_batch->data(tab_batch->index(idx.row(), BatchTableModel::HId), Qt::EditRole);
// 	PR(bid.toInt());
// 	QModelIndexList qmild = tab_distributor->match(tab_distributor->index(0, DistributorTableModel::HBatchId), Qt::EditRole, bid, -1);
// 	removeDistributorRecord(qmild);
// 
// 	tab_batch->removeRow(idx.row());
// 	tab_batch->submitAll();
// }

bool Database::addDistributorRecord(int bid, float qty, const QString& ddate, const QString& rdate, const QString& re1, const QString& re2, DistributorTableModel::Reasons re3) {
	bool status = true;

	int row = tab_distributor->rowCount();
// PR(bid);
// PR(qty);
// PR(ddate.toStdString());
// PR(rdate.toStdString());
// PR(re1.toStdString());
// PR(re2.toStdString());
// PR(re3);
// return false;

	status &= tab_distributor->insertRows(row, 1);
// 	status &= tab_distributor->setData(btm->index(row, DistributorTableModel::HId), row);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HBatchId), bid);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HQty), qty);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HDistDate), ddate);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HRegDate), rdate);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HReason), re1);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HReason2), re2);
	status &= tab_distributor->setData(tab_distributor->index(row, DistributorTableModel::HReason3), re3);

	if (!status) {
		tab_distributor->revertAll();
		return false;
	} else if (!tab_distributor->submitAll())
		return false;

	updateBatchQty(tab_distributor->index(row, 1).data(Qt::EditRole).toInt());
	return tab_batch->submitAll();
// 	update_model();	
}

bool Database::updateDistributorRecord(int id, int bid, float qty, const QString& ddate, const QString& rdate, const QString& re1, const QString& re2, DistributorTableModel::Reasons re3) {
	bool status = true;
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HBatchId), bid);
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HQty), qty);
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HDistDate), ddate);
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HRegDate), rdate);
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HReason), re1);
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HReason2), re2);
	status &= tab_distributor->setData(tab_distributor->index(id, DistributorTableModel::HReason3), re3);

	if (!status) {
		tab_distributor->revertAll();
		return false;
	} else if (!tab_distributor->submitAll())
		return false;

	updateBatchQty(tab_distributor->index(id, 1).data(Qt::EditRole).toInt());
	return tab_batch->submitAll();
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
				details = details % tab_distributor->data(tab_distributor->index(idxl.at(i).row(), DistributorTableModel::HBatchId), Qt::DisplayRole).toString() % "\n";
			}
		}
		status = tab_batch->distributeRemoveConfirmation(counter, details);
	}

	if (!status)
		return false;

	for (int i = 0; i < idxl.count(); ++i) {
		if (idxl.at(i).column() == DistributorTableModel::HBatchId) {
			int bid = tab_distributor->index(idxl.at(i).row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
			if (tab_distributor->removeRow(idxl.at(i).row()))
				idmap[bid]=1;
		}
	}
	status = tab_distributor->submitAll();
	if (status && submitBatches) {
		QMap<int, int>::iterator it = idmap.begin();
		while(it != idmap.end()) {
			updateBatchQty((it++).key());
		}
		status &= tab_batch->submitAll();
	}
	return status;
}

// bool Database::removeDistributorRecord(const QModelIndex & idx, bool askForConfirmation, bool submitBatches) {
// 	int bid = tab_distributor->index(idx.row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
// 	tab_distributor->removeRow(idx.row());
// 	tab_distributor->submitAll();
// 	updateBatchQty(bid);
// 	if (submitBatches) tab_batch->submitAll();
// }

bool Database::removeDistributorRecord(int recordid, bool askForConfirmation, bool submitBatches) {
	QString batch;
	QVariant bid = tab_distributor->index(recordid, DistributorTableModel::HBatchId).data(Qt::EditRole);
	bool status = true;

	if (askForConfirmation) {
		QModelIndexList batchl = tab_batch->match(tab_batch->index(0, BatchTableModel::HId), Qt::DisplayRole, bid);
		if (batchl.count())
			batch = tab_batch->data(tab_batch->index(batchl.at(0).row(), BatchTableModel::HSpec)).toString();
		status = tab_distributor->distributeRemoveConfirmation(1, batch);
	}

	if (status) {
		if (!tab_distributor->removeRow(recordid)) {
			tab_distributor->revertAll();
			return false;
		}
		status = tab_distributor->submitAll();
		updateBatchQty(bid.toInt());
		if (submitBatches) tab_batch->submitAll();
	}
	return status;
}

#include "Database.moc"