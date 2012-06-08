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

#include "TextInput.h"

TextInput::TextInput(QWidget * parent) : CustomLineEdit(parent), allow_empty(false) {
	setPlaceholderText(tr("Expiry date"));
}

bool TextInput::verifyText(const QString & raw, QString & placeholder) {
	if (DataParser::text(raw, placeholder, this->allow_empty)) {
		return true;
	}
	return false;
}

#include "TextInput.moc"