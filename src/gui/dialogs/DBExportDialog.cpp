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

// #include <QStringBuilder>

#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QStringBuilder>

#include "globals.h"
#include "DBExportDialog.h"
#include <Database.h>

DBExportDialog::DBExportDialog(QDialog * /*parent*/) : Ui::DBExportDialog(), dircompleter(NULL) {
	setupUi(this);

	// Export
	((QGridLayout *)layout())->addWidget(&dbexw, 0, 0);
	// Import
	((QGridLayout *)layout())->addWidget(&dbimw, 0, 2);

	connect(push_exit, SIGNAL(clicked(bool)), this, SLOT(close()));
}

DBExportDialog::~DBExportDialog() {
	DII();
}

int DBExportDialog::exec() {
	// Here prepare dialog to display
	dbexw.resetForm();
	dbimw.resetForm();

	// If no action required then exit
	int ret = QDialog::exec();

	if (!ret)
		return ret;

	// if action required then do it here

	// Exit from dialog
	return ret;
}

#include "DBExportDialog.moc"