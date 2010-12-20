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
}

bool DateInput::verifyText(const QString & raw, QString & placeholder) {
	if (DataParser::date(raw, placeholder)) {
		return true;
	}
	return false;
}

#include "DateInput.moc"