#ifndef zarlok_H
#define zarlok_H

#include "Database.h"

#include "TabProductsWidget.h"
#include "TabBatchWidget.h"
#include "TabDistributorWidget.h"
#include "TabMealWidget.h"

#include "ui_MainWindow.h"

#include "DBItemWidget.h"

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
	void doCampSettings();

	void printDailyReport();

	void about();

// 	void tabChanged(int index);
	void db2update();

private:
	Database * db;

	TabProductsWidget * tpw;
	TabBatchWidget * tbw;
	TabDistributorWidget * tdw;
	TabMealWidget * tmw;

	QToolBar * toolbar;
	QToolBar * dbtoolbar;

	QAction * actionQuit;
	QAction * actionAbout;
	QAction * actionPrintReport;
	QAction * actionSaveDB;

	QAction * actionSwitchDB;
	QAction * actionConfigDB;
	DBItemWidget * dbiw;
};

#endif // zarlok_H
