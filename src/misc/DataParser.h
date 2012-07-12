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

namespace DataParser {
	bool text(const QString & data, QString & text_parsed, bool allow_empty = false);
	bool quantity(const QString & data, int & qty_parsed);
	bool quantity(const QString & data, QString & qty_parsed);
	bool price(const QString & data, double & netto_parsed, int & vat_parsed);
	bool price(const QString & data, int & price_parsed);
	bool price(const QString & data, QString & price_parsed);
	bool unit(const QString & data, QString & unit_parsed);
	bool date(const QString & data, QDate & date_parsed, const QDate & ref = QDate::currentDate());
	bool date(const QString & data, QString & date_parsed, const QDate & ref = QDate::currentDate());
};

#endif // DATAPARSER_H
