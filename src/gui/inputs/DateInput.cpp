/*
    This file is part of Zarlok.

    Copyright (C) 2010  Rafał Lalik <rafal.lalik@ph.tum.de>

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

#include "globals.h"
#include "DataParser.h"

#include "DateInput.h"

DateInput::DateInput(QWidget * parent) : CustomLineEdit(parent), data_ref(NULL) {
	setPlaceholderText(tr("Expiry date"));
}

void DateInput::setDateReferenceObj(const DateInput * ref) {
	data_ref = ref;
}

const QDate DateInput::date() const { // TODO: Fix this buggy place
	QDate d;
// 	PR(displaytext.toStdString());
	if (is_ok) {
		DataParser::date(rawtext, d);
// 		d = QDate::fromString(displaytext, Qt::DefaultLocaleShortDate);
	}
// 	PR(d.toString(Qt::ISODate).toStdString());
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

	if (ref.toString().isEmpty()) {
// 		placeholder = tr("To obtain correct expiry date, set registration date first");
		placeholder.clear();
		setPlaceholderText(tr("To obtain correct expiry date, set registration date first"));
// 		this->setEnabled(false);
		return true;
	}
// 	this->setEnabled(true);
// 	PR(QDate::currentDate().toString(Qt::ISODate).toStdString());
	return DataParser::date(raw, placeholder, ref);
}

void DateInput::doRefresh() {
	CustomLineEdit::doRefresh();
}

#include "DateInput.moc"