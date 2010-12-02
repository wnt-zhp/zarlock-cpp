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
#include <QtSql/QSqlRecord>
#include <QtGui/QMessageBox>

#include "Database.h"
#include <QSqlError>

using namespace std;

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
		dbi = new Database();
		cout << "++ Create Database instance\n";
	} else {
		cout << "++ Use existing Database instance\n";
	}

	return *dbi;
}

Database::Database() : tab_products(NULL), tab_batch(NULL) {
	db = QSqlDatabase::addDatabase("QSQLITE");
}

Database::~Database() {
	if (tab_products) delete tab_products;
	if (tab_batch) delete tab_batch;

	if (db.isOpen())
		db.close();
	QSqlDatabase::removeDatabase("QSQLITE");
}

/**
 * @brief ...
 *
 * @param dbfile ...
 * @param create ...
 * @return bool
 **/
bool Database::open_database(const QString & dbfile, bool recreate) {
	db.setDatabaseName(dbfile);
	bool ok = db.open();
	if (!ok) {
// 		QMessageBox::critical(0, qApp->tr("Cannot create database"),
// 							  qApp->tr("Unable to establish a database connection.\n"
// 									   "This application needs SQLite support. Please read "
// 									   "the Qt SQL driver documentation for information how "
// 									   "to build it.\n\n"
// 									   "Click Close to exit."), QMessageBox::Close);
// 		return false;
	}

	QSqlQuery query;
	
	if (recreate)
		query.exec("DROP TABLE IF EXISTS products)");
		query.exec("CREATE TABLE IF NOT EXISTS 'products' ("
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
				   "name VARCHAR(50) UNIQUE,"
				   "unit VARCHAR(50),"
				   "expire VARCHAR(20)"
				   ")");
	if (recreate)
		query.exec("DROP TABLE IF EXISTS batch)");
		query.exec("CREATE TABLE IF NOT EXISTS 'batch' ("
				   "id INTEGER PRIMARY KEY AUTOINCREMENT, "		// id
				   "prod_id INTEGER, "							// id produktu ->products.id
				   "spec VARCHAR(50),"							// specyfikacja
				   "price VARCHAR(20),"							// cena
				   "unit VARCHAR(50),"							// jednostka
				   "start_qty FLOAT,"							// ilosc poczatkowa
				   "booking VARCHAR(20),"						// data ksiegowania
				   "expire VARCHAR(20),"						// data waznosci
				   "curr_qty FLOAT,"							// aktualna ilosc
				   "date VARCHAR(20),"								// data dodania wpisu
				   "desc TEXT,"									// opis uzytkownika
				   "invoice_no VARCHAR(200)"					// numer faktury
				   ")");

// 		query.exec("INSERT into products VALUES(0, 'ryz', '250mg', 100)");
// 		query.exec("INSERT into batch VALUES(0, 0, 'sypany', '2', '250mg', 10, 0, 100, 7, 0, 'ryz,ryzek,ryzuszek', 1234567890)");

	rebuild_models();
	return true;
}

/**
 * @brief ...
 *
 * @return bool
 **/
bool Database::close_database() {
	if (db.isOpen())
		db.close();
}

/**
 * @brief ...
 *
 * @return bool
 **/
bool Database::rebuild_models() {
	// products
	if (tab_products) delete tab_products;
	tab_products = new ProductsTableModel;
	tab_products->setTable("products");
	tab_products->setEditStrategy(QSqlTableModel::OnManualSubmit);
	if (!tab_products->select()) {
		QMessageBox::critical(0, "Błąd bazy danych", tab_products->lastError().text(), QMessageBox::Abort);
		return false;
	}

	// batch
	if (tab_batch) delete tab_batch;
	tab_batch = new BatchTableModel;
	tab_batch->setTable("batch");
	tab_batch->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);
	if (!tab_batch->select()) {
		QMessageBox::critical(0, "Błąd bazy danych", tab_batch->lastError().text(), QMessageBox::Abort);
		return false;
	}

	return true;
}
