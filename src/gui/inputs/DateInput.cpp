/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010  Rafał Lalik <rafal.lalik@ph.tum.de>

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

#include "DateInput.h"

DateInput::DateInput(QWidget * parent) : CustomLineEdit(parent) {
	setPlaceholderText(tr("Expiry date"));
	data_ref = NULL;
}

void DateInput::setDateReferenceObj(const DateInput * ref) {
	data_ref = ref;
}

const QDate DateInput::date() const {
	QDate d;
// 	PR(displaytext.toStdString());
	d = QDate::fromString(displaytext, "yyyy-MM-dd");
// 	PR(d.toString("yyyy-MM-dd").toStdString());
/*	if (DataParser::date(rawtext, d, data_ref->date())) {
		return d;
	}*/
	return d;
}

bool DateInput::verifyText(const QString & raw, QString & placeholder) {
	QDate ref;
	if (data_ref == NULL)
		ref = QDate::currentDate();
	else
		ref = data_ref->date();

// 	PR(QDate::currentDate().toString("yyyy-MM-dd").toStdString());
	if (DataParser::date(raw, placeholder, ref)) {
		return true;
	}
	return false;
}

#include "DateInput.moc"