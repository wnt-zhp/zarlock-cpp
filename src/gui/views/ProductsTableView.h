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


#ifndef PRODUCTSTABLEVIEW_H
#define PRODUCTSTABLEVIEW_H

#include <QtGui/QTableView>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

#include "Database.h"

/**
 * @brief Klasa dziedziczy po QTableView i odpowiada za wyświetlanie
 * naszych danych z tabeli 'products'. Ta klasa jest przykładem, jak można
 * w standardowym widoku tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class ProductsTableView : public QTableView {
Q_OBJECT
public:
    ProductsTableView(QWidget * parent = NULL);
    virtual ~ProductsTableView();

    virtual void setModel(QAbstractItemModel* model);

signals:
	void addRecordRequested(bool);

private slots:
	void removeRecord();
	void addRecord() { emit addRecordRequested(true); }

protected:
    void contextMenuEvent(QContextMenuEvent*);

private:
	Database & db;
	QMenu pmenu_del, pmenu_add;
	QAction * removeRec, * addRec;
};

#endif // PRODUCTSTABLEVIEW_H
