#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <QSplashScreen>

#include "config.h"
#include "globals.h"
#include "DBBrowser.h"

namespace globals {
	QPalette palette_ok = QPalette();
	QPalette palette_bad = QPalette();
	QColor item_expired = QColor();
	QColor item_aexpired = QColor();
	QColor item_nexpired = QColor();
	QFont font_edit = QFont();
	QFont font_display = QFont();

	QSettings * appSettings = NULL;
}

int main(int argc, char ** argv/*, char ** env*/) {
TD
TM
	QApplication app(argc, argv, QApplication::GuiClient);
TM
	const QString resWarnMsgTitle = QObject::tr("Cannot find resources");
	const QString resWarnMsg = QObject::tr(
		"Unable to find resources in %1\n"
		"Check your installation and try to run again.\n"
		"Click Close to exit.");

	QString resource_splash = PREFIX SHARE "/resources/splash.rcc";
	if (!QResource::registerResource(resource_splash)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_splash), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QPixmap splash_pixmap(":/resources/splash/splash.png");
	QSplashScreen * splash = new QSplashScreen(splash_pixmap);
	splash->show();

	splash->showMessage(QObject::tr("Preparing palletes"));

	globals::palette_ok.setColor(QPalette::Base, Qt::green);
	globals::palette_bad.setColor(QPalette::Base, Qt::red);
	globals::item_expired.setRgb(255, 172, 172);
	globals::item_aexpired.setRgb(243, 255, 172);
	globals::item_nexpired.setRgb(172, 255, 172);

	globals::font_edit.setItalic(true);
// 	globals::font_ok.setBold(false);
	globals::font_display.setItalic(false);
// 	globals::font_bad.setBold(true);

// 	QCoreApplication::setOrganizationName("Wydział nowych technologi Głównej Kwatery ZHP");
// 	QCoreApplication::setOrganizationDomain("wnt.zhp.pl");
// 	QCoreApplication::setApplicationName("Żarłok");
// 	QCoreApplication::setApplicationVersion("3.0_dev");

// 	QTranslator qtTranslator;
// 	qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
// 	app.installTranslator(&qtTranslator);

	splash->showMessage(QObject::tr("Loading translations"));

	QTranslator myappTranslator;
	myappTranslator.load("zarlok_" % QLocale::system().name(), PREFIX SHARE "/translations/");
	app.installTranslator(&myappTranslator);

	splash->showMessage(QObject::tr("Loading resources"));

	QString resource_database = PREFIX SHARE "/resources/database.rcc";
	if (!QResource::registerResource(resource_database)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_database), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString resource_reports = PREFIX SHARE "/resources/reports.rcc";
	if (!QResource::registerResource(resource_reports)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_reports), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString resource_icons = PREFIX SHARE "/resources/icons.rcc";
	if (!QResource::registerResource(resource_icons)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_icons), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	splash->showMessage(QObject::tr("Loading settings"));

	QDir dir(QDir::homePath() + QString(ZARLOK_HOME));

	QFile fsettings(QDir::homePath() + QString(ZARLOK_HOME) + QString("zarlok.cfg"));
	globals::appSettings = new QSettings(fsettings.fileName(), QSettings::IniFormat);

	globals::appSettings->beginGroup("Database");
	QString recentDB = globals::appSettings->value("RecentDatabase", "").toString();
	globals::appSettings->endGroup();

	splash->showMessage(QObject::tr("Starting database browser"));
TM
	DBBrowser dbb;

	splash->showMessage(QObject::tr("Running application"));

TM
	if (argc == 1) {
TM
		if (!recentDB.isEmpty()) {
			dbb.openZarlock(recentDB);
		} else {
			PR("First run! Welcome to żarłok.");
			dbb.newDatabaseCreator();
		}
	} else {
TM
		dbb.show();
	}
TM
	sleep(1);
	splash->finish(&dbb);

	return app.exec();
}
