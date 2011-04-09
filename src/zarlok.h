#ifndef zarlok_H
#define zarlok_H

#include <QtGui/QDataWidgetMapper>

#include <QtSql>

#include "Database.h"
#include "DBBrowser.h"

#include "AddDistributorRecordWidget.h"

#include "TabProductsWidget.h"
#include "TabBatchWidget.h"
#include "TabDistributorWidget.h"

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

private slots:
	void openDB(const QString & dbname);
	void saveDB();
	void closeDB();
	void printDailyReport();

	void about();

	void tabChanged(int index);
	void db2update();

private:
	QString dbname, dbfile;
	Database & db;
	QDataWidgetMapper * dwm_prod;

	TabProductsWidget * tpw;
	TabBatchWidget * tbw;
	TabDistributorWidget * tdw;

	DBBrowser * dbb;

	QAction * actionQuit;
	QAction * actionAbout;
	QAction * actionPrintReport;
	QAction * actionSaveDB;
};

#endif // zarlok_H
