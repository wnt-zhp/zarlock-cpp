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


#ifndef DBREPORTS_H
#define DBREPORTS_H

#include <QDate>
#include <QString>

class DBReports {
public:
	DBReports();
	virtual ~DBReports();

	static void printDailyReport(const QString & dbname, const QDate & date);

	static void showDailyMealReport(const QString & date, QString * reportfile = NULL, bool displayPDF = false);
};

#endif // DBREPORTS_H
