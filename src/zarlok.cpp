#include "zarlok.h"
#include "globals.h"
#include "config.h"

#include "DBReports.h"

#include <QDir>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>

#include <QFileDialog>
// #include <QPrintDialog>


// #include <QTextDocument>
// #include <QTextTable>
// #include <QAbstractTextDocumentLayout>

// public members

/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok(const QString & dbname) : QMainWindow(), db(Database::Instance()), dwm_prod(NULL),
	model_prod(NULL), model_batch_delegate(NULL), 
	model_dist_delegate(NULL), model_batchbyid(NULL) {

	dbb = new DBBrowser();

	setupUi(this);
	this->setWindowTitle(tr("Zarlok by Rafal Lalik [pre-demo version], 11.2010"));

	widget_add_products->setVisible(false);
	widget_add_batch->setVisible(false);
	widget_add_distributor->setVisible(true);

	aprw = new AddProductsRecordWidget(widget_add_products);
	abrw = new AddBatchRecordWidget(widget_add_batch);
	adrw = new AddDistributorRecordWidget(widget_add_distributor);

	activateUi(false);

	connect(dbb, SIGNAL(dbb_database(const QString&)), this, SLOT(openDB(const QString&)));
	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(closeDB()));

	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printDailyReport()));

	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(add_prod_record(bool)));
	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(aprw, SIGNAL(canceled(bool)), button_add_prod, SLOT(setChecked(bool)));

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(add_batch_record(bool)));
	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(abrw, SIGNAL(canceled(bool)), button_add_batch, SLOT(setChecked(bool)));

	PR(dbb);
	if (dbname.isEmpty())
		dbb->show();
	else {
// 		openDB(dbname);
		dbb->openDBName(dbname);
	}
}

zarlok::~zarlok() {
	saveDB();

	if (model_batch_delegate) delete model_batch_delegate;
	if (aprw) delete aprw;
	if (abrw) delete abrw;
	if (adrw) delete adrw;
}

void zarlok::set_filter(const QString& str) {
	QString q("select \"spec\", \"curr_qty\" from batch");
	q.append(str);
	if (table_batchbyid->model() != NULL) {
		((QSqlQueryModel *)table_batchbyid->model())->setQuery(q);
// 		model_batchbyid->setQuery(q);
	}
}

// private members

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void zarlok::activateUi(bool activate) {
	dbb->setVisible(!activate);
	this->setVisible(activate);
	MainTab->setVisible(activate);
// 	MainTab->setVisible(true);
	MainTab->setEnabled(activate);

	if (activate) {
		// products
		if ((model_prod = db.CachedProducts())) {
			table_products->setModel(model_prod);
			table_products->show();
			connect(edit_filter_prod, SIGNAL(textChanged(QString)), model_prod, SLOT(filterDB(QString)));
			aprw->update_model();
		}
		// batch
		if ((model_batch = db.CachedBatch())){
			table_batch->setModel(model_batch);
			if (model_batch_delegate) delete model_batch_delegate;
			model_batch_delegate = new QSqlRelationalDelegate(table_batch);
			table_batch->setItemDelegate(model_batch_delegate);
			table_batch->show();
			connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_batch, SLOT(filterDB(QString)));
			abrw->update_model();
		}
		if ((model_dist = db.CachedDistributor())){
			table_dist->setModel(model_dist);
			if (model_dist_delegate) delete model_dist_delegate;
			model_dist_delegate = new QSqlRelationalDelegate(table_dist);
			table_dist->setItemDelegate(model_dist_delegate);
			table_dist->show();
			connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_dist, SLOT(filterDB(QString)));
			adrw->update_model();
		}
// 		table_products->resizeColumnsToContents();
// 		table_batch->resizeColumnsToContents();
// 		if (dwm_prod) delete dwm_prod;
// 		dwm_prod = new QDataWidgetMapper;
// 		dwm_prod->setModel(model_prod);
// 		dwm_prod->addMapping(edit_products, 1);
// 		dwm_prod->toFirst();

// 		if (model_batchbyid != NULL) delete model_batchbyid;
// 		if (model_batchbyid = new QSqlQueryModel()) {
// 			model_batchbyid->setQuery("select \"spec\", \"curr_qty\" from batch");
// 			table_batchbyid->setModel(model_batchbyid);
// 			table_batchbyid->show();
// 		}
	}
}

/**
 * @brief Slot - otwiera bazę danych po wywołaniu akcji z menu lub skrótu klawiatury
 *
 * @param recreate jeśli w bazie istnieją tabele, wpierw je usuń i stwórz na nowo
 * @param file nazwa pliku do otwarcia
 * @return bool
 **/
void zarlok::openDB(const QString & dbname) {
	dbfile = QDir::homePath() + QString(ZARLOK_HOME ZARLOK_DB) +
					dbname + QString(".db");
	this->dbname = dbname;
	bool ret = db.open_database(dbfile, false);
	activateUi(ret);
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void zarlok::saveDB() {
	/*if (model_dist->isDirty())*/ model_dist->submitAll();
	/*if (model_batch->isDirty())*/ model_batch->submitAll();
	/*if (model_prod->isDirty())*/ model_prod->submitAll();
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void zarlok::closeDB() {
	saveDB();
	db.close_database();
	activateUi(false);
}

void zarlok::printDailyReport() {
	DBReports::printDailyReport(dbname, QDate::currentDate());
}


void zarlok::add_prod_record(bool newrec) {
	if (newrec) {
// 		table_products->setVisible(false);
		widget_add_products->setVisible(true);
	} else {
// 		table_products->setVisible(true);
		widget_add_products->setVisible(false);
	}
}

void zarlok::add_batch_record(bool newrec) {
	if (newrec) {
// 		table_products->setVisible(false);
		widget_add_batch->setVisible(true);
	} else {
// 		table_products->setVisible(true);
		widget_add_batch->setVisible(false);
	}
}

void zarlok::about() {
//      QLabel infoLabel->setText(tr("Invoked <b>Help|About</b>"));
     QMessageBox::about(this, tr("About Menu"),
             tr("The <b>Menu</b> example shows how to create "
                "menu-bar menus and context menus."));
}

#include "zarlok.moc"
