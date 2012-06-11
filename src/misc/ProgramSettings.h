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


#ifndef PROGRAMSETTING_H
#define PROGRAMSETTING_H

#include <QObject>

class ProgramSettings : public QObject {
Q_OBJECT
public:
	static ProgramSettings * Instance();
	static void Destroy();
	virtual ~ProgramSettings();

	void loadSettings();
	void saveSettings();

signals:
	void settingsUpdated();

private:
	ProgramSettings();
	ProgramSettings(const ProgramSettings &);

public:
	bool doUpdateCheck;			// Check for updates on startup
// 	QString campName;			// camp name
// 	QString campPlace;			// camp place
// 	QString campOrg;			// camp organizator
// 	QDate campDateBegin;		// camp begin date
// 	QDate campDateEnd;			// camp end date
// 	int scoutsNo;				// amount of scoutsNo
// 	int leadersNo;				// amount of leaders
// 	double avgCosts;			// average meal costs per day
// 	QString campLeader;			// leader's name
// 	QString campQuarter;		// quartermaster's name
// 	QString campOthers;			// others

private:
	static ProgramSettings * pset;
};

#endif /* PROGRAMSETTING_H */
