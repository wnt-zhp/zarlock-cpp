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
zarlok::zarlok(const QString & dbname) : QMainWindow(), db(Database::Instance()),
										 tpw(NULL), tbw(NULL), tdw(NULL), tmw(NULL) {
	setupUi(this);
	this->setWindowTitle(tr("Zarlok by Rafal Lalik --- build: ").append(__TIMESTAMP__));

	actionQuit = new QAction(QIcon(":/resources/icons/application-exit.png"), tr("Exit"), this);
	actionPrintReport = new QAction(QIcon(":/resources/icons/printer.png"), tr("Print Report DB"), this);
// 	actionSaveDB = new QAction(QIcon(":/resources/icons/svn-commit.png"), tr("Save DB"), this);
	actionAbout = new QAction(QIcon(":/resources/icons/system-help.png"), tr("About"), this);
	actionSwitchDB = new QAction(QIcon(":/resources/icons/system-switch-user.png"), tr("Switch Database"), this);

// 	actionSaveDB->setEnabled(false);

	toolbar = addToolBar(tr("Main"));
	toolbar->setObjectName("toolbar");
	toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
// 	toolbar->setMovable(false);
	toolbar->setIconSize(QSize(64, 64));


	toolbar->addAction(actionQuit);
// 	toolbar->addAction(actionPrintReport);
	toolbar->addSeparator();
// 	toolbar->addAction(actionSaveDB);
	toolbar->addSeparator();
	toolbar->addAction(actionAbout);

	dbtoolbar = addToolBar(tr("Database"));
	dbtoolbar->setObjectName("dbtoolbar");
	dbtoolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
// 	dbtoolbar->setMovable(false);
	dbtoolbar->setIconSize(QSize(64, 64));

	dbtoolbar->addAction(actionSwitchDB);

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
	dbtoolbar->addWidget(dbiw);
// 	activateUi(false);

// 	connect(&db, SIGNAL(databaseDirty()), this, SLOT(db2update()));

// 	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(doExitZarlok()));
	connect(actionSwitchDB, SIGNAL(triggered(bool)), this, SLOT(doExitZarlok()));

	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printDailyReport()));

// 	connect(MainTab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	activateUi(true);
	readSettings();
}

zarlok::~zarlok() {
	FPR(__func__);
	activateUi(false);
	delete toolbar;
	delete dbtoolbar;
}

void zarlok::doExitZarlok() {
	writeSettings();
	emit exitZarlok();
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
	this->setVisible(activate);

	if (activate) {
		tpw = new TabProductsWidget();
		tbw = new TabBatchWidget();
		tdw = new TabDistributorWidget();
		tmw = new TabMealWidget();

		MainTab->addTab(tpw, QIcon(":/resources/icons/folder-green.png"), tr("Products"));
		MainTab->addTab(tbw, QIcon(":/resources/icons/folder-orange.png"), tr("Stock"));
		MainTab->addTab(tdw, QIcon(":/resources/icons/folder-downloads.png"), tr("Distribute"));
		MainTab->addTab(tmw, QIcon(":/resources/icons/folder-violet.png"), tr("Meal"));

// 		MainTab->setTabPosition(QTabWidget::North);
		MainTab->setIconSize(QSize(48, 48));

		MainTab->setTabShape(QTabWidget::Triangular);
	
		MainTab->setVisible(activate);
		MainTab->setEnabled(activate);
	} else {
		while (MainTab->count())
			MainTab->removeTab(0);
		if (tmw) delete tmw; tmw = NULL;
		if (tdw) delete tdw; tdw = NULL;
		if (tbw) delete tbw; tbw = NULL;
		if (tpw) delete tpw; tpw = NULL;
	}
}

void zarlok::writeSettings() {
	globals::appSettings->beginGroup("WindowSettings");
	globals::appSettings->setValue("size", size());
	globals::appSettings->setValue("pos", pos());
	globals::appSettings->setValue("state", saveState());
	globals::appSettings->endGroup();
}

void zarlok::readSettings() {
	 globals::appSettings->beginGroup("WindowSettings");
	 resize			(globals::appSettings->value("size", QSize(860, 580)).toSize());
	 move			(globals::appSettings->value("pos", QPoint(200, 200)).toPoint());
	 restoreState	(globals::appSettings->value("state").toByteArray());
	 globals::appSettings->endGroup();
}

void zarlok::closeEvent(QCloseEvent *event) {
	if (/*userReallyWantsToQuit()*/true) {
		writeSettings();
		event->accept();
	} else {
		event->ignore();
	}
}

void zarlok::printDailyReport() {
// 	DBReports::printDailyReport(dbname, QDate::currentDate());
}

void zarlok::about() {
//      QLabel infoLabel->setText(tr("Invoked <b>Help|About</b>"));
     QMessageBox::about(this, tr("About Menu"),
             tr("The <b>Menu</b> example shows how to create "
                "menu-bar menus and context menus."));
}
/*
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
}*/

void zarlok::db2update() {
	statusbar->showMessage(tr("You need to save your database!"));
	actionSaveDB->setEnabled(true);
}

#include "zarlok.moc"
