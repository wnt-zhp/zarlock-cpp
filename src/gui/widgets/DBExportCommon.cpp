/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#include "DBExportCommon.h"

#include <QObject>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>

const QString DBExportCommon::exportArchName = QObject::tr("Zarlok archive");
const QString DBExportCommon::exportArchExt = ".zar";

void DBExportCommon::Compress(const QString& dbfilestr, const QString& infofilestr, DBBuffer * buff) {
	QFile ifile(dbfilestr);
	QFile infile(infofilestr);

	if (!ifile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, QObject::tr("Input file open"), QObject::tr("Unable to open source file %1. Export aborted.").arg(dbfilestr), QMessageBox::Ok);
		return;
	}

	buff->has_info = !infofilestr.isEmpty();

	if (buff->has_info and !infile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, QObject::tr("Input info file open"), QObject::tr("Unable to open source file %1. Export aborted.").arg(infofilestr), QMessageBox::Ok);
		return;
	}

	// export database file
	buff->dbname = qCompress(ifile.fileName().toAscii());
	buff->dbdata = qCompress(ifile.readAll());

	if (buff->has_info) {
		buff->infoname = qCompress(infile.fileName().toAscii());
		buff->infodata = qCompress(infile.readAll());
	}

	ifile.close();
	infile.close();
}

void DBExportCommon::ExportFile(const QString& archFile, DBExportCommon::DBBuffer* buff) {
	QFile ofile(archFile);

	if (ofile.exists()) {
		int ret = QMessageBox::question(NULL, QObject::tr("File exists"), QObject::tr("File %1 exists. Do you want overwrite it?").arg(archFile), QMessageBox::Yes | QMessageBox::No);
		if (ret != QMessageBox::Yes)
			return;
	}

	ofile.open(QIODevice::WriteOnly | QIODevice::Truncate);

	if (!ofile.isOpen()) {
		QMessageBox::critical(NULL, QObject::tr("File open"), QObject::tr("Unable to overwrite file %1. Export aborted.").arg(archFile), QMessageBox::Ok);
		return;
	}

	// basic info
	ofile.write(&exportIntSize, 1);
	ofile.write(&buff->has_info, 1);

	char converted_size_array[exportIntSize];

	// dbname
	convertToCharArray(buff->dbname.size(), converted_size_array, exportIntSize);
	ofile.write(converted_size_array, exportIntSize);
	ofile.write(buff->dbname);

	// dbdata
	convertToCharArray(buff->dbdata.size(), converted_size_array, exportIntSize);
	ofile.write(converted_size_array, exportIntSize);
	ofile.write(buff->dbdata);

	if (buff->has_info) {
		convertToCharArray(buff->infoname.size(), converted_size_array, exportIntSize);
		ofile.write(converted_size_array, exportIntSize);
		ofile.write(buff->infoname);

		// dbdata
		convertToCharArray(buff->infodata.size(), converted_size_array, exportIntSize);
		ofile.write(converted_size_array, exportIntSize);
		ofile.write(buff->infodata);
	}
}

void DBExportCommon::Uncompress(const QString& archFile, DBBuffer * buff) {
	QFile ifile(archFile);

	if (!ifile.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, QObject::tr("Input file open"), QObject::tr("Unable to open source file %1. Export aborted.").arg(archFile), QMessageBox::Ok);
		return;
	}

	buff->has_info = 0;
	buff->dbname.clear();
	buff->dbdata.clear();
	buff->infoname.clear();
	buff->infodata.clear();

	char int_size = 0;
	int size = 0;

	ifile.read(&int_size, 1);
	ifile.read(&buff->has_info, 1);

	char * converted_size_array = new char[int_size];

	// import database file
	ifile.read(converted_size_array, int_size);
	convertFromCharArray(size, converted_size_array, int_size);
	buff->dbname = ifile.read(size);

	ifile.read(converted_size_array, int_size);
	convertFromCharArray(size, converted_size_array, int_size);
	buff->dbdata = ifile.read(size);

	// import info file
	if (buff->has_info) {
		ifile.read(converted_size_array, int_size);
		convertFromCharArray(size, converted_size_array, int_size);
		buff->infoname = ifile.read(size);

		ifile.read(converted_size_array, int_size);
		convertFromCharArray(size, converted_size_array, int_size);
		buff->infodata = ifile.read(size);
	}

	ifile.close();
	delete [] converted_size_array;
}

void DBExportCommon::ImportFile(const QString& dbFile, const QString& infoFile, DBExportCommon::DBBuffer* buff) {
	QFile ofile(dbFile);
	QFile onfile(infoFile);

	if (ofile.exists()) {
		int ret = QMessageBox::question(NULL, QObject::tr("File exists"), QObject::tr("File %1 exists. Do you want overwrite it?").arg(dbFile), QMessageBox::Yes | QMessageBox::No);
		if (ret != QMessageBox::Yes)
			return;
	}

	ofile.open(QIODevice::WriteOnly | QIODevice::Truncate);

	if (!ofile.isOpen()) {
		QMessageBox::critical(NULL, QObject::tr("File open"), QObject::tr("Unable to overwrite file %1. Export aborted.").arg(dbFile), QMessageBox::Ok);
		return;
	}

	ofile.write(qUncompress(buff->dbdata));
	ofile.close();

	char import_info = !infoFile.isEmpty();

	// import info file
	if (buff->has_info and import_info) {
		if (!onfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QMessageBox::critical(NULL, QObject::tr("File open"), QObject::tr("Unable to overwrite file %1. Export aborted.").arg(infoFile), QMessageBox::Ok);
		} else {
			onfile.write(qUncompress(buff->infodata));
			onfile.close();
		}
	}
}

void DBExportCommon::convertToCharArray(int number, char * array, int size) {
	for (int i = 0; i < size; ++i) {
		array[i] = ((number >> (8*i)) & 0xff);
	}
}

void DBExportCommon::convertFromCharArray(int & number, char * array, int size) {
	number = 0;
	for (int i = 0; i < size; ++i) {
		number = number | ( int(array[i]) << (8*i) );
	}
}
