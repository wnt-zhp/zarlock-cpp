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


#ifndef DBIMPORTWIDGET_H
#define DBIMPORTWIDGET_H

#include "ui_DBImportWidget.h"
#include "DBExportCommon.h"

class DBImportWidget : public QWidget, public Ui::DBImportWidget {
Q_OBJECT
public:
	DBImportWidget(QWidget * parent = NULL);
	virtual ~DBImportWidget();

public slots:
	virtual void resetForm();
	void setAdvancedMode(bool advanced);
	void setTargetCurrent();
	void setTargetDatabase(int idx);
	void setTargetNew(const QString & newdb);

private slots:
	void validateForm();
	void selectImportFile();
	virtual void doImport();

	void inputFileChanged();
	void existingDBChanged();
	void newDBChanged();

	void targetSelected();

private:
	DBExportCommon::DBBuffer buff;

	QString datafile;
	QString infofile;
	bool initial;
};

#endif // DBIMPORTWIDGET_H
