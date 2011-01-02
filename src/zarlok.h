#ifndef zarlok_H
#define zarlok_H

#include <QtGui/QDataWidgetMapper>

#include <QtSql>

#include "Database.h"
#include "AddProductsRecordWidget.h"
#include "AddBatchRecordWidget.h"

#include "ProductsTableModel.h"
#include "BatchTableModel.h"
#include "ui_MainWindow.h"

class zarlok : public QMainWindow, private Ui::MainWindow {
Q_OBJECT
public:
	zarlok(const QString & file = QString(), QWidget * parent = NULL);
	virtual ~zarlok();
	
// 	void show_window(const QString &);

private:
	void activateUi(bool activate = true);

private slots:
	inline void createDB() { openDB(true, QString()); };
	void openDB(bool recreate = false, const QString & file = QString());
	void saveDB();
	void printReport();

	void add_prod_record(bool newrec = true);
	void add_batch_record(bool newrec = true);
	void about();

	void set_filter(const QString & str);

private:
	Database & db;
	QDataWidgetMapper * dwm_prod;

	ProductsTableModel * model_prod;
	BatchTableModel * model_batch;
	QSqlRelationalDelegate * model_batch_delegate;
	QSqlQueryModel * model_batchbyid;

	AddProductsRecordWidget * aprw;
	AddBatchRecordWidget * abrw;
};

#endif // zarlok_H
