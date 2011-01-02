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


#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <qt4/QtGui/QLineEdit>


class CustomLineEdit : public QLineEdit {
Q_OBJECT
public:
    CustomLineEdit(QWidget * parent);
    virtual ~CustomLineEdit();

    virtual void focusOutEvent(QFocusEvent* );
    virtual void focusInEvent(QFocusEvent* );

	bool ok();
	
	inline virtual QString text() const { return rawtext; }
	inline virtual void setText(const QString & t) { rawtext = t; }

protected:
	virtual bool verifyText(const QString &, QString & ) = 0;

private slots:
	bool verify(const QString &);

protected:
	QString displaytext, rawtext;
	bool edit_mode, is_ok;
};

#endif // CUSTOMLINEEDI