#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <QSplashScreen>

#include "config.h"
#include "globals.h"
#include "DBBrowser.h"

namespace globals {
	QPalette palette_ok;
	QPalette palette_bad;

	QPalette item_palette;
	QColor item_base;
	QColor item_altbase;

	QPalette item_expired_palette;
	QColor item_expired_base;
	QColor item_expired_altbase;

	QPalette item_aexpired_palette;
	QColor item_aexpired_base;
	QColor item_aexpired_altbase;

	QPalette item_nexpired_palette;
	QColor item_nexpired_base;
	QColor item_nexpired_altbase;

	QFont font_edit = QFont();
	QFont font_display = QFont();

	QSettings * appSettings = NULL;

	bool verbose_flag[VerboseLevel::VDummy] = { false, false };
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

void preparePallete() {
	globals::palette_ok.setColor(QPalette::Base, Qt::green);
	globals::palette_bad.setColor(QPalette::Base, Qt::red);

	// Standard
	globals::item_palette = QApplication::palette();
	globals::item_base				.setRgb(0xff, 0xff, 0xff);
	globals::item_altbase.setHsl(globals::item_base.hslHue(), globals::item_base.hslSaturation(), globals::item_base.lightness()-40);
// 	globals::item_altbase			.setRgb(0xf0, 0xf0, 0xf0);
	
	globals::item_palette.setColor(QPalette::Base, globals::item_base);
	globals::item_palette.setColor(QPalette::AlternateBase, globals::item_altbase);

	// Expired
	globals::item_expired_palette = QApplication::palette();
	globals::item_expired_base		.setRgb(255, 192, 192);
	globals::item_expired_altbase.setHsl(globals::item_expired_base.hslHue(), globals::item_expired_base.hslSaturation(), globals::item_expired_base.lightness()-40);
// 	globals::item_expired_altbase	.setRgb(0xfa, 0xfa, 0xfa);
	
	globals::item_expired_palette.setColor(QPalette::Base, globals::item_expired_base);
	globals::item_expired_palette.setColor(QPalette::AlternateBase, globals::item_expired_altbase);

	// Almost expired
	globals::item_aexpired_palette = QApplication::palette();
// 	globals::item_aexpired_base		.setRgb(243, 255, 172);
	globals::item_aexpired_base		.setRgb(255, 255, 225);
	globals::item_aexpired_altbase.setHsl(globals::item_aexpired_base.hslHue(), globals::item_aexpired_base.hslSaturation(), globals::item_aexpired_base.lightness()-40);
// 	globals::item_aexpired_altbase	.setRgb(0xfa, 0xfa, 0xfa);
	
	globals::item_aexpired_palette.setColor(QPalette::Base, globals::item_aexpired_base);
	globals::item_aexpired_palette.setColor(QPalette::AlternateBase, globals::item_aexpired_altbase);

	// Not expired
	globals::item_nexpired_palette = QApplication::palette();
// 	globals::item_nexpired_base		.setRgb(172, 255, 172);
	globals::item_nexpired_base		.setRgb(201, 244, 213);
	globals::item_nexpired_altbase.setHsl(globals::item_nexpired_base.hslHue(), globals::item_nexpired_base.hslSaturation(), globals::item_nexpired_base.lightness()-40);
// 	globals::item_nexpired_altbase	.setRgb(0xfa, 0xfa, 0xfa);
	
	globals::item_nexpired_palette.setColor(QPalette::Base, globals::item_nexpired_base);
	globals::item_nexpired_palette.setColor(QPalette::AlternateBase, globals::item_nexpired_altbase);
}

int main(int argc, char ** argv/*, char ** env*/) {
TD
bool static_build = STATIC_BUILD;
PR(static_build);
	qInstallMsgHandler(myMessageOutput);
	QApplication app(argc, argv, QApplication::GuiClient);

	const QString resWarnMsgTitle = QObject::tr("Cannot find resources");
	const QString resWarnMsg = QObject::tr(
		"Unable to find resources in %1\n"
		"Check your installation and try to run again.\n"
		"Click Close to exit.");

	if (!static_build) {
	QString resource_splash = SHARE "/resources/splash.rcc";
	if (!QResource::registerResource(resource_splash)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_splash), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}
	}

	QPixmap splash_pixmap(":/resources/splash/splash.png");
	QSplashScreen * splash = new QSplashScreen(splash_pixmap);
	splash->show();

	splash->showMessage(QObject::tr("Preparing palletes"));

	preparePallete();

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

// 	QPalette pal = QApplication::palette();
// 	pal.setColor(QPalette::AlternateBase, Qt::red);
// 	QApplication::setPalette(pal);

	splash->showMessage(QObject::tr("Loading translations"));

	QTranslator myappTranslator;
	myappTranslator.load("zarlok_" % QLocale::system().name(), SHARE "/translations/");
	app.installTranslator(&myappTranslator);

	splash->showMessage(QObject::tr("Loading resources"));

	if (!static_build) {

	QString resource_database = SHARE "/resources/database.rcc";
	if (!QResource::registerResource(resource_database)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_database), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString resource_reports = SHARE "/resources/reports.rcc";
	if (!QResource::registerResource(resource_reports)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_reports), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString resource_icons = SHARE "/resources/icons.rcc";
	if (!QResource::registerResource(resource_icons)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_icons), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}
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
