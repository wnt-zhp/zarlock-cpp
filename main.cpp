#include <QtGui/QApplication>
#include "zarlok.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
	QString arg_file;
	if (argc > 1) {
		arg_file = argv[1];
	}
    zarlok foo(arg_file);
    foo.show();
    return app.exec();
}
