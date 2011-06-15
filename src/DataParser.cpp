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

#include "globals.h"
#include "DataParser.h"

#include <QRegExp>

bool DataParser::text(const QString & data, QString & text_formated, bool allow_empty) {
// 	QRegExp rx("^\\s*[a-zA-Z0-9\,\.;:-_]\\s*$");
	QString regexp = allow_empty ? ".*" : "^.+$";
	QRegExp rx(regexp);

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		text_formated = data;
		return true;
	}
	return false;
}

bool DataParser::quantity(const QString & data, double & qty_formated) {
	QRegExp rx("^\\s*(\\d+([.,]?\\d+)?)\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());
// 	PR(rx.cap(4).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		qty_formated = rx.cap(1).toDouble();
		return true;
	}
	return false;
}

bool DataParser::quantity(const QString & data, QString & qty_formated) {
	double quantity;
	
	bool status = DataParser::quantity(data, quantity);
	qty_formated.sprintf("%.2f", quantity);

	return status;
}

bool DataParser::price(const QString & data, double & price_formated, double & tax_formated) {
	QRegExp rx("^\\s*(\\d+([.,]?\\d+)?)\\s*([p\\+]\\s*(22|15|7)\%?)?\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());
// 	PR(rx.cap(4).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		price_formated = rx.cap(1).toDouble();
		tax_formated = rx.cap(4).toDouble();
		return true;
	}
	return false;
}

bool DataParser::price(const QString & data, QString & price_formated) {
	double price, tax;
	
	bool status = DataParser::price(data, price, tax);
	price_formated.sprintf("%.2f zl", price*(100.0+tax)/100.0);

	return status;
}

bool DataParser::unit(const QString & data, QString & unit_formated) {
	QRegExp rx("^\\s*(\\d+([.,]?\\d+)?)\\s*(ml|l|mg|g|kg|szt)?\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		unit_formated.sprintf("%.2f %s", rx.cap(1).toDouble(), rx.cap(3).toStdString().c_str());
		return true;
	}
	return false;
}

bool DataParser::date(const QString & data, QDate & date_formated, const QDate & ref) {
	QRegExp rx1("^\\s*([+-]\\d+|0)\\s*$");
	QRegExp rx2("^\\s*dzis\\s*$");
	QRegExp rx3("^\\s*(\\d\\d?)([.:;,-/ ](\\d\\d?)([.:;,-/ ](\\d\\d\\d\\d))?)?\\s*$");	// dd/mm//yy
// 	QRegExp rx3("^\\s*(\\d\\d\\d\\d?)([.:;,-/ ](\\d\\d?)([.:;,-/ ](\\d\\d))?)?\\s*$");	// dd/mm//yy

	rx1.indexIn(data);
	rx2.indexIn(data);
	rx3.indexIn(data);
	if (!rx1.cap(0).isEmpty()) {
		date_formated = ref.addDays(rx1.cap(0).toInt());
		return true;
	} else if (!rx2.cap(0).isEmpty()) {
		date_formated = QDate::currentDate();
		return true;
	} else if (!rx3.cap(0).isEmpty()) {
		date_formated = QDate::currentDate();
		int year = rx3.cap(5).isEmpty() ? date_formated.year() : rx3.cap(5).toInt();
		int month = rx3.cap(3).isEmpty() ? date_formated.month() : rx3.cap(3).toInt();
		int day = rx3.cap(1).isEmpty() ? date_formated.day() : rx3.cap(1).toInt();
		date_formated.setDate(year, month, day);
		return date_formated.isValid();
	}
	return false;
}

bool DataParser::date(const QString & data, QString & date_formated, const QDate & ref) {
	QDate date;
	bool status = DataParser::date(data,  date, ref);
	date_formated = date.toString(Qt::DefaultLocaleShortDate);
	return status;
}