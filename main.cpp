#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

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
	globals::palette_ok.setColor(QPalette::Base, Qt::green);
	globals::palette_bad.setColor(QPalette::Base, Qt::red);
	globals::item_expired.setRgb(255, 172, 172);
	globals::item_aexpired.setRgb(243, 255, 172);
	globals::item_nexpired.setRgb(172, 255, 172);

	globals::font_edit.setItalic(true);
// 	globals::font_ok.setBold(false);
	globals::font_display.setItalic(false);
// 	globals::font_bad.setBold(true);

	QCoreApplication::setOrganizationName("Wydział nowych technologi Głównej Kwatery ZHP");
	QCoreApplication::setOrganizationDomain("wnt.zhp.pl");
	QCoreApplication::setApplicationName("Żarłok");
	QCoreApplication::setApplicationVersion("3.0_dev");
    QApplication app(argc, argv);

// 	QTranslator qtTranslator;
// 	qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
// 	app.installTranslator(&qtTranslator);

	PR(QDate::currentDate().toString(Qt::ISODate).toStdString());
	QTranslator myappTranslator;
	myappTranslator.load("zarlok_" % QLocale::system().name(), PREFIX SHARE "/translations/");
	app.installTranslator(&myappTranslator);

	const QString resWarnMsgTitle = QObject::tr("Cannot find resources");
	const QString resWarnMsg = QObject::tr(
									"Unable to find resources in %1\n"
									"Check your installation and try to run again.\n"
									"Click Close to exit.");
								
	QString resource_database = PREFIX SHARE "/resources/database.rcc";
// 	PR(resource_database.toStdString());
	if (!QResource::registerResource(resource_database)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_database), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString resource_reports = PREFIX SHARE "/resources/reports.rcc";
// 	PR(resource_reports.toStdString());
	if (!QResource::registerResource(resource_reports)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_reports), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString resource_icons = PREFIX SHARE "/resources/icons.rcc";
// 	PR(resource_icons.toStdString());
	if (!QResource::registerResource(resource_icons)) {
		QMessageBox::critical(0, resWarnMsgTitle, resWarnMsg.arg(resource_icons), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QDir dir(QDir::homePath() + QString(ZARLOK_HOME));
// 	QString fsettings = dir.absoluteFilePath("zarlok.settings");
// 	PR(fsettings.toStdString().c_str());

	QFile fsettings(QDir::homePath() + QString(ZARLOK_HOME) + QString("zarlok.cfg"));
	globals::appSettings = new QSettings(fsettings.fileName(), QSettings::IniFormat);

// 	if (argc > 1) {
// 		arg_dbname = argv[1];
// 	}
//     zarlok foo(arg_dbname);
//     foo.show();
	DBBrowser dbb(!fsettings.exists());

	return app.exec();
}
