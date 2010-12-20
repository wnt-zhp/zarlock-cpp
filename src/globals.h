#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <iostream>
#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "|\n";

#include <QtGui/QPalette>
#include <QtGui/QFont>

namespace globals {
	extern QPalette palette_ok;
	extern QPalette palette_bad;
// 	extern QPalette palette_neutral;
	extern QFont font_edit;
	extern QFont font_display;
}

#endif /*__GLOBALS_H__*/