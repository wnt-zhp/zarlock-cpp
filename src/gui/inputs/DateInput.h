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


#ifndef DATEINPUT_H
#define DATEINPUT_H

#include "CustomLineEdit.h"
#include <QDate>

class DateInput : public CustomLineEdit {
Q_OBJECT
public:
	DateInput(QWidget * parent);

	const QDate date() const;
	void setDateReferenceObj(const DateInput * ref);

public slots:
	void doRefresh();

private slots:
	bool verifyText(const QString &, QString &);

private:
	const DateInput * data_ref;
};

#endif // DATEINPUT_H
