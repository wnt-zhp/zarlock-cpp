/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#ifndef __CAMPPROPERTIES_H__
#define __CAMPPROPERTIES_H__

#include <QDate>
#include <QString>

struct CampProperties {
	bool isCorrect;				// is camp set correct
	QString campName;			// camp name
	QDate campDateBegin;		// camp begin date
	QDate campDateEnd;			// camp end date
	int scoutsNo;				// amount of scoutsNo
	int leadersNo;				// amount of leaders
	QString campLeader;			// leader's name
	QString campQuarter;		// quartermaster's name
	QString campOthers;			// others

	enum dbkeys {	HisCorrect		= 0x001,
					HcampName		= 0x002,
					HcampDateBegin	= 0x004,
					HcampDateEnd	= 0x008,
					HscoutsNo		= 0x010,
					HleadersNo		= 0x020,
					HcampLeader		= 0x040,
					HcampQuarter	= 0x080,
					HcampOthers		= 0x100
	};
	void writeCampSettings();
	void readCampSettings();
};

#endif /* __CAMPPROPERTIES_H__ */