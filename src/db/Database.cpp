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

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtGui/QMessageBox>

#include <QFile>

#include "Database.h"
#include "globals.h"

/**
 * @brief Przetrzymuję jedną instancję klasy.
 **/
Database * Database::dbi = NULL;

/**
 * @brief Tworzy i zwraca instację do obiektu Database
 *
 * @return Database&
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
}

Database::Database(const Database & db) : QObject() {
	PR(this);
	PR(&db);
}

Database::~Database() {
	FPR(__func__);

	if (db.isOpen())
		db.close();

	if (tab_products) delete tab_products;
	if (tab_batch) delete tab_batch;
	if (tab_distributor) delete tab_distributor;

// 	QSqlDatabase::removeDatabase("QSQLITE");
}

/**
 * @brief ...
 *
 * @param dbfile ...
 * @param create ...
 * @return bool
 **/
bool Database::open_database(const QString & dbfile, bool recreate) {
	if (locked) {
		std::cerr << "Database already opened, close it before reopen." << std::endl;
		return false;
	}

	db.setDatabaseName(dbfile);
	bool ok = db.open();
	if (!ok) {
		QMessageBox::critical(0, tr("Cannot create database"),
							  tr("Unable to establish a database connection.\n"
								   "This application needs SQLite support. Please read "
								   "the Qt SQL driver documentation for information how "
								   "to build it.\n\n"
								   "Click Close to exit."), QMessageBox::Close);
		return false;
	}

	if (recreate) {
		QSqlQuery query;

		QFile dbresfile(":/resources/database.sql");
		if (!dbresfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			PR(false);
			return false;
		}
		while (!dbresfile.atEnd()) {
			QString line = dbresfile.readLine();
			query.exec(line.fromUtf8(line.toStdString().c_str()));
		}

		QFile dbtestfile(":/resources/test_data.sql");
		if (!dbtestfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			PR(false);
			return false;
		}
		while (!dbtestfile.atEnd()) {
			QString line = dbtestfile.readLine();
			query.exec(line.fromUtf8(line.toStdString().c_str()));
		}
	}

	// products
	tab_products = new ProductsTableModel;
	tab_products->setTable("products");
	tab_products->setEditStrategy(QSqlTableModel::OnManualSubmit);

	// batch
	tab_batch = new BatchTableModel;
	tab_batch->setTable("batch");
	tab_batch->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
	tab_batch->setRelation(BatchTableModel::HProdId, QSqlRelation("products", "id", "name"));

	tab_distributor = new DistributorTableModel;
	tab_distributor->setTable("distributor");
	tab_distributor->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
	tab_distributor->setRelation(DistributorTableModel::HBatchId, QSqlRelation("batch", "id", "id"/*"spec"*/));

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
	if (locked) {
		if (tab_products) delete tab_products;
		if (tab_batch) delete tab_batch;
		if (tab_distributor) delete tab_distributor;
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

void Database::updateBatchQty() {
	QSqlQuery qBatch("SELECT id FROM batch;");
	qBatch.exec();
	while (qBatch.next()) {
		updateBatchQty(qBatch.value(0).toInt());
	}
}

void Database::updateBatchQty(const int pid) {
	QSqlQuery qDist("SELECT quantity FROM distributor WHERE batch_id=?;");
	qDist.bindValue(0, pid);
	qDist.exec();
	int qty = 0;
	while (qDist.next()) {
		qty += qDist.value(0).toInt();
	}

	QSqlQuery qBatch("UPDATE batch SET used_qty=? WHERE id=?;");
	qBatch.bindValue(0, qty);
	qBatch.bindValue(1, pid);
	qBatch.exec();
}

void Database::updateMealCosts() {
	QSqlQuery qMeal("SELECT id FROM meal;");
	qMeal.exec();
	while (qMeal.next()) {
		updateMealCosts(qMeal.value(0).toInt());
	}
}

void Database::updateMealCosts(const int mid) {
	QSqlQuery qDist("SELECT price FROM distributor WHERE reason=?;");
	qDist.bindValue(0, mid);
	qDist.exec();
	int price = 0;
	while (qDist.next()) {
		price += qDist.value(0).toInt();
	}

// 	QSqlQuery qBatch("UPDATE batch SET used_qty=? WHERE id=?;");
// 	qBatch.bindValue(0, price);
// 	qBatch.bindValue(1, pid);
// 	qBatch.exec();
}

#include "Database.moc"