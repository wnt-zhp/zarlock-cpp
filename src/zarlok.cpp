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
#include <DBItemWidget.h>

// public members

/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok(const QString & dbname) : QMainWindow(), db(Database::Instance()), dwm_prod(NULL),
										 tpw(NULL), tbw(NULL), tdw(NULL), tmw(NULL) {
	setupUi(this);

	this->setWindowTitle(tr("Zarlok by Rafal Lalik --- build: ").append(__TIMESTAMP__));

// 	dbb = new DBBrowser();

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

	QToolBar * tb2 = addToolBar(tr("Database"));
	tb2->setAllowedAreas(Qt::RightToolBarArea);
	tb2->setIconSize(QSize(400, 100));
// 	QPushButton * b = new QPushButton("adasd");
// 	QIcon * b = new QIcon(QPixmap(300, 80));
// 	b->resize( 400, 100);
// 	b->setGeometry(0, 0, 400, 100);
	DBItemWidget * dbiw = new DBItemWidget();
// 	b->setGeometry(0, 0, 400, 100);
// 	dbiw->setGeometry(0, 0, 400, 100);
// 	toolbar->addWidget(new DBItemWidget(b));
// 	toolbar->addWidget(b);
// 	tb2->addAction(*b, "adssad");
// 	tb2->addAction(new QAction(dbiw));
// 	tb2->addWidget(b);
	tb2->addWidget(dbiw);
	activateUi(false);

// 	connect(&db, SIGNAL(databaseDirty()), this, SLOT(db2update()));
	connect(dbb, SIGNAL(dbb_database(const QString&)), this, SLOT(openDB(const QString&)));

	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(closeDB()));

	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printDailyReport()));

// 	connect(MainTab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

// 	if (dbname.isEmpty()) {
// 		dbb->show();
// 	} else {
// // 		openDB(dbname);
// 		dbb->openDBName(dbname);
// 	}
}

zarlok::~zarlok() {
// 	saveDB();
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
		tmw = new TabMealWidget();

		MainTab->addTab(tpw, tr("Products"));
		MainTab->addTab(tbw, tr("Stock"));
		MainTab->addTab(tdw, tr("Distribute"));
		MainTab->addTab(tmw, tr("Meal"));

		MainTab->setTabPosition(QTabWidget::North);

		MainTab->setVisible(activate);
		MainTab->setEnabled(activate);
	} else {
		if (tmw) delete tmw; tmw = NULL;
		if (tdw) delete tdw; tdw = NULL;
		if (tbw) delete tbw; tbw = NULL;
		if (tpw) delete tpw; tpw = NULL;
	}
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
