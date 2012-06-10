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

#ifndef zarlok_H
#define zarlok_H

#include "Database.h"

#include "TabProductsWidget.h"
#include "TabBatchWidget.h"
#include "TabDistributorWidget.h"
#include "TabMealWidget.h"

#include "ui_MainWindow.h"

#include "DBItemWidget.h"
#include "DimmingMessage.h"

class SettingsDialog;
class zarlok : public QMainWindow, private Ui::MainWindow {
Q_OBJECT
public:
	zarlok();
	virtual ~zarlok();

private:
	void activateUi(bool activate = true);

	void writeSettings();
	void readSettings();

	void updateAppTitle();

	void closeEvent(QCloseEvent *event);

signals:
	void exitZarlok();
	void switchDB();

private slots:
	void doSwitchDB();
	void doExitZarlok();
	bool doCampSettings();

	void printDailyReport();

	void about();
	void settings();

// 	void tabChanged(int index);
	void db2update();

	virtual void doCreateSMreports();
	virtual void doCreateKMreports();
	virtual void doCreateZZReports();
	virtual void doBrowseReports();
	

private:
	Database * db;

	TabProductsWidget * tpw;
	TabBatchWidget * tbw;
	TabDistributorWidget * tdw;
	TabMealWidget * tmw;

	QToolBar * toolbar;
	QToolBar * dbtoolbar;
	QToolButton * tools;

	QAction * actionQuit;
	QAction * actionSettings;
	QAction * actionAbout;

	QAction * actionPrintReport;
	QAction * actionSaveDB;

	QAction * actionSwitchDB;
	QAction * actionConfigDB;

	QAction * actionSyncDB;
	QAction * actionCreateSMrep;
	QAction * actionCreateKMrep;
	QAction * actionCreateZZrep;
	QAction * actionBrowseReports;

	DBItemWidget * dbiw;

	DimmingMessage * dw;

	SettingsDialog * appsettings;
};

#endif // zarlok_H
