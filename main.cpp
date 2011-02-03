#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include "config.h"
#include "globals.h"
#include "zarlok.h"

namespace globals {
	QPalette palette_ok = QPalette();
	QPalette palette_bad = QPalette();
	QFont font_edit = QFont();
	QFont font_display = QFont();
}

int main(int argc, char ** argv/*, char ** env*/) {
	globals::palette_ok.setColor(QPalette::Base, Qt::green);
	globals::palette_bad.setColor(QPalette::Base, Qt::red);

	globals::font_edit.setItalic(true);
// 	globals::font_ok.setBold(false);
	globals::font_display.setItalic(false);
// 	globals::font_bad.setBold(true);

    QApplication app(argc, argv);

	QString resource_database = PREFIX SHARE "/resources/database.rcc";
	PR(resource_database.toStdString());
	if (!QResource::registerResource(resource_database)) {
			QMessageBox::critical(0, QObject::tr("Cannot find resources"),
									QObject::tr("Unable to find a database resources in ") +
									resource_database + "\n" +
									QObject::tr("Check your installation and try to run again.\n"
									"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString reports_database = PREFIX SHARE "/resources/reports.rcc";
	PR(reports_database.toStdString());
	if (!QResource::registerResource(reports_database)) {
			QMessageBox::critical(0, QObject::tr("Cannot find resources"),
									QObject::tr("Unable to find a reports resources in ") +
									reports_database + "\n" +
									QObject::tr("Check your installation and try to run again.\n"
									"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QString arg_file;
	if (argc > 1) {
		arg_file = argv[1];
	}
    zarlok foo;
//     foo.show();
    return app.exec();
}
