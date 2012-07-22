/*
    This file is part of Zarlok.

    Copyright (C) 2012  Rafał Lalik <rafal.lalik@zhp.net.pl>

    Zarlok is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Zarlok is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QSplashScreen>
#include <QLabel>
#include <QDir>
#include <QResource>
#include <QTranslator>
#include <QStringBuilder>

#include <getopt.h>

#include "globals.h"
#include "config.h"
#include "version.h"

#include "DBBrowser.h"
#include "NetworkServicesManager.h"
#include "AbstractNetworkService.h"
#include "ProgramSettings.h"

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
	bool show_browser = false;
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

void myMessageOutput(QtMsgType type, const char *msg) {
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

void configure(int argc, char* argv[]) {
	int c;

	while (true) {
		static struct option long_options[] = {
			/* These options set a flag. */
			{"version", no_argument,       0, 'V'},
			{"verbose", no_argument,       0, 'v'},
			{"vverbose", no_argument,       0, 'w'},
			{"quiet",   no_argument,       0, 'q'},
			/* These options don't set a flag.
			 *        We distinguish them by their indices. */
			{"browser",  no_argument,       0, 'b'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "Vvwqb", 
						 long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;
		
		switch (c) {
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
			case 'b':
				globals::show_browser = true;
			// 				puts ("option -b\n");
				break;
			case 'v':
				globals::verbose_flag[0] = true;
				break;
			case 'w':
				globals::verbose_flag[1] = true;
				break;
			case 'q':
				globals::verbose_flag[0] = false;
				break;
			case 'V':
				std::cout << ZARLOK_VERSION << std::endl;
// 				std::cout << ZARLOK_VERSION << " --- " << QDateTime::currentDateTime().toString().toStdString() << std::endl;
				exit(EXIT_SUCCESS);
			case '?':
				/* getopt_long already printed an error message. */
				break;
			default:
				abort();
		}
	}

	/* Instead of reporting ‘--verbose’
	 * and ‘--brief’ as they are encountered,
	 * we report the final status resulting from them. */
	// 	if (verbose_flag)
	// 		puts ("verbose flag is set");

	/* Print any remaining command line arguments (not options). */
	// 	if (optind < argc) {
		// 		printf ("non-option ARGV-elements: ");
		// 		while (optind < argc)
		// 			printf ("%s ", argv[optind++]);
		// 		putchar ('\n');
		// 	}
	
		// 	for (int i = 0; i < argc; ++i)
		// 		PR(argv[i]);
}

void doFirstRunMessage() {
	globals::appSettings->beginGroup("Misc");
	QString recentVersion = (globals::appSettings->value("recentRunVersion", QString()).toString());

	// if you don't want update status right now, comment out next line and use later code from lines below
// 	if (recentVersion != ZARLOK_VERSION)
// 		globals::appSettings->setValue("recentRunVersion", ZARLOK_VERSION);

	globals::appSettings->endGroup();

/* Snippet to use:

	globals::appSettings->beginGroup("Misc");
	globals::appSettings->setValue("recentRunVersion", ZARLOK_VERSION);
	globals::appSettings->endGroup();

*/

	// place your code to execute

	if (recentVersion.isNull()) {
		QMessageBox mbox;
		mbox.setIcon(QMessageBox::Information);
		mbox.setWindowTitle(QObject::tr("Startup information"));
		mbox.setText(QObject::tr("You are using development version of the Zarlok. "
				"We strongly recommend you to register to our mailing list "
				"to retrieve always fresh information about Zarlok. "
				"Just send e-mail to address "
				"<a href='mailto:%1?subject='[Newsletter registration]''>%1</a>").arg("zarlok@zhp.net.pl"));
		mbox.addButton(QObject::tr("Remind me later"), QMessageBox::RejectRole);
		mbox.addButton(QObject::tr("Ok"), QMessageBox::AcceptRole);

		mbox.setTextFormat(Qt::RichText);

		QLabel * label = qobject_cast<QLabel *>(mbox.layout()->itemAt(1)->widget());
		if (label)
			label->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

		int ret = mbox.exec();
		if (ret == QMessageBox::Accepted) {
			globals::appSettings->beginGroup("Misc");
			globals::appSettings->setValue("recentRunVersion", ZARLOK_VERSION);
			globals::appSettings->endGroup();
		}
	}

	// end of your code
	return;
}

int main(int argc, char ** argv/*, char ** env*/) {
	qInstallMsgHandler(myMessageOutput);

	QApplication app(argc, argv, QApplication::GuiClient);
	configure(argc, argv);

	QDir dir(QDir::homePath() + QString(ZARLOK_HOME));

	QFile fsettings(QDir::homePath() + QString(ZARLOK_HOME) + QString("zarlok.cfg"));
	globals::appSettings = new QSettings(fsettings.fileName(), QSettings::IniFormat);

	ProgramSettings * ps = ProgramSettings::Instance();

	NetworkServicesManager netmanager;
	if (ProgramSettings::Instance()->doUpdateCheck) {
		netmanager.checkForUpdates();
		netmanager.getFeed();

	}

	bool static_build = STATIC_BUILD;

	qDebug() << "Runtime app:" << argv[0] << ZARLOK_VERSION;

#ifdef __unix__ /* __unix__ is usually defined by compilers targeting Unix systems */

#elif defined _WIN32 /* _Win32 is usually defined by compilers targeting 32 or 64 bit Windows systems */
	app.addLibraryPath(app.applicationDirPath() % "/plugins");
#endif

	qDebug() << "Plugins path: " << app.libraryPaths();
	qDebug() << app.applicationFilePath();
	qDebug() << app.applicationDirPath();

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

	splash->showMessage(QObject::tr("Starting database browser"));

	doFirstRunMessage();

	DBBrowser dbb;

	splash->showMessage(QObject::tr("Running application"));

	qInfo(globals::VLevel1, "Zarlok is started at %s", qPrintable(QDateTime::currentDateTime().toString(Qt::TextDate)));

// 	dbb.show();
	dbb.goBrowser();
	splash->finish(&dbb);
	int retval = app.exec();

	ps->Destroy();

	return retval;
}
