/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#ifndef DIMMINGMESSAGE_H
#define DIMMINGMESSAGE_H

#include "AbstractDimmingWidget.h"
#include <QStyle>
#include <QLabel>

class DimmingMessage : public AbstractDimmingWidget {
public:
	DimmingMessage(QWidget * parent = NULL);
	virtual ~DimmingMessage();

	void setMessage(const QString & message);
	void setIcon(QIcon * icon = NULL);

private:
	QStyle * st;
	QLabel * icon;
// 	QProgressIndicator * busy;
	QLabel * label;
};

#endif // DIMMINGMESSAGE_H
