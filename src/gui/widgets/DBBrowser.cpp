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

// if LINUX

#include <dirent.h> 
#include <stdio.h> 

// endif

#include "globals.h"
#include "config.h"

#include "DBBrowser.h"
#include "DBItemWidget.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QFile>

#include "Database.h"

DBBrowser::DBBrowser(bool firstrun) {
	this->setVisible(false);
	setupUi(this);
	connect(dbb_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(dbb_list_selected(QListWidgetItem *)));
	connect(dbb_new, SIGNAL(clicked(bool)), this, SLOT(newDatabaseCreator()));
	connect(dbb_quit, SIGNAL(clicked(bool)), this, SLOT(close()));

	dbb_delete->setEnabled(false);

	dbb_new->setIcon( dbb_new->style()->standardIcon(QStyle::SP_FileIcon) );
	dbb_delete->setIcon( dbb_delete->style()->standardIcon(QStyle::SP_TrashIcon) );
	dbb_quit->setIcon( dbb_quit->style()->standardIcon(QStyle::SP_DialogCloseButton) );
	dbb_load->setIcon( dbb_load->style()->standardIcon(QStyle::SP_DialogOpenButton) );

	if (!firstrun) {
		globals::appSettings->beginGroup("Database");
		recentDB = globals::appSettings->value("RecentDatabase").toString();
		globals::appSettings->endGroup();
		if (!recentDB.isEmpty())
			openDB(recentDB);
	} else {
		PR("First run! Welcome to żarłok.");
		newDatabaseCreator();
	}

	refreshList();
}

DBBrowser::~DBBrowser() {
	FPR(__func__);
	delete z;
	while (dbb_list->count()) {
		QListWidgetItem * item = dbb_list->item(0);
		dbb_list->removeItemWidget(item);
		delete item;
	}
}

void DBBrowser::openZarlock() {
	this->setVisible(false);
	z = new zarlok("");
	z->show();
// 	connect(z, SIGNAL(destroyed()), this, SLOT(closeZarlock()));
	connect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
}

void DBBrowser::closeZarlock() {
	FPR(__func__);
	if (z) {
		disconnect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
		delete z;
		z = NULL;
	}
	this->setVisible(true);
	Database::Destroy();
}

/**
 * @brief Slot - otwiera bazę danych po wywołaniu akcji z menu lub skrótu klawiatury
 *
 * @param recreate jeśli w bazie istnieją tabele, wpierw je usuń i stwórz na nowo
 * @param file nazwa pliku do otwarcia
 * @return bool
 **/
void DBBrowser::openDB(const QString & dbname, bool createifnotexists) {
	QString dbfile;
	if (openDBFile(dbname, dbfile, createifnotexists)) {
		Database & db = Database::Instance();
		db.open_database(dbfile, false);

		globals::appSettings->beginGroup("Database");
		globals::appSettings->setValue("RecentDatabase", dbname);
		globals::appSettings->endGroup();

		openZarlock();
	}
}

void DBBrowser::dbb_list_selected(QListWidgetItem * item) {
	QString dbname = item->data(Qt::UserRole).toString();
	openDB(dbname);
}

void DBBrowser::refreshList(int sort, int order) {
	dbb_list->clear();

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

	QDir dir(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));

	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::SortFlag(sflag));

	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		QFileInfo fileInfo = list.at(i);
		QString fname = fileInfo.fileName();
		int pos = fname.lastIndexOf(".db", -3);
		if (pos < 0)
			continue;
		fname.remove(pos, 3);

		QFile infoFile(dir.absoluteFilePath(fname % ".info"));
		QString finfo = fname;
		if (infoFile.exists()) {
			infoFile.open(QIODevice::ReadOnly);
			finfo = QString::fromUtf8(infoFile.readAll());
			infoFile.close();
		}

		QListWidgetItem * witem = new QListWidgetItem();
		witem->setData(Qt::UserRole, fname);
		witem->setText(finfo);
		witem->setSizeHint(QSize(0, 60));
		witem->setIcon(QIcon(QPixmap(QSize(60,60))));
		dbb_list->addItem(witem);
// 		DBItemWidget * w = new DBItemWidget();
// 		w->setDBName(finfo);
// // 		dbb_list->setIconSize(QSize(50, 50));
// 		dbb_list->setItemWidget(witem, w);
	}
}

void DBBrowser::newDatabaseCreator(bool autoopen) {
	bool wasOK = false;
	QString dbname = QInputDialog::getText(this, tr("Create new database"), tr("Database name"),QLineEdit::Normal, "", &wasOK);

	if (wasOK && autoopen)
		openDB(dbname, true);
	else
		this->setVisible(true);
}

bool DBBrowser::openDBFile(const QString & dbname, QString & dbfile, bool createifnotexists) {
	QString safename = dbname;
	safename.replace(QRegExp(QString::fromUtf8("[^a-zA-Z0-9_]")), "_");

	QString dbfilenoext = QDir::homePath() % QString(ZARLOK_HOME ZARLOK_DB) % safename;
	dbfile = dbfilenoext % QString(".db");

	QFile fdbfile(dbfile);

	if  (fdbfile.exists()) {
		return true;
	} else {
		int create = QMessageBox::No;
		if (!createifnotexists) {
			QMessageBox msgBox;
			msgBox.setText(tr("The database name \"%1\" doesn't exists!").arg(dbname));
			msgBox.setInformativeText(tr("Do you want to create new database with name \"%1\"?").arg(dbname));
			msgBox.setIcon(QMessageBox::Question);
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::Yes);
			create = msgBox.exec();
		}
		if (createifnotexists || (create == QMessageBox::Yes))
			return createDBFile(dbname, dbfilenoext);
	}

	return false;
}

bool DBBrowser::createDBFile(const QString & dbname, const QString & dbfile) {
	if (dbname.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name is empty"));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return false;
	} else {
		QDir dbsavepath(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));
		if (!dbsavepath.exists())
			dbsavepath.mkpath(dbsavepath.absolutePath());

		QString datafile = dbfile % QString(".db");
		QString infofile = dbfile % QString(".info");

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

		Database & db = Database::Instance();
		db.open_database(datafile, true);
		db.close_database();
	}
	return true;
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void DBBrowser::saveDB() {
	Database & db = Database::Instance();
	db.CachedProducts()->submitAll();
	db.CachedBatch()->submitAll();
	db.CachedDistributor()->submitAll();

	db.updateBatchQty();
// 	actionSaveDB->setEnabled(false);
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void DBBrowser::closeDB() {
// 	activateUi(false);
	Database & db = Database::Instance();
	saveDB();
	db.close_database();
}

#include "DBBrowser.moc"