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


#include "EventFilter.h"
#include <qcoreevent.h>
#include <QContextMenuEvent>

#include "globals.h"

EventFilter::EventFilter(QObject* parent): QObject(parent), emit_objects(false) {
}

void EventFilter::setEmitObjects(bool emit_objects) {
	this->emit_objects = emit_objects;
}

void EventFilter::registerFilter(QEvent::Type type) {
	events_filter.push_back(type);
}

bool EventFilter::eventFilter(QObject * obj, QEvent * event) {
// 	PR(event->type());
	for (QVector<QEvent::Type>::iterator it = events_filter.begin(); it != events_filter.end(); ++it) {
		if (event->type() == *it) {
			if (emit_objects)
				emit eventFiltered(obj, event);
			else
				emit eventFiltered(event);

			return true;
		}
	}

	// standard event processing
	return QObject::eventFilter(obj, event);
}

#include "EventFilter.moc"