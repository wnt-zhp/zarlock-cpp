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
#include "CampSettingsDialog.h"

#include <QPushButton>
#include <QCalendarWidget>
#include <QStringBuilder>

CampSettingsDialog::CampSettingsDialog(CampProperties * cp, QDialog * /*parent*/) : Ui::CampSettingsDialog(), camp(cp) {
	setupUi(this);

	buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);

	period_start->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	period_stop->calendarWidget()->setFirstDayOfWeek(Qt::Monday);

	period_start->setWrapping(true);
// 	period_start->setAccelerated(true);

	period_stop->setWrapping(true);
// 	period_stop->setAccelerated(true);

	edit_name->setText(camp->campName);
	edit_place->setText(camp->campPlace);
	edit_org->setText(camp->campOrg);

	if (camp->isCorrect) {
		edit_leader->setText(camp->campLeader);
		edit_qmaster->setText(camp->campQuarter);
		edit_others->setText(camp->campOthers);
		spin_scouts->setValue(camp->scoutsNo);
		spin_leaders->setValue(camp->leadersNo);
		spin_avgcosts->setValue(camp->avgCosts);
		period_start->setDate(camp->campDateBegin);
		period_stop->setDate(camp->campDateEnd);
	} else {
		period_stop->setDate(QDate::currentDate());
		period_start->setDate(QDate::currentDate());
	}

	connect(edit_name, SIGNAL(textChanged(QString)), this, SLOT(verify()));
	connect(edit_place, SIGNAL(textChanged(QString)), this, SLOT(verify()));
	connect(edit_org, SIGNAL(textChanged(QString)), this, SLOT(verify()));
	connect(edit_leader, SIGNAL(textChanged(QString)), this, SLOT(verify()));
	connect(edit_qmaster, SIGNAL(textChanged(QString)), this, SLOT(verify()));
	connect(spin_scouts, SIGNAL(valueChanged(int)), this, SLOT(verify()));
	connect(spin_leaders, SIGNAL(valueChanged(int)), this, SLOT(verify()));
	connect(spin_avgcosts, SIGNAL(valueChanged(double)), this, SLOT(verify()));
	connect(period_start, SIGNAL(editingFinished()), this, SLOT(verifyStartDate()));
	connect(period_stop, SIGNAL(editingFinished()), this, SLOT(verifyStopDate()));

	verifyStartDate();
}

CampSettingsDialog::~CampSettingsDialog() {
	DII();
}

void CampSettingsDialog::verifyStartDate() {
	QDate stadate = period_start->date();
	QDate stodate = period_stop->date();

	if (stadate.daysTo(stodate) < 0) {
		stodate = stadate;
		period_stop->setDate(stodate);
	}

	int days = period_start->date().daysTo(period_stop->date())+1;
	label_days->setText(QString("= ") % tr("%n days", "", days));

	verify();
}

void CampSettingsDialog::verifyStopDate() {
	QDate stadate = period_start->date();
	QDate stodate = period_stop->date();

	if (stadate.daysTo(stodate) < 0) {
		stadate = stodate;
		period_start->setDate(stadate);
	}

	int days = period_start->date().daysTo(period_stop->date())+1;
	label_days->setText(QString("= ") % tr("%n days", "", days));

	verify();
}

void CampSettingsDialog::verify() {
	if (	!edit_name->text().isEmpty()		&&
			!edit_place->text().isEmpty()		&&
			!edit_org->text().isEmpty()			&&
			!edit_leader->text().isEmpty()		&&
			!edit_qmaster->text().isEmpty()		&&
			spin_scouts->value() >= 0			&&
			spin_leaders->value()>= 0			&&
			spin_avgcosts->value()>= 0
	) {
		buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
	} else {
		buttonBox->button(QDialogButtonBox::Save)->setEnabled(false);
	}
}

void CampSettingsDialog::accept() {
	camp->isCorrect = true;
	camp->campName = edit_name->text();
	camp->campPlace = edit_place->text();
	camp->campOrg = edit_org->text();
	camp->campLeader = edit_leader->text();
	camp->campQuarter = edit_qmaster->text();
	camp->campOthers = edit_others->text();
	camp->scoutsNo = spin_scouts->value();
	camp->leadersNo = spin_leaders->value();
	camp->avgCosts= spin_avgcosts->value();
	camp->campDateBegin = period_start->date();
	camp->campDateEnd = period_stop->date();

    QDialog::accept();
}

#include "CampSettingsDialog.moc"