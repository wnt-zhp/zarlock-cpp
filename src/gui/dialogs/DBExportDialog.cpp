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

const QStringList DBExportDialog::exportArchNames = {"No archive"};//, "ZIP", "TAR Gzip", "TAR Bzip2"};
const QStringList DBExportDialog::exportArchCodes = {""};//, "zip", "tgz", "tbz2"};
const QStringList DBExportDialog::exportArchExts = {".sqlite3"};//, ".zip", ".tgz", ".tbz2"};

DBExportDialog::DBExportDialog(QDialog * /*parent*/) : Ui::DBExportDialog(), dircompleter(NULL) {
	setupUi(this);

	for (int i = 0; i < exportArchCodes.size(); ++i) {
		cb_format->addItem(exportArchNames[i], exportArchCodes[i]);
	}

	connect(push_dir, SIGNAL(clicked(bool)), this, SLOT(selectDirectory()));

	connect(push_export, SIGNAL(clicked(bool)), this, SLOT(doExport()));
	connect(push_reset, SIGNAL(clicked(bool)), this, SLOT(reset()));
	connect(push_cancel, SIGNAL(clicked(bool)), this, SLOT(close()));

	connect(cb_format, SIGNAL(currentIndexChanged(int)), this, SLOT(validateFormatSelection(int)));

	connect(line_dir, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	connect(line_filename, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	connect(cb_format, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
}

DBExportDialog::~DBExportDialog() {
	DII();

	if (dircompleter)
		delete dircompleter->model();

	delete dircompleter;
}

void DBExportDialog::validateFormatSelection(int index) {
	chb_info->setDisabled(index == 0);

	validateForm();
}

void DBExportDialog::validateForm() {
	int idx = cb_format->currentIndex();

	QString str_dir = line_dir->text();
// 	if (str_dir.isEmpty())
	str_dir.append("/");

	QString ofilename;
	QString str_fn = line_filename->text();
	if (!str_fn.isEmpty()) {
		ofilename =  str_dir % str_fn % exportArchExts[idx];
		push_export->setEnabled(true);
	} else {
		push_export->setEnabled(false);
	}

	QFile file(ofilename);
	line_output->setText(file.fileName());
}

void DBExportDialog::selectDirectory() {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	dialog.setDirectory(line_dir->text());

	if (!dialog.exec())
		return;

// 	QString dir = dialog.getExistingDirectory();
	QString dir = dialog.selectedFiles().at(0);

	line_dir->setText(dir);
}


void DBExportDialog::reset() {
	chb_info->setChecked(true);
	chb_info->setDisabled(true);

	push_export->setDisabled(true);

	line_dir->setText(QDir::homePath());

// 	cb_format->setCurrentIndex(1);

	if (dircompleter)
		delete dircompleter->model();
	delete dircompleter;

	dircompleter = new QCompleter(this);
	// 	dircompleter->setMaxVisibleItems(maxVisibleSpinBox->value());
	dircompleter->setMaxVisibleItems(10);
	dircompleter->setCaseSensitivity(Qt::CaseInsensitive);
	dircompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	dircompleter->setWrapAround(true);

	QFileSystemModel * fsmodel = new QFileSystemModel(dircompleter);
	fsmodel->setRootPath(line_dir->text());
	fsmodel->setFilter(QDir::Dirs);

	dircompleter->setModel(fsmodel);

	line_dir->setCompleter(dircompleter);

	QString expdbfname = Database::Instance()->openedDatabase() % QDateTime::currentDateTime().toString("_yyMMdd_hhmmss");

	line_filename->setText(expdbfname);
}

int DBExportDialog::exec() {
	// Here prepare dialog to display
	reset();
	validateForm();

	// If no action required then exit
	int ret = QDialog::exec();

	if (!ret)
		return ret;

	// if action required then do it here

	// Exit from dialog
	return ret;
}

void DBExportDialog::doExport() {
	Database * db = Database::Instance();
	QString ifile = db->fileFromDBName(db->openedDatabase(), true, true);
	QString ofile = line_output->text();
	
	if (QFile::exists(ofile)) {
		int ret = QMessageBox::question(this, tr("File exists"), tr("File %1 exists. Do you want overwrite it?").arg(ofile), QMessageBox::Yes | QMessageBox::No);
		if (ret == QMessageBox::Yes) {
			if (!QFile::remove(ofile)) {
				QMessageBox::critical(this, tr("File remove"), tr("Unable to overwrite file %1. Export aborted.").arg(ofile), QMessageBox::Ok);
				reject();
			}
		} else {
			return;
		}
	}

	switch (cb_format->currentIndex()) {
		case 0:
			if (!QFile::copy(ifile, ofile)) {
				QMessageBox::critical(this, tr("File copy"), tr("Unable to overwrite file %1. Export aborted.").arg(ofile), QMessageBox::Ok);
				reject();
			}
			break;
		default:
			break;
	}

	this->accept();
}

#include "DBExportDialog.moc"