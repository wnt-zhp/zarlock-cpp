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

#include "globals.h"
#include "config.h"

#include <QMessageBox>
#include <QInputDialog>

#include "Database.h"
#include "DBBrowser.h"

DBBrowser::DBBrowser(QWidget * parent): QWidget(parent), z(NULL) {
	CI();

	this->setVisible(false);
	setupUi(this);

	connect(dbb_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(databaseSelected(QListWidgetItem *)));
	connect(dbb_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(openZarlock()));

	connect(dbb_new, SIGNAL(clicked(bool)), this, SLOT(newDatabaseCreator()));
	connect(dbb_quit, SIGNAL(clicked(bool)), this, SLOT(close()));
	connect(dbb_load, SIGNAL(clicked(bool)), this, SLOT(openZarlock()));
	connect(dbb_delete, SIGNAL(clicked(bool)), this, SLOT(deleteDatabase()));

	dbb_delete->setEnabled(false);

	dbb_new->setIcon( QApplication::style()->standardIcon(QStyle::SP_FileIcon) );
	dbb_delete->setIcon( QApplication::style()->standardIcon(QStyle::SP_TrashIcon) );
	dbb_quit->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton) );
	dbb_load->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton) );

// 	dbb_list->setViewMode(QListView::IconMode);
	dbb_list->setWrapping(true);
	refreshList();

// 	dbb_list->setViewMode(QListView::IconMode);
}

DBBrowser::~DBBrowser() {
	DI();

	while (dbb_list->count()) {
		QListWidgetItem * item = dbb_list->item(0);
		dbb_list->removeItemWidget(item);
		delete item;
	}
}

void DBBrowser::goBrowser() {
	globals::appSettings->beginGroup("Database");
	QString recentDB = globals::appSettings->value("RecentDatabase", "").toString();
	globals::appSettings->endGroup();

	if (globals::show_browser) {
		show();
	} else {
		if (!recentDB.isEmpty()) {
			openZarlock(recentDB);
		} else {
			PR("First run! Welcome to żarłok.");
			QString newdb = newDatabaseCreator();
			if (!newdb.isNull())
				openZarlock(newdb);
			else
				this->setVisible(true);
		}
	}
}

void DBBrowser::openZarlock(const QString & dbname) {
	if (Database::Instance()->open_database(dbname)) {
		dbb_load->setEnabled(false);
		this->setVisible(false);

		globals::appSettings->beginGroup("Database");
		globals::appSettings->setValue("RecentDatabase", dbname);
		globals::appSettings->endGroup();

		z = new zarlok();
		z->show();

// 		connect(z, SIGNAL(destroyed()), this, SLOT(closeZarlock()));
		connect(z, SIGNAL(switchDB()), this, SLOT(closeZarlock()));
	}
}

void DBBrowser::openZarlock() {
	QListWidgetItem * item = dbb_list->currentItem();
	if (item) {
		QString dbname = item->data(Qt::UserRole).toString();
		openZarlock(dbname);
	}
}

void DBBrowser::closeZarlock() {
	FI();

	if (z) {
// 		Database::Instance().close_database();
		disconnect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
		delete z;
		z = NULL;
	}
	this->setVisible(true);
	Database::Destroy();

	QListWidgetItem * item = dbb_list->currentItem();
	if (item)
		dbb_load->setEnabled(true);
}

void DBBrowser::deleteDatabase() {
	QListWidgetItem * item = dbb_list->currentItem();
	if (item) {
		QString dbname = item->data(Qt::UserRole).toString();
		int ret = QMessageBox::question(
			this,
			tr("Database removing"),
			tr("Are you sure to delete database %1?").arg(dbname),
			QMessageBox::Yes, QMessageBox::No);

		if (ret == QMessageBox::Yes) {
			Database::Instance()->delete_database(dbname);
			refreshList();
		}
	}
}


void DBBrowser::databaseSelected(QListWidgetItem * item) {
// 	QString dbname = item->data(Qt::UserRole).toString();
// 	openZarlock(dbname);
	if (item) {
		dbb_load->setEnabled(true);
		dbb_delete->setEnabled(true);
	}
}

void DBBrowser::refreshList(sortflags sort, orderflags order) {
	dbb_list->clear();

	QVector<DBBrowser::DBListEntry> dble = fetchDBEntryList(sort, order);

	for (int i = 0; i < dble.size(); ++i) {
		dbb_list->setIconSize(QSize(64, 64));
		QListWidgetItem * witem = new QListWidgetItem(QIcon(":/resources/icons/user-home.png"), dble[i].infoContent);
		witem->setData(Qt::UserRole, dble[i].fileInfo.baseName());
		dbb_list->addItem(witem);
	}
}

QString DBBrowser::newDatabaseCreator(bool autoopen) {
	bool wasOK = false;
	QString dbname = QInputDialog::getText(this, tr("Create new database"), tr("Database name"), QLineEdit::Normal, "", &wasOK);

	if (wasOK && autoopen) {
// 		Database::Instance().open_database(dbname, true);
// 		openZarlock(dbname);
		Database::Instance()->create_database(dbname);
		refreshList();

		return dbname;
	}

	return QString();
}

QVector<DBBrowser::DBListEntry> DBBrowser::fetchDBEntryList(DBBrowser::sortflags sort, DBBrowser::orderflags order) {
	int sflag = 0x00;
	switch (sort) {
		case s_name:
			sflag |= QDir::Name;
			break;
		case s_time:
			sflag |= QDir::Time;
			break;
		case s_size:
			sflag |= QDir::Size;
			break;
	}
	switch (order)  {
		case o_asc:
			break;
		case o_dsc:
			sflag |= QDir::Reversed;
			break;
	}

	QStringList filters;
	filters << "*" % Database::dbfilext;

	QDir dir(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::SortFlag(sflag));
	dir.setNameFilters(filters);

	QVector<DBListEntry> dble;

	QFileInfoList list = dir.entryInfoList();

	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QString fname = fileInfo.baseName();

		QString finfo = dbInfoName(fname);

		DBListEntry le;
		le.fileInfo = fileInfo;
		le.infoContent = finfo;
		dble.push_back(le);
	}

	return dble;
}

QString DBBrowser::dbInfoName(const QString& dbname) {
	QFile infoFile(Database::fileFromDBName(dbname, true) % ".info");
	QString finfo = dbname;
	if (infoFile.exists()) {
		infoFile.open(QIODevice::ReadOnly);
		finfo = QString::fromUtf8(infoFile.readAll());
		infoFile.close();
	}

	return finfo;
}


#include "DBBrowser.moc"