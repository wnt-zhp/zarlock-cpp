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

	bool verbose_flag[VerboseLevel::VDummy] = { false, false };
// 	verbose_flag[0] = false;
// 	verbose_flag[1] = false;
}

// enum QtMsgType { QtInfoMsg, QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtSystemMsg };

void qInfo(globals::VerboseLevel lvl, const char *msg, ...) {
	if (!globals::verbose_flag[lvl])
		return;

	char buffer[1024];
	va_list args;
	va_start (args, msg);
	vsprintf (buffer, msg, args);
	va_end (args);
	switch (lvl) {
		case globals::VLevel1:
			std::cout <<  "++INFO: " << buffer << std::endl;
			break;
		case globals::VLevel2:
			std::cout <<  " ++INFO: " << buffer << std::endl;
			break;
		default:
			break;
	}
}

void myMessageOutput(QtMsgType type, const char *msg)
{
	switch (type) {
		case QtDebugMsg:
			fprintf(stderr, "++DEBUG: %s\n", msg);
			break;
		case QtWarningMsg:
			fprintf(stderr, "++WARNING: %s\n", msg);
			break;
		case QtCriticalMsg:
			fprintf(stderr, "++CRITICAL: %s\n", msg);
			break;
		case QtFatalMsg:
			fprintf(stderr, "++FATAL: %s\n", msg);
			abort();
	}
}

int main(int argc, char ** argv/*, char ** env*/) {
TD
	qInstallMsgHandler(myMessageOutput);
	QApplication app(argc, argv, QApplication::GuiClient);

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

	splash->showMessage(QObject::tr("Starting database browser"));

	DBBrowser dbb;

	splash->showMessage(QObject::tr("Running application"));

	dbb.configure(argc, argv);

	qInfo(globals::VLevel1, "Zarlok is started at %s", qPrintable(QDateTime::currentDateTime().toString(Qt::TextDate)));

// 	dbb.show();
	dbb.goBrowser();

	splash->finish(&dbb);

	return app.exec();
}
