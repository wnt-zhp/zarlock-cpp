/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Logger.h"

#include <QtDebug>

#include <iostream>

Logger * Logger::loginst = NULL;

Logger::Logger() {
	for (int i = BL+1; i < AL; i++)
		lvlen[i] = false;
}

Logger::~Logger() {
}

Logger & Logger::Instance() {
	if (!loginst) {
		loginst = new Logger();
		std::cout << "++ Create Logger instance\n";
	} else {
		std::cout << "++ Use existing Logger instance\n";
	}

	return * loginst;
}

void Logger::lvlEn(int ll, bool en) {
	switch (ll) {
		case SQL:
		case CTR:
		case DTR:
			Instance().lvlen[ll] = en;
			break;
		default:
			qWarning("Logger::lvlEn() - Wrong level ID");
	}
}

void Logger::logme(int ll, const QString & msg) {
	// check for correct level ID
	if ((ll <= BL) or (ll >= AL)) {
		qWarning("Logger::lvlEn() - Wrong level ID");
		return;
	}

	// is level enabled?
	if (Instance().lvlen[ll] == false)
		return;

	// msg for level
	switch (ll) {
		case SQL:
			std::cout << "-- (" << ll << ") SQL query executed: |" << msg.toStdString() << "|\n";
			break;
		case CTR:
			std::cout << "-- (" << ll << ") Constructor invoked: |" << msg.toStdString() << "|\n";
			break;
		case DTR:
			std::cout << "-- (" << ll << ") Destructor invoked: |" << msg.toStdString() << "|\n";
			break;
	} 

}
