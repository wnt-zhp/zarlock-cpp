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
};

#endif // zarlok_H
