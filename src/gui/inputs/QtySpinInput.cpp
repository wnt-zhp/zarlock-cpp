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

#include "QtySpinInput.h"

QtySpinInput::QtySpinInput(QWidget* parent): QSpinBox(parent) {
	setSingleStep(100);
}

QString QtySpinInput::textFromValue(int val) const {
	int ival = val+0.5;
	return tr("%1.%2").arg(ival/100).arg(ival % 100, 2, 10, QChar('0'));
//	return QSpinBox::textFromValue(val);
}

int QtySpinInput::valueFromText(const QString& text) const {
	double d = text.section(" ", 0, 0).toDouble();
	return static_cast<int>(d * 100.0 + 0.5);
}

QValidator::State QtySpinInput::validate(QString& input, int& pos) const {
	QCharRef cr = input[pos-1];
	if (!cr.isDigit()) {
		return (cr == '.' or cr == ',') ? QValidator::Acceptable : QValidator::Invalid;
	}
	return QValidator::Acceptable;
// 	return QSpinBox::validate(input, pos);
}

#include "QtySpinInput.moc"