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


#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QDate>

namespace DataParser {
	bool text(const QString & data, QString & text_formated, bool allow_empty = false);
	bool quantity(const QString & data, double & qty_formated);
	bool quantity(const QString & data, QString & qty_formated);
	bool price(const QString & data, double & price_formated, double & tax_formated);
	bool price(const QString & data, QString & price_formated);
	bool unit(const QString & data, QString & unit_formated);
	bool date(const QString & data, QDate & date_formated, const QDate & ref = QDate::currentDate());
	bool date(const QString & data, QString & date_formated, const QDate & ref = QDate::currentDate());
};

#endif // DATAPARSER_H
