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

// #include <QStringBuilder>

#include "globals.h"
#include "SettingsDialog.h"

const QStringList SettingsDialog::exportFormatsNames = {"CSV (*.csv) - Comma Separated Values"};
const QStringList SettingsDialog::exportFormatsCodes = {"csv"};

const QStringList SettingsDialog::encodingNames = {"Windows-1250 (Windows recommended)", "UTF-8 (Linux recommended)", "ISO-8859-2"};
const QStringList SettingsDialog::encodingCodes = {"Windows-1250", "UTF-8", "ISO-8859-2"};
const QStringList SettingsDialog::separatorCodes = {",", ";", "|"};

SettingsDialog::SettingsDialog(QDialog * /*parent*/) : Ui::SettingsDialog() {
	setupUi(this);
	progset = ProgramSettings::Instance();

	for (int i = 0; i < exportFormatsNames.size(); ++i) {
		cb_KM_format->addItem(exportFormatsNames[i], exportFormatsCodes[i]);
		cb_SM_format->addItem(exportFormatsNames[i], exportFormatsCodes[i]);
// 		cb_ZZ_format->addItem(exportFormatsNames[i], exportFormatsCodes[i]);
	}

	for (int i = 0; i < encodingCodes.size(); ++i) {
		cb_format_csv_encoding->addItem(encodingNames[i], encodingCodes[i]);
	}

	for (int i = 0; i < separatorCodes.size(); ++i) {
		cb_format_csv_separator->addItem(separatorCodes[i], separatorCodes[i]);
	}

	fcb_font_family->setDisabled(true);
	cb_font_size->setDisabled(true);
	pb_font_color->setDisabled(true);
	pb_color_odd_normal->setDisabled(true);
	pb_color_odd_expired->setDisabled(true);
	pb_color_odd_expiring->setDisabled(true);
	pb_color_odd_valid->setDisabled(true);
	pb_color_even_normal->setDisabled(true);
	pb_color_even_expired->setDisabled(true);
	pb_color_even_expiring->setDisabled(true);
	pb_color_even_valid->setDisabled(true);
	cb_ZZ_format->setDisabled(true);
}

SettingsDialog::~SettingsDialog() {
	DII();
}

int SettingsDialog::exec() {
	progset->loadSettings();
	cb_adv_updatecheck->setChecked(progset->doUpdateCheck);

	cb_format_csv_encoding->setCurrentIndex(encodingCodes.indexOf(progset->csv_encoding));
	cb_format_csv_separator->setCurrentIndex(separatorCodes.indexOf(progset->csv_separator));
	
	int ret = QDialog::exec();

	if (!ret)
		return ret;

	progset->doUpdateCheck = cb_adv_updatecheck->isChecked();

	progset->csv_encoding = cb_format_csv_encoding->itemData(cb_format_csv_encoding->currentIndex()).toString();
	progset->csv_separator = cb_format_csv_separator->itemData(cb_format_csv_separator->currentIndex()).toString();

	progset->saveSettings();


// 	int idx = cb_KM_encoding->
	return ret;
}


#include "SettingsDialog.moc"