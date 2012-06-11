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

SettingsDialog::SettingsDialog(QDialog * /*parent*/) : Ui::SettingsDialog() {
	setupUi(this);
	progset = ProgramSettings::Instance();
}

SettingsDialog::~SettingsDialog() {
	DII();
}

int SettingsDialog::exec() {
	progset->loadSettings();
	cb_adv_updatecheck->setChecked(progset->doUpdateCheck);

	int ret = QDialog::exec();

	if (!ret)
		return ret;

	progset->doUpdateCheck = cb_adv_updatecheck->isChecked();
	progset->saveSettings();

	return ret;
}


#include "SettingsDialog.moc"