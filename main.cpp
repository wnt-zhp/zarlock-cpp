#include <QtGui/QApplication>
#include "globals.h"
#include "zarlok.h"

namespace globals {
	QPalette palette_ok = QPalette();
	QPalette palette_bad = QPalette();
	QFont font_edit = QFont();
	QFont font_display = QFont();
}

int main(int argc, char ** argv) {
	globals::palette_ok.setColor(QPalette::Base, Qt::green);
	globals::palette_bad.setColor(QPalette::Base, Qt::red);

	globals::font_edit.setItalic(true);
// 	globals::font_ok.setBold(false);
	globals::font_display.setItalic(false);
// 	globals::font_bad.setBold(true);

    QApplication app(argc, argv);
	QString arg_file;
	if (argc > 1) {
		arg_file = argv[1];
	}
    zarlok foo(arg_file);
    foo.show();
    return app.exec();
}
