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
#include <QDesktopServices>
#include <QProgressDialog>

#include "CampSettingsDialog.h"
// public members

/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok() : QMainWindow(), db(Database::Instance()),
					tpw(NULL), tbw(NULL), tdw(NULL), tmw(NULL), dw(NULL) {
	CI();
	setupUi(this);
	this->setWindowTitle(tr("Zarlok (WNT)"));
	this->statusBar()->showMessage(tr("Zarlok (WNT) | build: " RELEASE_REVNUM " | version: " ZARLOK_VERSION));

	toolbar = addToolBar(tr("Main"));
	dbtoolbar = addToolBar(tr("Database"));

	actionQuit = new QAction(QIcon(":/resources/icons/application-exit.png"), tr("Exit"), this);
	actionPrintReport = new QAction(QIcon(":/resources/icons/printer.png"), tr("Print Report DB"), this);
// 	actionSaveDB = new QAction(QIcon(":/resources/icons/svn-commit.png"), tr("Save DB"), this);
	actionAbout = new QAction(QIcon(":/resources/icons/system-help.png"), tr("About"), this);
	actionSwitchDB = new QAction(QIcon(":/resources/icons/system-switch-user.png"), tr("Switch Database"), this);
	actionConfigDB = new QAction(QIcon(":/resources/icons/configure.png"), tr("Configure"), this);

// 	actionSyncDB = new QAction(tr("Sync database"), this);
	actionCreateSMrep = new QAction(QIcon(":/resources/icons/application-pdf.png"), tr("Create SM reports"), this);
	actionCreateKMrep = new QAction(QIcon(":/resources/icons/application-pdf.png"), tr("Create KM reports"), this);
	actionCreateZZrep = new QAction(QIcon(":/resources/icons/application-pdf.png"), tr("Create ZZ reports"), this);
	actionBrowseReports = new QAction(style()->standardIcon(QStyle::SP_DirHomeIcon), tr("Browse reports directory"), this);
	

	actionQuit->setShortcuts(QKeySequence::Quit);
	actionAbout->setShortcut(QKeySequence::HelpContents);
	actionSwitchDB->setShortcuts(QKeySequence::Replace);
	actionConfigDB->setShortcut(QKeySequence::Preferences);

	actionAbout->setMenuRole(QAction::AboutRole);

	toolbar->setObjectName("toolbar");
	toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	toolbar->setIconSize(QSize(64, 64));

	toolbar->addAction(actionQuit);
// 	toolbar->addAction(actionPrintReport);
// 	toolbar->addSeparator();
// 	toolbar->addSeparator();
	toolbar->addAction(actionAbout);
	toolbar->setFloatable(false);
	toolbar->setMovable(false);

	dbtoolbar->setWindowTitle("Database1");
	dbtoolbar->setAccessibleName("Database2");
	dbtoolbar->setAccessibleDescription("Database3");
	dbtoolbar->setObjectName("dbtoolbar");
	dbtoolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon/*ToolButtonTextUnderIcon*/);
	dbtoolbar->setIconSize(QSize(64, 64));
	dbiw = new DBItemWidget();

// 	dbiw->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	dbiw->resize(QSize(64, 300));

	tools = new QToolButton(dbtoolbar);
// 	tools->setIcon(QIcon(":/resources/icons/configure.png"));
	tools->setIcon(QIcon(":/resources/icons/tools-wizard.png"));
	tools->setText(tr("Tools"));
	tools->setToolButtonStyle(Qt::ToolButtonTextUnderIcon/*ToolButtonTextBesideIcon*/);
	tools->setPopupMode(QToolButton::InstantPopup);

	dbtoolbar->addWidget(dbiw);
	dbtoolbar->addWidget(tools);
	dbtoolbar->addAction(actionSwitchDB);
	dbtoolbar->setFloatable(false);
	dbtoolbar->setMovable(false);

	tools->addAction(actionConfigDB);
// 	tools->addAction(actionSyncDB);
	tools->addAction(actionCreateKMrep);
	tools->addAction(actionCreateSMrep);
	tools->addAction(actionCreateZZrep);
	tools->addAction(actionBrowseReports);

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

