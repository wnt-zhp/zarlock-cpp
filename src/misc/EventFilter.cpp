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


#include "EventFilter.h"
#include <qcoreevent.h>

#include "globals.h"

// bool EventFilter::event(QEvent* )
// {
// return QObject::event();
// }

bool EventFilter::eventFilter(QObject * obj, QEvent * event) {
	if (event->type() == QEvent::Resize) {
// 		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
// 		qDebug("Ate key press %d", keyEvent->key());

// 		QVector<QObject *>::iterator f = qFind(array, obj);
		emit resized();

		return true;
	} else {
		// standard event processing
		return QObject::eventFilter(obj, event);
	}
}

// EventFilter::addMonitoredObjects(QObject * obj) {
// 	array.push_back(obj);
// }

#include "EventFilter.moc"