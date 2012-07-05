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

const QString DBExportDialog::exportArchName = QObject::tr("Zarlok archive");
const QString DBExportDialog::exportArchExt = ".zar";

DBExportDialog::DBExportDialog(QDialog * /*parent*/) : Ui::DBExportDialog(), dircompleter(NULL) {
	setupUi(this);

	// Export
	connect(exp_push_dir, SIGNAL(clicked(bool)), this, SLOT(selectExportDirectory()));
	connect(exp_line_dir, SIGNAL(textChanged(QString)), this, SLOT(validateExportForm()));
	connect(exp_line_filename, SIGNAL(textChanged(QString)), this, SLOT(validateExportForm()));
	
	connect(exp_push_export, SIGNAL(clicked(bool)), this, SLOT(doExport()));
	connect(exp_push_reset, SIGNAL(clicked(bool)), this, SLOT(resetExportForm()));

	// Import
	connect(imp_push_file, SIGNAL(clicked(bool)), this, SLOT(selectImportFile()));
	connect(imp_line_file, SIGNAL(textChanged(QString)), this, SLOT(validateImportForm()));

	connect(imp_push_import, SIGNAL(clicked(bool)), this, SLOT(doImport()));
	connect(imp_push_reset, SIGNAL(clicked(bool)), this, SLOT(resetImportForm()));

	connect(push_exit, SIGNAL(clicked(bool)), this, SLOT(close()));
}

DBExportDialog::~DBExportDialog() {
	DII();

	if (dircompleter)
		delete dircompleter->model();

	delete dircompleter;
}

void DBExportDialog::validateExportForm() {

	QString str_dir = exp_line_dir->text();
// 	if (str_dir.isEmpty())
	str_dir.append("/");

	QString ofilename;
	QString str_fn = exp_line_filename->text();
	if (!str_fn.isEmpty()) {
		ofilename =  str_dir % str_fn % exportArchExt;
		exp_push_export->setEnabled(true);
	} else {
		exp_push_export->setEnabled(false);
	}

	QFile file(ofilename);
	exp_line_output->setText(file.fileName());
}

void DBExportDialog::resetExportForm() {
	exp_chb_info->setChecked(true);
	exp_chb_info->setDisabled(true);
	
	exp_push_export->setDisabled(true);
	
	exp_line_dir->setText(QDir::homePath());
	
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
	fsmodel->setRootPath(exp_line_dir->text());
	fsmodel->setFilter(QDir::Dirs);
	
	dircompleter->setModel(fsmodel);
	
	exp_line_dir->setCompleter(dircompleter);
	
	QString expdbfname = Database::Instance()->openedDatabase() % QDateTime::currentDateTime().toString("_yyMMdd_hhmmss");
	
	exp_line_filename->setText(expdbfname);
}

void DBExportDialog::doExport() {
	Database * db = Database::Instance();
	QString ifilestr = db->fileFromDBName(db->openedDatabase(), true, true);
	QString infilestr = db->fileFromDBName(db->openedDatabase(), true, false) % ".info";
	QString ofilestr = exp_line_output->text();

	QFile ofile(ofilestr);

	if (ofile.exists()) {
		int ret = QMessageBox::question(this, tr("File exists"), tr("File %1 exists. Do you want overwrite it?").arg(ofilestr), QMessageBox::Yes | QMessageBox::No);
		if (ret != QMessageBox::Yes)
			return;
	}

	ofile.open(QIODevice::WriteOnly | QIODevice::Truncate);

	if (!ofile.isOpen()) {
		QMessageBox::critical(this, tr("File open"), tr("Unable to overwrite file %1. Export aborted.").arg(ofilestr), QMessageBox::Ok);
		return;
	}

	QFile ifile(ifilestr);
	QFile infile(infilestr);

	if (!ifile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, tr("Input file open"), tr("Unable to open source file %1. Export aborted.").arg(ifilestr), QMessageBox::Ok);
		return;
	}

	char has_info = exp_chb_info->isChecked();

	if (has_info and !infile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, tr("Input info file open"), tr("Unable to open source file %1. Export aborted.").arg(infilestr), QMessageBox::Ok);
		return;
	}

	const char int_size = 4;
	char converted_size_array[int_size];

	ofile.write(&int_size, 1);
	ofile.write(&has_info, 1);

	// export database file
	QByteArray compressedFileName = qCompress(ifile.fileName().toAscii());
	int size = compressedFileName.size();
	convertToCharArray(size, converted_size_array, int_size);

	ofile.write(converted_size_array, int_size);
	ofile.write(compressedFileName);
	
	QByteArray compressedData = qCompress(ifile.readAll());
	size = compressedData.size();
	convertToCharArray(size, converted_size_array, int_size);

	ofile.write(converted_size_array, int_size);
	ofile.write(compressedData);

	if (has_info) {
		QByteArray compressedFileName = qCompress(infile.fileName().toAscii());
		long size = compressedFileName.size();
		
		convertToCharArray(size, converted_size_array, int_size);
		ofile.write(converted_size_array, int_size);
		ofile.write(compressedFileName);
		
		QByteArray compressedData = qCompress(infile.readAll());
		size = compressedData.size();
		convertToCharArray(size, converted_size_array, int_size);
		
		ofile.write(converted_size_array, int_size);
		ofile.write(compressedData);
	}

	ofile.close();
	ifile.close();
	infile.close();
}

