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

#include "CustomLineEdit.h"

CustomLineEdit::CustomLineEdit(QWidget * parent) : QLineEdit(parent), edit_mode(false), is_ok(false) {
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(verify(QString)));
}

CustomLineEdit::~CustomLineEdit() {
	FPR(__func__);
}

bool CustomLineEdit::ok() {
	return is_ok;
}

void CustomLineEdit::focusOutEvent(QFocusEvent * ev) {
	this->setFont(globals::font_display);

	rawtext = QLineEdit::text();
	if (rawtext.isEmpty()) {
		displaytext.clear();
	} else if (!verify(rawtext)) {
		displaytext = tr("Parser error!");
	}

	edit_mode = false;
	QLineEdit::setText(displaytext);
    if (ev != NULL)
		QLineEdit::focusOutEvent(ev);
}

void CustomLineEdit::focusInEvent(QFocusEvent* ev) {
	// bez tego if'a kiedy znika popup completera to wywoływany
	// jest ponowny focusInEvent i psuje zawartość inputa
	if (!edit_mode) {
		QLineEdit::setFont(globals::font_edit);
		QLineEdit::setText(rawtext);
	}
	QLineEdit::focusInEvent(ev);
	edit_mode = true;
}

bool CustomLineEdit::verify(const QString & t) {
	if (!edit_mode)
		return false;

	if (verifyText(t, displaytext)) {
		this->setPalette(globals::palette_ok);
		is_ok = true;
	} else {
		this->setPalette(globals::palette_bad);
		is_ok = false;
	}
	return is_ok;
}

void CustomLineEdit::clear() {
	rawtext.clear();
	is_ok = false;
	QLineEdit::clear();
}

#include "CustomLineEdit.moc"