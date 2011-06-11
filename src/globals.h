#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <iostream>
#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| in " << __FILE__ << " at line " << __LINE__ << "\n";
#define FPR(x) //std::cout << "++DEBUG: " << #x << " = |" << x << "| in " << __FILE__ << " at line " << __LINE__ << "\n";

#define INFO if (true)

#include <QtGui/QPalette>
#include <QtGui/QFont>
#include <QtCore/QSettings>

#include "Logger.h"

namespace globals {
	extern QPalette palette_ok;
	extern QPalette palette_bad;
	extern QColor item_expired;
	extern QColor item_aexpired;
	extern QColor item_nexpired;
// 	extern QPalette palette_neutral;
	extern QFont font_edit;
	extern QFont font_display;

	extern QSettings * appSettings;
}

#endif /*__GLOBALS_H__*/