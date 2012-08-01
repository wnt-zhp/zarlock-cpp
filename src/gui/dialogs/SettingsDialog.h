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

// #include <QDate>
// #include <QString>
// #include <QVector>

#include "ProgramSettings.h"
#include "ui_SettingsDialog.h"

class SettingsDialog : public QDialog, public Ui::SettingsDialog {
	Q_OBJECT
public:
	SettingsDialog(QDialog * parent = NULL);
	virtual ~SettingsDialog();

public slots:
	virtual int exec();

private:
	ProgramSettings * progset;

	enum EncodingCode {UTF8, CP1250, ISO88592};

	#if QT_VERSION >= 0x040800
	static const QStringList exportFormatsNames;
	static const QStringList exportFormatsCodes;

	static const QStringList encodingNames;
	static const QStringList encodingCodes;
	static const QStringList separatorCodes;
	#else
	QStringList exportFormatsNames;
	QStringList exportFormatsCodes;

	QStringList encodingNames;
	QStringList encodingCodes;
	QStringList separatorCodes;
	#endif /* QT_VERSION >= 0x040800 */

	static QSettings appSettings;
};

#endif /* SETTINGSDIALOG_H */
