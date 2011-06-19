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
	msgBox.setText(QObject::tr("Value \"%1\" which you try store in database is not valid.").arg(val));
	msgBox.setInformativeText(QObject::tr("Try to edit your data again, pay attention to its validity."));
	msgBox.setIcon(QMessageBox::Critical);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}

bool ModelsCommon::productRemoveConfirmation(const QString & prod) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove product '%1'").arg(prod));

	msgBox.setInformativeText(QObject::tr("Are you sure?"));

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::productRemoveConfirmation(const QString & prod, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove product '%1'").arg(prod));

	msgBox.setInformativeText(QObject::tr("It has associated batches. All associated batches will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::batchRemoveConfirmation(const QString & batch) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove batch '%1'").arg(batch));

	msgBox.setInformativeText(QObject::tr("Are you sure?"));

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::batchRemoveConfirmation(const QString & batch, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove batch '%1'").arg(batch));

	msgBox.setInformativeText(QObject::tr("It has associated distributed products and meals. All distributions and meals will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::distributeRemoveConfirmation(const QString & batch) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove distributed batch '%1'").arg(batch));

	msgBox.setInformativeText(QObject::tr("Are you sure?"));

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}

bool ModelsCommon::distributeRemoveConfirmation(const QString & batch, const QString & details) {
	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setText(QObject::tr("You try to remove distributed batch '%1'").arg(batch));

	msgBox.setInformativeText(QObject::tr("It has associated distributed products and meals. All distributions and meals will be removed. Are you sure?"));
	msgBox.setDetailedText(details);

	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);

	if (msgBox.exec() == QMessageBox::Yes)
		return true;

	return false;
}
#include "ModelsCommon.moc"