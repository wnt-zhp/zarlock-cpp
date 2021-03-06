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

#include "CampSettingsDialog.h"
// public members
GTD
/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok() : QMainWindow(), db(Database::Instance()),
					tpw(NULL), tbw(NULL), tdw(NULL), tmw(NULL) {
	setupUi(this);
	this->setWindowTitle(tr("Zarlok by Rafal Lalik --- build: ").append(__TIMESTAMP__));

	actionQuit = new QAction(QIcon(":/resources/icons/application-exit.png"), tr("Exit"), this);
	actionPrintReport = new QAction(QIcon(":/resources/icons/printer.png"), tr("Print Report DB"), this);
// 	actionSaveDB = new QAction(QIcon(":/resources/icons/svn-commit.png"), tr("Save DB"), this);
	actionAbout = new QAction(QIcon(":/resources/icons/system-help.png"), tr("About"), this);
	actionSwitchDB = new QAction(QIcon(":/resources/icons/system-switch-user.png"), tr("Switch Database"), this);
	actionConfigDB = new QAction(QIcon(":/resources/icons/configure.png"), tr("Configure"), this);

	actionQuit->setShortcuts(QKeySequence::Quit);
	actionAbout->setShortcut(QKeySequence::HelpContents);
	actionSwitchDB->setShortcuts(QKeySequence::Replace);
	actionConfigDB->setShortcut(QKeySequence::Preferences);

	actionAbout->setMenuRole(QAction::AboutRole);

	toolbar = addToolBar(tr("Main"));
	toolbar->setObjectName("toolbar");
	toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	toolbar->setIconSize(QSize(64, 64));

	toolbar->addAction(actionQuit);
// 	toolbar->addAction(actionPrintReport);
// 	toolbar->addSeparator();
// 	toolbar->addSeparator();
	toolbar->addAction(actionAbout);

	dbtoolbar = addToolBar(tr("Database"));
	dbtoolbar->setWindowTitle("Database1");
	dbtoolbar->setAccessibleName("Database2");
	dbtoolbar->setAccessibleDescription("Database3");
	dbtoolbar->setObjectName("dbtoolbar");
	dbtoolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	dbtoolbar->setIconSize(QSize(64, 64));
	dbiw = new DBItemWidget();

// 	dbiw->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	dbiw->resize(QSize(64, 300));

	dbtoolbar->addWidget(dbiw);
	dbtoolbar->addAction(actionConfigDB);
	dbtoolbar->addAction(actionSwitchDB);

	tpw = new TabProductsWidget();
	tbw = new TabBatchWidget();
	tdw = new TabDistributorWidget();
	tmw = new TabMealWidget();

	MainTab->addTab(tpw, QIcon(":/resources/icons/folder-green.png"), tr("Products"));
	MainTab->addTab(tbw, QIcon(":/resources/icons/folder-orange.png"), tr("Stock"));
	MainTab->addTab(tdw, QIcon(":/resources/icons/folder-downloads.png"), tr("Distribute"));
	MainTab->addTab(tmw, QIcon(":/resources/icons/folder-violet.png"), tr("Meal"));

	MainTab->setIconSize(QSize(48, 48));

// 	MainTab->setDocumentMode(true);
//	MainTab->setTabShape(QTabWidget::Triangular);
//	MainTab->setTabPosition(QTabWidget::North);

// 	connect(&db, SIGNAL(databaseDirty()), this, SLOT(db2update()));
// 	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
	connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(doExitZarlok()));
	connect(actionSwitchDB, SIGNAL(triggered(bool)), this, SLOT(doExitZarlok()));
	connect(actionConfigDB, SIGNAL(triggered(bool)), this, SLOT(doCampSettings()));

	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printDailyReport()));

// 	connect(MainTab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	readSettings();

	activateUi(db.cs()->isCorrect);
// 	updateAppTitle();
}

zarlok::~zarlok() {
	FPR(__func__);
	while (MainTab->count())
		MainTab->removeTab(0);
	/*if (tmw)*/ delete tmw;/* tmw = NULL;*/
	/*if (tdw)*/ delete tdw;/* tdw = NULL;*/
	/*if (tbw)*/ delete tbw;/* tbw = NULL;*/
	/*if (tpw)*/ delete tpw;/* tpw = NULL;*/

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
 * @param activate Gdy ustawione, powoduje aktywację kontrolek
 * @return void
 **/
void zarlok::activateUi(bool activate) {
	MainTab->setEnabled(activate);

	if (activate) {
		dbiw->update(db.cs());
	} else {
		doCampSettings();
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
	resize			(globals::appSettings->value("size", QSize(860, 620)).toSize());
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

void zarlok::doCampSettings() {
	CampSettingsDialog csd(db.cs());
	if (csd.exec()) {
		db.cs()->writeCampSettings();
		dbiw->update(db.cs());
	}
	activateUi(db.cs()->isCorrect);
}

void zarlok::updateAppTitle() {
// 	QString name = "CAMP NAME: " % camp.campName % " ( " % tr("quatermaster") % ": " % camp.campQuarter % ")";
// 	QString period = "\tPERIOD: " % camp.campDateBegin.toString(Qt::TextDate) % " - " % camp.campDateEnd.toString(Qt::TextDate);
// 	QString scoutsnr = "\tSCOUTS: " % QString("%1 + %2 scouts").arg(camp.scoutsNo).arg(camp.leadersNo);

// 	this->statusBar()->showMessage(name % period % scoutsnr);
// 	this->setWindowTitle(name % period % scoutsnr);
}

void zarlok::about() {
//	QLabel infoLabel->setText(tr("Invoked <b>Help|About</b>"));
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
