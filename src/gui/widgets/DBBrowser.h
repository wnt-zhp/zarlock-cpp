/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#ifndef DBBROWSER_H
#define DBBROWSER_H

#include "zarlok.h"

#include "ui_DatabaseBrowser.h"

class DBBrowser : public QWidget, public Ui::DatabaseBrowser {
Q_OBJECT
public:
    DBBrowser(QWidget * parent = NULL);
    virtual ~DBBrowser();

	virtual void goBrowser();
	virtual void configure(int argc, char * argv[]);

public slots:
	void databaseSelected(QListWidgetItem * item);
	void newDatabaseCreator(bool autoopen = true);

	void openZarlock(const QString & dbname);
	void closeZarlock();

private slots:

private:
	void refreshList(int sort = 0, int order = 0);

signals:
	void dbb_database(const QString & dbname);

private:
	enum sortflags { s_name = 0, s_time, s_size };
	enum orderflags { o_asc = 0, o_dsc };

// 	QString dbname, dbfile;
// 	Database & db;
	zarlok * z;

	bool show_browser;
};

#endif // DBBROWSER_H
