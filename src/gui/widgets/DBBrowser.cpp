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

#include <QMessageBox>
#include <QInputDialog>

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
	}

	globals::appSettings->beginGroup("Database");
	recentDB = globals::appSettings->value("RecentDatabase").toString();
	globals::appSettings->endGroup();
	if (!recentDB.isEmpty()) {
		openZarlock(recentDB);
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

void DBBrowser::openZarlock(const QString & dbname) {
	if (Database::Instance().open_database(dbname)) {
		this->setVisible(false);

		globals::appSettings->beginGroup("Database");
		globals::appSettings->setValue("RecentDatabase", dbname);
		globals::appSettings->endGroup();

		z = new zarlok();
		z->show();

// 		connect(z, SIGNAL(destroyed()), this, SLOT(closeZarlock()));
		connect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
	}
}

void DBBrowser::closeZarlock() {
	FPR(__func__);
	if (z) {
// 		Database::Instance().close_database();
		disconnect(z, SIGNAL(exitZarlok()), this, SLOT(closeZarlock()));
		delete z;
		z = NULL;
	}
	this->setVisible(true);
	Database::Destroy();
}

// openZarlock();
		
void DBBrowser::dbb_list_selected(QListWidgetItem * item) {
	QString dbname = item->data(Qt::UserRole).toString();
	openZarlock(dbname);
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

		dbb_list->setIconSize(QSize(64, 64));
		QListWidgetItem * witem = new QListWidgetItem(QIcon(":/resources/icons/user-home.png"), finfo);
		witem->setData(Qt::UserRole, fname);
		dbb_list->addItem(witem);
	}
}

void DBBrowser::newDatabaseCreator(bool autoopen) {
	bool wasOK = false;
	QString dbname = QInputDialog::getText(this, tr("Create new database"), tr("Database name"), QLineEdit::Normal, "", &wasOK);

	if (wasOK && autoopen) {
// 		Database::Instance().open_database(dbname, true);
		openZarlock(dbname);
		refreshList();
	} else
		this->setVisible(true);
}

#include "DBBrowser.moc"