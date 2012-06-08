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


#ifndef ABSTRACTTABLEVIEW_H
#define ABSTRACTTABLEVIEW_H

#include <QtGui/QTableView>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QMenu>

#include "Database.h"

/**
 * @brief Klasa dziedziczy po QTableView i odpowiada za wyświetlanie
 * naszych danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym widoku tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class AbstractTableView : public QTableView {
Q_OBJECT
public:
	AbstractTableView(QWidget * parent = NULL);
	virtual ~AbstractTableView();

	virtual void setModel(QAbstractItemModel* model);

	virtual void reloadPalette();

signals:
	void addRecordRequested(bool);
	void editRecordRequested(const QVector<int> & ids);
	bool removeRecordRequested(const QVector<int> & ids, bool ask);

private slots:
	virtual void removeRecord();
	virtual void addRecord();
	virtual void editRecord();

protected:
	virtual void contextMenuEvent(QContextMenuEvent* );

private:
	QMenu pmenu_del, pmenu_add;
	QAction * removeRec, * addRec, * editRec;
};

#endif // ABSTRACTTABLEVIEW_H
