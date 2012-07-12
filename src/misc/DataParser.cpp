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

#include <QDate>
#include <QRegExp>

#include "globals.h"
#include "DataParser.h"

bool DataParser::text(const QString & data, QString & text_parsed, bool allow_empty) {
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
		text_parsed = data;
		return true;
	}
	text_parsed.clear();
	return allow_empty;
}

bool DataParser::quantity(const QString & data, int & qty_parsed) {
	QRegExp rx("^\\s*((\\d+)?(([.,]?\\d+))?)\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());
// 	PR(rx.cap(4).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		double qty = rx.cap(1).toDouble();
		if (qty > 0)
			qty_parsed = qty * 100.0 + 0.5;
			return true;
	}
	return false;
}

bool DataParser::quantity(const QString & data, QString & qty_parsed) {
	int quantity;

	bool status = DataParser::quantity(data, quantity);
	qty_parsed = QObject::tr("%1.%2").arg(quantity/100).arg(quantity % 100, 2, 10, QChar('0'));

	return status;
}

bool DataParser::price(const QString & data, double & netto_parsed, int & vat_parsed) {
	QRegExp rx("^\\s*(\\d+([.,]?\\d+)?)\\s*([p\\+]\\s*(23|8|5|0)\%?)?\\s*(zl)?\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());
// 	PR(rx.cap(4).toStdString());
// 	PR(rx.cap(5).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		netto_parsed = rx.cap(1).toDouble();
		vat_parsed = rx.cap(4).toInt()*100;
		return true;
	}
	return false;
}

bool DataParser::price(const QString & data, int & price_parsed) {
	double price; int tax;
	
	bool status = DataParser::price(data, price, tax);
	// 	price_parsed.sprintf(QObject::tr("%.2f zl").toStdString().c_str(), 333);//0.01*price*(100.0+tax));
	price_parsed = price*(100+tax) + 0.5;
	
	return status;
}

bool DataParser::price(const QString & data, QString & price_parsed) {
	int price;

	bool status = DataParser::price(data, price);
// 	price_parsed.sprintf(QObject::tr("%.2f zl").toStdString().c_str(), 333);//0.01*price*(100.0+tax));
	price_parsed = QObject::tr("%1.%2 zl").arg(price/100).arg(price % 100, 2, 10, QChar('0'));

	return status;
}

bool DataParser::unit(const QString & data, QString & unit_parsed) {
	QRegExp rx("^\\s*(\\d+([.,]?\\d+)?)?\\s*(ml|l|mg|g|kg|szt|op)?\\s*$");

// 	PR(rx.indexIn(data));
// 	PR(rx.cap(0).toStdString());
// 	PR(rx.cap(1).toStdString());
// 	PR(rx.cap(2).toStdString());
// 	PR(rx.cap(3).toStdString());

	rx.indexIn(data);
	if (!rx.cap(0).isEmpty()) {
		unit_parsed.sprintf("%.2f %s", rx.cap(1).isEmpty() ? 1.0 : rx.cap(1).toDouble(), rx.cap(3).toStdString().c_str());
		return true;
	}
	return false;
}

bool DataParser::date(const QString & data, QDate & date_parsed, const QDate & ref) {
	QRegExp rx1("^\\s*([+-]\\d+|0)\\s*$");
	QRegExp rx2("^\\s*(d|dzis|dziś)\\s*$");
	QRegExp rx3("^\\s*(\\d\\d?)([.:;,-/ ](\\d\\d?)([.:;,-/ ](\\d\\d\\d\\d))?)?\\s*$");	// dd/mm//yy
// 	QRegExp rx3("^\\s*(\\d\\d\\d\\d?)([.:;,-/ ](\\d\\d?)([.:;,-/ ](\\d\\d))?)?\\s*$");	// dd/mm//yy

	rx1.indexIn(data);
	rx2.indexIn(data);
	rx3.indexIn(data);
	if (!rx1.cap(0).isEmpty()) {
		date_parsed = ref.addDays(rx1.cap(0).toInt());
		return true;
	} else if (!rx2.cap(0).isEmpty()) {
		date_parsed = QDate::currentDate();
		return true;
	} else if (!rx3.cap(0).isEmpty()) {
		date_parsed = QDate::currentDate();
		int year = rx3.cap(5).isEmpty() ? date_parsed.year() : rx3.cap(5).toInt();
		int month = rx3.cap(3).isEmpty() ? date_parsed.month() : rx3.cap(3).toInt();
		int day = rx3.cap(1).isEmpty() ? date_parsed.day() : rx3.cap(1).toInt();
		date_parsed.setDate(year, month, day);
		return date_parsed.isValid();
	}
	return false;
}

bool DataParser::date(const QString & data, QString & date_parsed, const QDate & ref) {
	QDate date;
	bool status = DataParser::date(data,  date, ref);
	date_parsed = date.toString(Qt::DefaultLocaleShortDate);
	return status;
}
