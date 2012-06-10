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


#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDate>
#include <QString>
#include <QVector>

struct ProgramSettings {
	bool isCorrect;				// is camp set correct
	QString campName;			// camp name
	QString campPlace;			// camp place
	QString campOrg;			// camp organizator
	QDate campDateBegin;		// camp begin date
	QDate campDateEnd;			// camp end date
	int scoutsNo;				// amount of scoutsNo
	int leadersNo;				// amount of leaders
	double avgCosts;			// average meal costs per day
	QString campLeader;			// leader's name
	QString campQuarter;		// quartermaster's name
	QString campOthers;			// others
};

#include "ui_SettingsDialog.h"

class SettingsDialog : public QDialog, public Ui::SettingsDialog {
	Q_OBJECT
public:
	SettingsDialog(QDialog * parent = NULL);
	virtual ~SettingsDialog();

private:
	ProgramSettings * progset;
};

#endif /* SETTINGSDIALOG_H */
