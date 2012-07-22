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

#ifndef DBBROWSER_H
#define DBBROWSER_H

#include <QFileInfo>

#include "ui_DatabaseBrowser.h"

class zarlok;

class DBBrowser : public QWidget, public Ui::DatabaseBrowser {
Q_OBJECT
public:
	DBBrowser(QWidget * parent = NULL);
	virtual ~DBBrowser();

	virtual void goBrowser();

	struct DBListEntry {
		QFileInfo fileInfo;
		QString infoContent;
	};

	enum sortflags { s_name = 0, s_time, s_size };
	enum orderflags { o_asc = 0, o_dsc };
	static QVector<DBListEntry> fetchDBEntryList(sortflags sort = s_name, orderflags order = o_asc);
	static QString dbInfoName(const QString & dbname);

public slots:
	void databaseSelected(QListWidgetItem * item);
	QString newDatabaseCreator(bool autoopen = true);
	void deleteDatabase();

	void openZarlock();
	void openZarlock(const QString & dbname);
	void closeZarlock();

private:
	void refreshList(sortflags sort = s_name, orderflags order = o_asc);

signals:
	void dbb_database(const QString & dbname);

private:
	zarlok * z;
};

#endif // DBBROWSER_H