void DBExportDialog::validateImportForm() {
	if (!imp_line_file->text().isEmpty()) {
		uncompressImportFile();

		imp_line_dbname->setText(iname);
		imp_line_infoname->setText(indata);

		imp_chb_info->setChecked(has_info);
	}

	imp_push_import->setEnabled(!imp_line_file->text().isEmpty());
}

void DBExportDialog::resetImportForm() {

}

void DBExportDialog::doImport() {
	Database * db = Database::Instance();
	QString ofilestr = db->fileFromDBName(db->openedDatabase(), true, true);
	QString onfilestr = db->fileFromDBName(db->openedDatabase(), true, false) % ".info";

	QFile ofile(ofilestr);
	QFile onfile(onfilestr);

	if (ofile.exists()) {
		int ret = QMessageBox::question(this, tr("File exists"), tr("File %1 exists. Do you want overwrite it?").arg(ofilestr), QMessageBox::Yes | QMessageBox::No);
		if (ret != QMessageBox::Yes)
			return;
	}

	ofile.open(QIODevice::WriteOnly | QIODevice::Truncate);

	if (!ofile.isOpen()) {
		QMessageBox::critical(this, tr("File open"), tr("Unable to overwrite file %1. Export aborted.").arg(ofilestr), QMessageBox::Ok);
		return;
	}

	char import_info = imp_chb_info->isChecked();
	ofile.write(idata);

	// import info file
	if (has_info and import_info) {
		if (!onfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QMessageBox::critical(this, tr("File open"), tr("Unable to overwrite file %1. Export aborted.").arg(onfilestr), QMessageBox::Ok);
		} else {
			onfile.write(indata);
			onfile.close();
		}
	}

	ofile.close();

	db->rebuild_models();
}

int DBExportDialog::exec() {
	// Here prepare dialog to display
	resetExportForm();
	validateExportForm();

	resetImportForm();
	validateImportForm();
	
	// If no action required then exit
	int ret = QDialog::exec();
	
	if (!ret)
		return ret;
	
	// if action required then do it here
		
		// Exit from dialog
		return ret;
}

void DBExportDialog::selectExportDirectory() {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);
	
	dialog.setDirectory(exp_line_dir->text());
	
	if (!dialog.exec())
		return;
	
	// 	QString dir = dialog.getExistingDirectory();
		QString dir = dialog.selectedFiles().at(0);
		
	exp_line_dir->setText(dir);
}

void DBExportDialog::selectImportFile() {
	QFileDialog dialog(this);
	dialog.setNameFilter(QString("%1 (*%2)").arg(exportArchName).arg(exportArchExt));

// 	dialog.setDirectory(exp_line_dir->text());

	if (!dialog.exec())
		return;

// 	QString dir = dialog.getExistingDirectory();
	QString file = dialog.selectedFiles().at(0);

	imp_line_file->setText(file);
}

void DBExportDialog::uncompressImportFile() {
	QString ifilestr = imp_line_file->text();
	QFile ifile(ifilestr);

	if (!ifile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(this, tr("Input file open"), tr("Unable to open source file %1. Export aborted.").arg(ifilestr), QMessageBox::Ok);
		return;
	}

	has_info = 0;
	iname.clear();
	inname.clear();
	idata.clear();
	indata.clear();
	
	char int_size = 0;
	int size = 0;
	
	ifile.read(&int_size, 1);
	ifile.read(&has_info, 1);

	char * converted_size_array = new char[int_size];

	// import database file
	ifile.read(converted_size_array, int_size);
	convertFromCharArray(size, converted_size_array, int_size);
	iname = qUncompress(ifile.read(size));

	ifile.read(converted_size_array, int_size);
	convertFromCharArray(size, converted_size_array, int_size);
	idata = qUncompress(ifile.read(size));

	// import info file
	if (has_info) {
		ifile.read(converted_size_array, int_size);
		convertFromCharArray(size, converted_size_array, int_size);
		inname = qUncompress(ifile.read(size));

		ifile.read(converted_size_array, int_size);
		convertFromCharArray(size, converted_size_array, int_size);
		indata = qUncompress(ifile.read(size));
	}

	ifile.close();
	delete [] converted_size_array;
}

void DBExportDialog::convertToCharArray(int number, char * array, int size) {
	for (int i = 0; i < size; ++i) {
		array[i] = ((number >> (8*i)) & 0xff);
	}
}

void DBExportDialog::convertFromCharArray(int & number, char * array, int size) {
	number = 0;
	for (int i = 0; i < size; ++i) {
		number = number | ( int(array[i]) << (8*i) );
	}
}

#include "DBExportDialog.moc"