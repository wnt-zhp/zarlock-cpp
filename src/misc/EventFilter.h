/*
    This file is part of Zarlok.

    Copyright (C) 2012  Rafał Lalik <rafal.lalik@zhp.net.pl>

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


#ifndef EVENTFILTER_H
#define EVENTFILTER_H

#include <QObject>
#include <QVector>
#include <QEvent>

class EventFilter : public QObject {
Q_OBJECT
public:
	explicit EventFilter(QObject* parent = 0);

	void setEmitObjects(bool emit_objects);

	virtual bool eventFilter(QObject* , QEvent* );

	virtual void registerFilter(QEvent::Type type);

signals:
	void eventFiltered(QObject *, QEvent *);
	void eventFiltered(QEvent *);

private:
	bool emit_objects;
	QVector<QEvent::Type> events_filter;
};

#endif // EVENTFILTER_H
