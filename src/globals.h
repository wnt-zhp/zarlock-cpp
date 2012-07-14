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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <iostream>
#include <QTime>
#define INFO if (true)

// #define PR(x) std::cout << "++DEBUG: " << #x << " = |" << (x) << "| in " << __FILE__ << " at line " << __LINE__ << "\n";
// #define FPR(x) //std::cout << "++DEBUG: " << #x << " = |" << (x) << "| in " << __FILE__ << " at line " << __LINE__ << "\n";

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << (x) << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
#define FPR(x) //std::cout << "++DEBUG: " << #x << " = |" << (x) << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

#define TD	QTime stoper;
#define TM	std::cout << "+++ TIME: " << __func__ << " " << stoper.elapsed() << "s. in " << __FILE__ << " at line " << __LINE__ << std::endl;

#define GTD		QTime gstoper;
#define EGTD	extern QTime gstoper;
#define GTM	std::cout << "+++ GLOBAL TIME: " << __func__ << " " << gstoper.elapsed() << "s. in " << __FILE__ << " at line " << __LINE__ << std::endl;

#define _FL_	"( " __FILE__ " , " __LINE__ << " )"

#include <QtGui/QPalette>
#include <QtGui/QFont>
#include <QtCore/QSettings>
#include <QDebug>

#include "Logger.h"

namespace globals {
	extern QPalette palette_ok;
	extern QPalette palette_bad;

	extern QPalette item_palette;
	extern QColor item_base;
	extern QColor item_altbase;

	extern QPalette item_expired_palette;
	extern QColor item_expired_base;
	extern QColor item_expired_altbase;

	extern QPalette item_aexpired_palette;
	extern QColor item_aexpired_base;
	extern QColor item_aexpired_altbase;

	extern QPalette item_nexpired_palette;
	extern QColor item_nexpired_base;
	extern QColor item_nexpired_altbase;

	extern QFont font_edit;
	extern QFont font_display;

	extern QSettings * appSettings;

	enum VerboseLevel { VLevel1, VLevel2, VDummy };
	extern bool verbose_flag[VDummy];
	extern bool show_browser;
}

void qInfo(globals::VerboseLevel lvl, const char *msg, ...);
void preparePalette();

#define FI() qInfo(globals::VLevel1, "Calling function: %s", __func__)
#define CI() qInfo(globals::VLevel1, "Constructor: %s", __func__)
#define DI() qInfo(globals::VLevel1, "Destructor: %s", __func__)

#define FII() qInfo(globals::VLevel2, "Calling function: %s", __func__)
#define CII() qInfo(globals::VLevel2, "Constructor: %s", __func__)
#define DII() qInfo(globals::VLevel2, "Destructor: %s", __func__)

#endif /*__GLOBALS_H__*/
