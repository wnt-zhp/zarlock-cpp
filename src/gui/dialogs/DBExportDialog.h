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


#ifndef DBEXPORTDIALOG_H
#define DBEXPORTDIALOG_H

// #include <QDate>
// #include <QString>
// #include <QVector>

#include "ProgramSettings.h"
#include "ui_DBExportDialog.h"
#include <QCompleter>

class DBExportDialog : public QDialog, public Ui::DBExportDialog {
	Q_OBJECT
public:
	DBExportDialog(QDialog * parent = NULL);
	virtual ~DBExportDialog();

public slots:
	virtual int exec();
	virtual void resetExportForm();
	virtual void resetImportForm();

private slots:
	virtual void validateExportForm();
	virtual void validateImportForm();

	virtual void selectExportDirectory();
	virtual void selectImportFile();

	virtual void doExport();
	virtual void doImport();

private:
	void convertToCharArray(int number, char * array, int size);
	void convertFromCharArray(int & number, char * array, int size);

	void uncompressImportFile();

private:
	static const QString exportArchName;
	static const QString exportArchExt;

	static QSettings appSettings;

	QCompleter * dircompleter;

	// import fields
	char has_info;
	int size_iname, size_inname;
	int size_idata, size_indata;
	QByteArray iname, inname;
	QByteArray idata, indata;
};

#endif /* DBEXPORTDIALOG_H */
