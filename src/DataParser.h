/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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


#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QDate>

class DataParser {
public:
	static bool text(const QString & data, QString & text_formated, bool allow_empty = false);
	static bool quantity(const QString & data, double & qty_formated);
	static bool quantity(const QString & data, QString & qty_formated);
	static bool price(const QString & data, double & price_formated, double & tax_formated);
	static bool price(const QString & data, QString & price_formated);
	static bool unit(const QString & data, QString & unit_formated);
	static bool date(const QString & data, QDate & date_formated, const QDate & ref = QDate::currentDate());
	static bool date(const QString & data, QString & date_formated, const QDate & ref = QDate::currentDate());
};

#endif // DATAPARSER_H
