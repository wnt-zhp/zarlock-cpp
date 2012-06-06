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


#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
class Logger {

public:
	static Logger & Instance();

	static void lvlEn(int ll, bool en = true);
	static void logme(int ll, const QString & msg);

private:
	Logger();
	Logger(const Logger &) {};
	virtual ~Logger();


public:
	enum loglvl { BL = -1, SQL, CTR, DTR, AL };
	bool lvlen[3];

private:
	static Logger * loginst;
};

#endif // LOGGER_H
