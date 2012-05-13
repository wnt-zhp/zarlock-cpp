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

// endif

#include "globals.h"
#include "config.h"

#include <QMessageBox>
#include <QInputDialog>
#include <getopt.h>

#include "Database.h"
#include "DBBrowser.h"

DBBrowser::DBBrowser(QWidget * parent): QWidget(parent), z(NULL), show_browser(false) {
	CI();

	this->setVisible(false);
	setupUi(this);
	connect(dbb_list, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(databaseSelected(QListWidgetItem *)));
	connect(dbb_new, SIGNAL(clicked(bool)), this, SLOT(newDatabaseCreator()));
	connect(dbb_quit, SIGNAL(clicked(bool)), this, SLOT(close()));

	dbb_delete->setEnabled(false);

	dbb_new->setIcon( QApplication::style()->standardIcon(QStyle::SP_FileIcon) );
	dbb_delete->setIcon( QApplication::style()->standardIcon(QStyle::SP_TrashIcon) );
	dbb_quit->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton) );
	dbb_load->setIcon( QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton) );

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

void DBBrowser::configure(int argc, char* argv[]) {
	int c;

	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"verbose", no_argument,       0, 'v'},
			{"vverbose", no_argument,       0, 'w'},
			{"quiet",   no_argument,       0, 'q'},
			/* These options don't set a flag.
			 *        We distinguish them by their indices. */
			{"browser",  no_argument,       0, 'b'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		
		c = getopt_long (argc, argv, "bvwq", 
						 long_options, &option_index);
		
		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'b':PR(c);
				show_browser = true;
// 				puts ("option -b\n");
				break;
			case 'v':
				globals::verbose_flag[0] = true;
				break;
			case 'w':
				globals::verbose_flag[1] = true;
				break;
			case 'q':
				globals::verbose_flag[0] = false;
				break;
			case '?':
				/* getopt_long already printed an error message. */
				break;
			default:
				abort ();
		}
	}

	/* Instead of reporting ‘--verbose’
	 * and ‘--brief’ as they are encountered,
	 * we report the final status resulting from them. */
// 	if (verbose_flag)
// 		puts ("verbose flag is set");

	/* Print any remaining command line arguments (not options). */
// 	if (optind < argc) {
// 		printf ("non-option ARGV-elements: ");
// 		while (optind < argc)
// 			printf ("%s ", argv[optind++]);
// 		putchar ('\n');
// 	}

// 	for (int i = 0; i < argc; ++i)
// 		PR(argv[i]);
}


void DBBrowser::goBrowser() {
	globals::appSettings->beginGroup("Database");
	QString recentDB = globals::appSettings->value("RecentDatabase", "").toString();
	globals::appSettings->endGroup();

	if (show_browser) {
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
}

// openZarlock();
		
void DBBrowser::databaseSelected(QListWidgetItem * item) {
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

// 	QFileInfoList list = dir.entryInfoList(QStringList({".db"}), QDir::Files);
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

#include "DBBrowser.moc"