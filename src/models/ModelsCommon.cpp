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

#include "globals.h"

#include <QtGui/QMessageBox>
#include <QtCore/QString>
#include <QPushButton>

#include "ModelsCommon.h"
#include <QLayout>
#include <QCheckBox>
#include <QAction>
#include <Database.h>

#include <QStringBuilder>

void ModelsCommon::inputErrorMsgBox(const QString& val) {
	QMessageBox msgBox;
	msgBox.setText(QObject::tr("Value <b>%1</b> which you try store in database is not valid.").arg(val));
	msgBox.setInformativeText(QObject::tr("Try to edit your data again, pay attention to its validity."));
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}

bool ModelsCommon::productRemoveConfirmation(int count, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove %n product(s) with all associated batches and distributtions.", NULL, count));

	msgBox.setInformativeText(QObject::tr("All associated batches, distributions and meals will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::batchRemoveConfirmation(int count, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove %n batch(es) with all distributions.", NULL, count));

	msgBox.setInformativeText(QObject::tr("All distributions and meals will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::distributeRemoveConfirmation(int count, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove %n distribution(s).", NULL, count));

	msgBox.setInformativeText(QObject::tr("All distributions will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::mealdayRemoveConfirmation(int count, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove %n day(s) from your meals list.", NULL, count));

	msgBox.setInformativeText(QObject::tr("All meals assigned to this day will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

#include "ModelsCommon.moc"