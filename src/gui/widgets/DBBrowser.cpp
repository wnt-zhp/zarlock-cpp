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

#include "DBBrowser.h"
#include "globals.h"
#include "config.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QFile>

#include "Database.h"

DBBrowser::DBBrowser() {
	setupUi(this);
	connect(dbb_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(dbb_list_selected(QListWidgetItem*)));
	connect(dbb_new, SIGNAL(clicked(bool)), this, SLOT(dbb_new_clicked(bool)));

	dbb_delete->setEnabled(false);

	reload_list();
}

DBBrowser::~DBBrowser() {
}

void DBBrowser::dbb_list_selected(QListWidgetItem * item) {
	QString dbname = item->data(Qt::DisplayRole).toString();
// 	PR(dbname.toStdString());
	emit dbb_database(dbname);
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
		dbb_list->addItem(fname);
	}
}

void DBBrowser::dbb_new_clicked(bool) {
// 	Ui::NewDatabase * dialog = new Ui::NewDatabase();
// 	dialog->exec();
	while (true) {

	QString name = QInputDialog::getText(this, tr("New database"), tr("New database name"));

	if (name.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText(tr("The database name is empty."));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.exec();
	} else {
		QString fname = QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
						name + QString(".db");
		PR(fname.toStdString());
		QFile file(fname);
		if (file.exists()) {
			QMessageBox msgBox;
			msgBox.setText(tr("The database name already exists."));
			msgBox.setInformativeText("Do you want to overwrite existing database?");
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			msgBox.setDefaultButton(QMessageBox::Save);
			int ret = msgBox.exec();

			if (ret == QMessageBox::Cancel)
				break;
			if (ret == QMessageBox::Discard)
				continue;
			file.open(QIODevice::ReadWrite | QIODevice::Truncate);
		} else {
			file.open(QIODevice::ReadWrite);
		}
		file.close();
		Database & db = Database::Instance();
		db.open_database(fname, true);
		db.close_database();
	}
	break;

	}

	reload_list();
}

#include "DBBrowser.moc"