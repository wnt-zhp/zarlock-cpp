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
#include <QToolBar>

// public members

/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok(const QString & dbname) : QMainWindow(), db(Database::Instance()), dwm_prod(NULL),
										 tpw(NULL), tbw(NULL), tdw(NULL) {
	setupUi(this);

	this->setWindowTitle(tr("Zarlok by Rafal Lalik --- build: ").append(__TIMESTAMP__));

	dbb = new DBBrowser();

	QToolBar * toolbar;
	toolbar = addToolBar(tr("Main"));
	toolbar->setMovable(false);
	toolbar->setIconSize(QSize(32, 32));

	actionQuit = new QAction(QIcon(":/resources/icons/application-exit.png"), tr("Exit"), toolbar);
	actionPrintReport = new QAction(QIcon(":/resources/icons/printer.png"), tr("Print Report DB"), toolbar);
	actionSaveDB = new QAction(QIcon(":/resources/icons/svn-commit.png"), tr("Save DB"), toolbar);
	actionAbout = new QAction(QIcon(":/resources/icons/system-help.png"), tr("About"), toolbar);

	actionSaveDB->setEnabled(false);

	toolbar->addAction(actionQuit);
	toolbar->addAction(actionPrintReport);
	toolbar->addSeparator();
	toolbar->addAction(actionSaveDB);
	toolbar->addSeparator();
	toolbar->addAction(actionAbout);

	activateUi(false);

	connect(&db, SIGNAL(databaseDirty()), this, SLOT(db2update()));
	connect(dbb, SIGNAL(dbb_database(const QString&)), this, SLOT(openDB(const QString&)));

	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(closeDB()));

	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printDailyReport()));

// 	connect(MainTab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	if (dbname.isEmpty())
		dbb->show();
	else {
// 		openDB(dbname);
		dbb->openDBName(dbname);
	}
}

zarlok::~zarlok() {
	saveDB();
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

	if (activate) {
		tpw = new TabProductsWidget();
		tbw = new TabBatchWidget();
		tdw = new TabDistributorWidget();

		MainTab->addTab(tpw, "Products");
		MainTab->addTab(tbw, "Stock");
		MainTab->addTab(tdw, "Distribute");

		MainTab->setTabPosition(QTabWidget::North);

		MainTab->setVisible(activate);
		MainTab->setEnabled(activate);
	} else {
		if (tdw) delete tdw; tdw = NULL;
		if (tbw) delete tbw; tbw = NULL;
		if (tpw) delete tpw; tpw = NULL;
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
// 	/*if (model_dist->isDirty())*/ model_dist->submitAll();
// 	/*if (model_batch->isDirty())*/ model_batch->submitAll();
// 	/*if (model_prod->isDirty())*/ model_prod->submitAll();
	db.CachedProducts()->submitAll();
	db.CachedBatch()->submitAll();
	db.CachedDistributor()->submitAll();
	db.updateBatchQty();
	actionSaveDB->setEnabled(false);
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void zarlok::closeDB() {
	activateUi(false);
	saveDB();
	db.close_database();
}

void zarlok::printDailyReport() {
	DBReports::printDailyReport(dbname, QDate::currentDate());
}

void zarlok::about() {
//      QLabel infoLabel->setText(tr("Invoked <b>Help|About</b>"));
     QMessageBox::about(this, tr("About Menu"),
             tr("The <b>Menu</b> example shows how to create "
                "menu-bar menus and context menus."));
}

void zarlok::tabChanged(int index) {
	PR(index);
	if (actionSaveDB->isEnabled()) {
		QMessageBox msgBox;
		msgBox.setText(tr("Your haven't save your changes in database. Your new data will be lost."));
		msgBox.setInformativeText("Do you really want to change tab?");
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		int ret = msgBox.exec();
		if (ret == QMessageBox::No) {
			return;
		} else {
// 			MainTab->setCurrentIndex(index);
		}
	} else {
// 		MainTab->setCurrentIndex(index);
	}
}

void zarlok::db2update() {
	statusbar->showMessage(tr("You need to save your database!"));
	actionSaveDB->setEnabled(true);
}

#include "zarlok.moc"
