/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "CampProperties.h"

#include <QSqlQuery>
#include <QVariant>

void CampProperties::writeCampSettings() {
	QSqlQuery csq;
	QString query("UPDATE settings SET value=\"%2\" WHERE key=\"%1\";");

	csq.exec(query.arg(CampProperties::HisCorrect).arg(this->isCorrect));
	csq.exec(query.arg(CampProperties::HcampName).arg(this->campName));
	csq.exec(query.arg(CampProperties::HcampPlace).arg(this->campPlace));
	csq.exec(query.arg(CampProperties::HcampOrg).arg(this->campOrg));
	csq.exec(query.arg(CampProperties::HcampDateBegin).arg(this->campDateBegin.toString(Qt::ISODate)));
	csq.exec(query.arg(CampProperties::HcampDateEnd).arg(this->campDateEnd.toString(Qt::ISODate)));
	csq.exec(query.arg(CampProperties::HscoutsNo).arg(this->scoutsNo));
	csq.exec(query.arg(CampProperties::HleadersNo).arg(this->leadersNo));
	csq.exec(query.arg(CampProperties::HAvgCosts).arg(this->avgCosts));
	csq.exec(query.arg(CampProperties::HcampLeader).arg(this->campLeader));
	csq.exec(query.arg(CampProperties::HcampQuarter).arg(this->campQuarter));
	csq.exec(query.arg(CampProperties::HcampOthers).arg(this->campOthers));
}

void CampProperties::readCampSettings() {
	QSqlQuery csq;
	csq.exec("SELECT * FROM settings;");
	while(csq.next()) {
		switch (csq.value(0).toInt()) {
			case CampProperties::HisCorrect:
				this->isCorrect = csq.value(1).toBool();
				break;
			case CampProperties::HcampName:
				this->campName = csq.value(1).toString();
				break;
			case CampProperties::HcampPlace:
				this->campPlace = csq.value(1).toString();
				break;
			case CampProperties::HcampOrg:
				this->campOrg = csq.value(1).toString();
				break;
			case CampProperties::HcampDateBegin:
				this->campDateBegin = csq.value(1).toDate();
				break;
			case CampProperties::HcampDateEnd:
				this->campDateEnd = csq.value(1).toDate();
				break;
			case CampProperties::HscoutsNo:
				this->scoutsNo = csq.value(1).toInt();
				break;
			case CampProperties::HleadersNo:
				this->leadersNo = csq.value(1).toInt();
				break;
			case CampProperties::HAvgCosts:
				this->avgCosts = csq.value(1).toDouble();
				break;
			case CampProperties::HcampLeader:
				this->campLeader = csq.value(1).toString();
				break;
			case CampProperties::HcampQuarter:
				this->campQuarter = csq.value(1).toString();
				break;
			case CampProperties::HcampOthers:
				this->campOthers = csq.value(1).toString();
				break;
		}
	}
}