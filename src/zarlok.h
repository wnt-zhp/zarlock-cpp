#ifndef zarlok_H
#define zarlok_H

#include <QtSql>

#include "Database.h"

#include "AddDistributorRecordWidget.h"

#include "TabProductsWidget.h"
#include "TabBatchWidget.h"
#include "TabDistributorWidget.h"
#include "TabMealWidget.h"

#include "BatchTableModel.h"
#include "DistributorTableModel.h"

#include "ui_MainWindow.h"

class zarlok : public QMainWindow, private Ui::MainWindow {
Q_OBJECT
public:
	zarlok(const QString & dbname);
	virtual ~zarlok();

private:
	void activateUi(bool activate = true);

	void writeSettings();
	void readSettings();

	void closeEvent(QCloseEvent *event);
signals:
	void exitZarlok();

private slots:
	void doExitZarlok();

	void printDailyReport();

	void about();

// 	void tabChanged(int index);
	void db2update();

private:
	Database & db;

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
};

#endif // zarlok_H
