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


#ifndef CAMPSETTINGSDIALOG_H
#define CAMPSETTINGSDIALOG_H

#include "ui_CampSettingsDialog.h"

#include "CampProperties.h"

class CampSettingsDialog : public QDialog, public Ui::CampSettingsDialog {
Q_OBJECT
public:
	CampSettingsDialog(CampProperties * cp, QDialog * parent = NULL);
	virtual ~CampSettingsDialog();

private:
	void accept();

private slots:
	void verify();
	void verifyStartDate();
	void verifyStopDate();

private:
	CampProperties * camp;
};

#endif // CAMPSETTINGSDIALOG_H
