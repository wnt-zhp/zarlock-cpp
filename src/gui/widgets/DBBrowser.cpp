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

DBBrowser::DBBrowser() {
	setupUi(this);
	connect(dbb_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(dbb_list_selected(QListWidgetItem *)));
	connect(dbb_new, SIGNAL(clicked(bool)), this, SLOT(dbb_new_clicked(bool)));

	dbb_delete->setEnabled(false);

	dbb_new->setIcon( dbb_new->style()->standardIcon(QStyle::SP_FileIcon) );
	dbb_delete->setIcon( dbb_delete->style()->standardIcon(QStyle::SP_TrashIcon) );
	dbb_quit->setIcon( dbb_quit->style()->standardIcon(QStyle::SP_DialogCloseButton) );
	dbb_load->setIcon( dbb_load->style()->standardIcon(QStyle::SP_DialogOpenButton) );

	reload_list();
}

DBBrowser::~DBBrowser() {
	PR(666);
	while (dbb_list->count()) {
		QListWidgetItem * item = dbb_list->item(0);
		dbb_list->removeItemWidget(item);
		delete item;
	}
	PR(667);
}

void DBBrowser::dbb_list_selected(QListWidgetItem * item) {
	QString dbname = item->data(Qt::UserRole).toString();
	openDBName(dbname);
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
		int pos = fname.lastIndexOf(".db");
		fname.remove(pos, 3);

		QListWidgetItem * witem = new QListWidgetItem();
		witem->setData(Qt::UserRole, fname);
		witem->setSizeHint(QSize(400, 80));
		witem->setIcon(QIcon(QPixmap(QSize(100,100))));
		dbb_list->addItem(witem);

		DBItemWidget * w = new DBItemWidget();
		w->setDBName(fname);

// 		dbb_list->setIconSize(QSize(50, 50));
		dbb_list->setItemWidget(witem, w);
	}
}

bool DBBrowser::openDBName(const QString& dbname) {
	QFile dbfile(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
					dbname + QString(".db"));

	if  (dbfile.exists()) {
// 	PR(dbname.toStdString());
		emit dbb_database(dbname);
	} else {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name %1 doesn't exists!").arg(dbname));
		msgBox.setInformativeText("Do you want to create new database?");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();
		if (ret == QMessageBox::Yes) {
			if (createDBName(dbname))
				emit dbb_database(dbname);
		}
	}
	return true;
}

bool DBBrowser::createDBName(const QString & dbname) {
	if (dbname.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name is empty."));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
		return false;
	} else {
		QDir dbsavepath(QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB));
		if (!dbsavepath.exists())
			dbsavepath.mkpath(dbsavepath.absolutePath());

		QString dbfile = QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
						dbname + QString(".db");
// 		dbfile.
		PR(dbfile.toStdString());
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
			file.open(QIODevice::ReadWrite | QIODevice::Truncate);
		} else {
			file.open(QIODevice::ReadWrite);
		}
		file.close();
		Database & db = Database::Instance();
		db.open_database(dbfile, true);
		db.close_database();
	}
	return true;
}

void DBBrowser::dbb_new_clicked(bool) {
// 	Ui::NewDatabase * dialog = new Ui::NewDatabase();
// 	dialog->exec();
	while (true) {
		QString dbname = QInputDialog::getText(this, tr("New database"), tr("New database name"));
		bool ret = createDBName(dbname);
		if (ret) break;
	}

	reload_list();
}

#include "DBBrowser.moc"