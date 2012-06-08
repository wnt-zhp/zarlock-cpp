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


#ifndef UNITINPUT_H
#define UNITINPUT_H

#include "CustomLineEdit.h"

class UnitInput : public CustomLineEdit {
Q_OBJECT
public:
    UnitInput(QWidget * parent);

private slots:
	bool verifyText(const QString &, QString & );
};

#endif // UNITINPUT_H
