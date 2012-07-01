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
	virtual void reset();

private slots:
	virtual void validateFormatSelection(int index);
	virtual void validateForm();
	virtual void selectDirectory();

	virtual void doExport();

private:
	static const QStringList exportArchNames;
	static const QStringList exportArchCodes;
	static const QStringList exportArchExts;

	static QSettings appSettings;

	QCompleter * dircompleter;
};

#endif /* DBEXPORTDIALOG_H */
