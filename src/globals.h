#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <iostream>
#include <QTime>
#define INFO if (true)

// #define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| in " << __FILE__ << " at line " << __LINE__ << "\n";
// #define FPR(x) //std::cout << "++DEBUG: " << #x << " = |" << x << "| in " << __FILE__ << " at line " << __LINE__ << "\n";

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
#define FPR(x) //std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

#define TD	QTime stoper;
#define TM	std::cout << "+++ TIME: " << __func__ << " " << stoper.elapsed() << "s. in " << __FILE__ << " at line " << __LINE__ << std::endl;

#define GTD		QTime gstoper;
#define EGTD	extern QTime gstoper;
#define GTM	std::cout << "+++ GLOBAL TIME: " << __func__ << " " << gstoper.elapsed() << "s. in " << __FILE__ << " at line " << __LINE__ << std::endl;

#define _FL_	"( " __FILE__ " , " __LINE__ << " )"

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

	extern bool verbose_flag;
}

void qInfo(const char *msg, ...);

#define FI() qInfo("Calling function: %s", __func__)
#define CI() qInfo("Constructor: %s", __func__)
#define DI() qInfo("Destructor: %s", __func__)

#endif /*__GLOBALS_H__*/
