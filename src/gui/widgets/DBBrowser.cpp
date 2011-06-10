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

DBBrowser::DBBrowser(bool firstrun) : db(Database::Instance()) {
	this->setVisible(false);

	setupUi(this);
	connect(dbb_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(dbb_list_selected(QListWidgetItem *)));
	connect(dbb_new, SIGNAL(clicked(bool)), this, SLOT(dbb_new_clicked(bool)));
	connect(dbb_quit, SIGNAL(clicked(bool)), this, SLOT(close()));

	dbb_delete->setEnabled(false);

	dbb_new->setIcon( dbb_new->style()->standardIcon(QStyle::SP_FileIcon) );
	dbb_delete->setIcon( dbb_delete->style()->standardIcon(QStyle::SP_TrashIcon) );
	dbb_quit->setIcon( dbb_quit->style()->standardIcon(QStyle::SP_DialogCloseButton) );
	dbb_load->setIcon( dbb_load->style()->standardIcon(QStyle::SP_DialogOpenButton) );

	reload_list();

	bool firstRun = false;
// 	LOG(fsettings.fileName().toStdString());

	if (!firstrun) {
		PR("First run! Welcome to żarłok.");
	} else {
		globals::appSettings->beginGroup("Database");
		recentDB = globals::appSettings->value("RecentDatabase").toString();
		globals::appSettings->endGroup();
	}

	if (!recentDB.isEmpty())
		openDBFile(recentDB);
}

DBBrowser::~DBBrowser() {
	while (dbb_list->count()) {
		QListWidgetItem * item = dbb_list->item(0);
		dbb_list->removeItemWidget(item);
		delete item;
	}
}

bool DBBrowser::openZarlock() {
	this->setVisible(false);
	z = new zarlok("");
	z->show();
// 	connect(z, SIGNAL(destroyed()), this, SLOT(closeZarlock()));
	connect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
}

bool DBBrowser::closeZarlock() {
// 	disconnect(z, SIGNAL(destroyed()), this, SLOT(closeZarlock()));
	disconnect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
	delete z;
	z = NULL;
	this->setVisible(true);
}

void DBBrowser::dbb_list_selected(QListWidgetItem * item) {
	QString dbname = item->data(Qt::UserRole).toString();
	openDBFile(dbname);
}

void DBBrowser::reload_list(int sort, int order) {
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

bool DBBrowser::openDBFile(const QString& dbname) {
	QFile dbfile(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
					dbname + QString(".db"));

	if  (dbfile.exists()) {
		openDB(dbfile.fileName());
	} else {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name %1 doesn't exists!").arg(dbname));
		msgBox.setInformativeText("Do you want to create new database?");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes) {
			if (createDBFile(dbname)) {
				openDB(dbfile.fileName());
			}
		} else
			return false;
	}

	globals::appSettings->beginGroup("Database");
	globals::appSettings->setValue("RecentDatabase", dbname);
	globals::appSettings->endGroup();

	return true;
}

bool DBBrowser::createDBFile(const QString & dbname) {
	if (dbname.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name is empty"));
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.exec();
		return false;
	} else {
		QString safename = dbname;
		safename.replace(QRegExp(QString::fromUtf8("[^a-zA-Z0-9_]")), "_");
		QDir dbsavepath(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));
		if (!dbsavepath.exists())
			dbsavepath.mkpath(dbsavepath.absolutePath());

		QString dbfile = QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
						safename + QString(".db");
		QString dbifile = QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
						safename + QString(".info");
// 		dbfile.
		QFile file(dbfile);
		if (file.exists()) {
			QMessageBox msgBox;
			msgBox.setText(tr("The database name '%1' already exists.").arg(dbname));
			msgBox.setInformativeText("Do you want to overwrite existing database?");
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Yes);
			int ret = msgBox.exec();

			if (ret == QMessageBox::Cancel)
				return true;
			if (ret == QMessageBox::No)
				return false;
		} 
		
		file.open(QIODevice::ReadWrite | QIODevice::Truncate);
		file.close();

		file.setFileName(dbifile);
		file.open(QIODevice::ReadWrite | QIODevice::Truncate);
		file.write(dbname.toUtf8());
		file.close();

		Database & db = Database::Instance();
		db.open_database(dbfile, true);
		db.close_database();
	}
	return true;
}

/**
 * @brief Slot - otwiera bazę danych po wywołaniu akcji z menu lub skrótu klawiatury
 *
 * @param recreate jeśli w bazie istnieją tabele, wpierw je usuń i stwórz na nowo
 * @param file nazwa pliku do otwarcia
 * @return bool
 **/
void DBBrowser::openDB(const QString & dbfile) {
	bool ret = db.open_database(dbfile, false);
	openZarlock();
// 	activateUi(ret);
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void DBBrowser::saveDB() {
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
	saveDB();
	db.close_database();
}

void DBBrowser::dbb_new_clicked(bool) {
	bool wasOK = false;
	QString dbname = QInputDialog::getText(this, tr("New database"), tr("New database name"),QLineEdit::Normal, "", &wasOK);
	if (wasOK && createDBFile(dbname))
		reload_list();
}

#include "DBBrowser.moc"