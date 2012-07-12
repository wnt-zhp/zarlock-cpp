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


#ifndef QTYSPININPUT_H
#define QTYSPININPUT_H

#include <QSpinBox>

class QtySpinInput : public QSpinBox {
public:
	explicit QtySpinInput(QWidget* parent = 0);

	virtual QString textFromValue(int val) const;
	virtual int valueFromText(const QString& text) const;

	virtual QValidator::State validate(QString& input, int& pos) const;
};

#endif // QTYSPININPUT_H
