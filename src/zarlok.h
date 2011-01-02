#ifndef zarlok_H
#define zarlok_H

#include <QtGui/QMainWindow>
#include <QtGui/QDataWidgetMapper>
#include <QtGui/QDialog>
#include <QtGui/QMenu>
#include <QtCore/QString>
#include <QtSql/QSqlRelationalDelegate>

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

// 	QSqlTableModel * model_prod;
// 	QSqlTableModel * model_batch;

	ProductsTableModel * model_prod;
	BatchTableModel * model_batch;
	QSqlRelationalDelegate * model_batch_delegate;
	QSqlQueryModel * model_batchbyid;

	QDialog pop;
	AddProductsRecordWidget * aprw;
	AddBatchRecordWidget * abrw;
};

#endif // zarlok_H
