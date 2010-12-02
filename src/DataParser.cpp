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

#include <iostream>
using namespace std;

#define PR(x) cout << "++DEBUG: " << #x << " = |" << x << "|\n";


#include "DataParser.h"

#include <QRegExp>

bool DataParser::price(const QString & data, double & price, double & tax) {
	QRegExp rx("^\\s*(\\d+([.,]?\\d+)?)[ \t]?([p\\+][ \t]?(22|15|7)\%?)?\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());
// 	PR(rx.cap(4).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		price = rx.cap(1).toFloat();
		tax = rx.cap(4).toFloat();
		return true;
	}
	return false;
}

bool DataParser::date(const QString & data, QDate & date, const QDate & ref) {
	QRegExp rx1("^\\s*([+-]\\d+|0)\\s*$");
	QRegExp rx2("^\\s*dzis\\s*$");
	QRegExp rx3("^\\s*(\\d\\d?)([.:;,-/ ](\\d\\d?)([.:;,-/ ](\\d\\d\\d\\d))?)?\\s*$");	// dd/mm//yy

// 	PR(data.toStdString());

// 	PR(rx1.indexIn(data));
// 	PR(rx1.cap(0).toStdString());

// 	PR(rx2.indexIn(data));
// 	PR(rx2.cap(0).toStdString());
// 	PR(rx2.cap(1).toStdString());

// 	PR(rx3.indexIn(data));
// 	PR(rx3.cap(0).toStdString());
// 	PR(rx3.cap(1).toStdString());
// 	PR(rx3.cap(2).toStdString());
// 	PR(rx3.cap(3).toStdString());
// 	PR(rx3.cap(4).toStdString());
// 	PR(rx3.cap(5).toStdString());

// 	PR(ref.toString("dd/MM/yyyy").toStdString());

	rx1.indexIn(data);
	rx2.indexIn(data);
	rx3.indexIn(data);
	if (!rx1.cap(0).isEmpty()) {
		date = ref.addDays(rx1.cap(0).toInt());
		return true;
	} else if (!rx2.cap(0).isEmpty()) {
		date = QDate::currentDate();
		return true;
	} else if (!rx3.cap(0).isEmpty()) {
		date = QDate::currentDate();
		int day = rx3.cap(1).isEmpty() ? date.day() : rx3.cap(1).toInt();
		int month = rx3.cap(3).isEmpty() ? date.month() : rx3.cap(3).toInt();
		int year = rx3.cap(5).isEmpty() ? date.year() : rx3.cap(5).toInt();
		date.setDate(year, month, day);
// 		PR(date.toString("dd/MM/yyyy").toStdString());
		return date.isValid();
	}
	return false;
}