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

#include <QPushButton>
#include <QCalendarWidget>
#include <QStringBuilder>

#include "globals.h"
#include "SettingsDialog.h"

/**
 * @brief Instancja klasy Database
 **/
ProgramSettings * ProgramSettings::pset = NULL;

/**
 * @brief Tworzy i zwraca instację do obiektu Database
 *
 * @return Database &
 **/
ProgramSettings * ProgramSettings::Instance() {
	if (!pset) {
		pset = new ProgramSettings;
		FI();
		std::cout << "++ Create ProgramSettings instance\n";
	}

	return pset;
}

void ProgramSettings::Destroy() {
	delete pset;
	pset = NULL;
}

ProgramSettings::~ProgramSettings() {
	saveSettings();
}

ProgramSettings::ProgramSettings() : QObject() {
	loadSettings();
}

ProgramSettings::ProgramSettings(const ProgramSettings& ) : QObject() {
}

void ProgramSettings::loadSettings() {
	globals::appSettings->beginGroup("Preferences");
	doUpdateCheck		= globals::appSettings->value("doUpdateCheck", true).toBool();
	// 	resize			(globals::appSettings->value("size", QSize(860, 620)).toSize());
	// 	move			(globals::appSettings->value("pos", QPoint(200, 200)).toPoint());
	// 	restoreState	(globals::appSettings->value("state").toByteArray());
	globals::appSettings->endGroup();
}

void ProgramSettings::saveSettings() {
	globals::appSettings->beginGroup("Preferences");
	globals::appSettings->setValue("doUpdateCheck", this->doUpdateCheck);
	// 	globals::appSettings->setValue("size", size());
	// 	globals::appSettings->setValue("pos", pos());
	// 	globals::appSettings->setValue("state", saveState());
	globals::appSettings->endGroup();

	emit settingsUpdated();
}

#include "ProgramSettings.moc"