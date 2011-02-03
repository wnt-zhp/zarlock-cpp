#ifndef zarlok_H
#define zarlok_H

#include <QtGui/QDataWidgetMapper>

#include <QtSql>

#include "Database.h"
#include "DBBrowser.h"

#include "AddProductsRecordWidget.h"
#include "AddBatchRecordWidget.h"
#include "AddDistributorRecordWidget.h"

#include "ProductsTableModel.h"
#include "BatchTableModel.h"
#include "DistributorTableModel.h"

#include "ui_MainWindow.h"

class zarlok : public QMainWindow, private Ui::MainWindow {
Q_OBJECT
public:
	zarlok();
	virtual ~zarlok();
	
private:
	void activateUi(bool activate = true);

private slots:
	void openDB(QString & dbname);
	void saveDB();
	void closeDB();
	void printDailyReport();

	void add_prod_record(bool newrec = true);
	void add_batch_record(bool newrec = true);
	void about();

	void set_filter(const QString & str);

private:
	QString dbname, dbfile;
	Database & db;
	QDataWidgetMapper * dwm_prod;

	ProductsTableModel * model_prod;
	BatchTableModel * model_batch;
	DistributorTableModel * model_dist;

	QSqlRelationalDelegate * model_batch_delegate;
	QSqlRelationalDelegate * model_dist_delegate;
	QSqlQueryModel * model_batchbyid;

	AddProductsRecordWidget * aprw;
	AddBatchRecordWidget * abrw;
	AddDistributorRecordWidget * adrw;

	DBBrowser * dbb;
};

#endif // zarlok_H