// 	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(close()));
// 	connect(this, SIGNAL(destroyed(QObject*)), this, SLOT(doExitZarlok()));

// 	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(doExitZarlok()));

	connect(actionQuit, SIGNAL(triggered(bool)), this, SLOT(doExitZarlok()));

	connect(actionSwitchDB, SIGNAL(triggered(bool)), this, SLOT(doSwitchDB()));

// 	connect(actionConfigDB, SIGNAL(triggered(bool)), this, SLOT(doCampSettings()));
	connect(actionConfigDB, SIGNAL(triggered(bool)), this, SLOT(doCampSettings()));

	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printDailyReport()));

// 	connect(syncdb, SIGNAL(triggered(bool)), this, SLOT(syncDB()));
	connect(actionCreateSMrep, SIGNAL(triggered(bool)), this, SLOT(doCreateSMreports()));
	connect(actionCreateKMrep, SIGNAL(triggered(bool)), this, SLOT(doCreateKMreports()));

	connect(actionCreateZZrep, SIGNAL(triggered(bool)), this, SLOT(doCreateZZReports()));
	connect(actionBrowseReports, SIGNAL(triggered(bool)), this, SLOT(doBrowseReports()));

// 	connect(MainTab, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

	readSettings();

	if (!db->cs()->isCorrect)
		doCampSettings();
	else
		activateUi(db->cs()->isCorrect);
// 	updateAppTitle();

	setAttribute(Qt::WA_DeleteOnClose);
}

zarlok::~zarlok() {
	DI();
	while (MainTab->count())
		MainTab->removeTab(0);
	/*if (tmw)*/ delete tmw;/* tmw = NULL;*/
	/*if (tdw)*/ delete tdw;/* tdw = NULL;*/
	/*if (tbw)*/ delete tbw;/* tbw = NULL;*/
	/*if (tpw)*/ delete tpw;/* tpw = NULL;*/

	delete toolbar;
	delete dbtoolbar;

	delete dw;
}

void zarlok::doSwitchDB() {
	writeSettings();
	emit switchDB();
}

void zarlok::doExitZarlok() {
	writeSettings();
	emit exitZarlok();
	close();
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

	if (!db->cs()->isCorrect) {
// 		doSwitchDB();
		delete dw;
		dw = new DimmingMessage(MainTab);
		dw->setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical));
		dw->setMessage(tr("Database is not loaded properly."));
		dw->go();
	} else {
		if (dw)
			dw->og();
	}

	if (activate) {
		dbiw->update(db->cs());
	} else {
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

bool zarlok::doCampSettings() {
	CampSettingsDialog csd(db->cs());

	int ans = csd.exec();

	if (ans)
		db->cs()->writeCampSettings();

	activateUi(db->cs()->isCorrect);
	

	return ans;
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

void zarlok::db2update() {
	statusbar->showMessage(tr("You need to save your database!"));
	actionSaveDB->setEnabled(true);
}

void zarlok::doCreateSMreports() {
	DBReports::printSMReport();
}

void zarlok::doCreateKMreports() {
	DBReports::printKMReport();
}

void zarlok::doCreateZZReports() {
	QString fn;
	int num = db->CachedMealDay()->rowCount();
	
	QProgressDialog progress(tr("Printing reports..."), tr("&Cancel"), 0, num);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setValue(0);
	
	for (int i = 0; i < num; ++i) {
		QDate sd = db->CachedMealDay()->index(i, MealDayTableModel::HMealDate).data(Qt::EditRole).toDate();
		
		progress.setValue(i);
		progress.setLabelText(tr("Creating report for day: ") % sd.toString(Qt::DefaultLocaleShortDate));
		if (progress.wasCanceled())
			break;
		
		DBReports::printDailyMealReport(sd.toString(Qt::ISODate), &fn);
	}
	progress.setValue(num);
}

void zarlok::doBrowseReports() {
	
	// 	QDesktopServices::openUrl(QUrl("file:///home"));
	QString reportsdir = "file://" % QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % db->openedDatabase();
	PR(reportsdir.toStdString());
	QDesktopServices::openUrl(QUrl(reportsdir));
}

#include "zarlok.moc"
