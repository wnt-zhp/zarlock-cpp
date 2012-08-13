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


#ifndef __CAMPPROPERTIES_H__
#define __CAMPPROPERTIES_H__

#include <QDate>
#include <QString>
#include <QVector>

struct CampProperties {
	bool isCorrect;				// is camp set correct
	QString campName;			// camp name
	QString campPlace;			// camp place
	QString campOrg;			// camp organizator
	QDate campDateBegin;		// camp begin date
	QDate campDateEnd;			// camp end date
	int scoutsNo;				// amount of scoutsNo
	int leadersNo;				// amount of leaders
	int avgCosts;			// average meal costs per day
	QString campLeader;			// leader's name
	QString campQuarter;		// quartermaster's name
	QString campOthers;			// others

	enum dbkeysnum {	HisCorrect,
						HcampName,
						HcampPlace,
						HcampOrg,
						HcampDateBegin,
						HcampDateEnd,
						HscoutsNo,
						HleadersNo,
						HAvgCosts,
						HcampLeader,
						HcampQuarter,
						HcampOthers,
						Hdummy
	};

#if QT_VERSION >= 0x040800
	static const QVector<QString> dbsetkeys;
#else
	QVector<QString> dbsetkeys;
#endif /* QT_VERSION >= 0x040800 */

	CampProperties();

	void writeCampSettings();
	void readCampSettings();
	void resetCampSettings();
};

#endif /* __CAMPPROPERTIES_H__ */
