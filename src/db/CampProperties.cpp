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

#include "CampProperties.h"
#include "Database.h"
#include "globals.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#if QT_VERSION >= 0x040800

const QVector<QString> CampProperties::dbsetkeys = QVector<QString>({ 
		"IsDatabaseCorrect", "CampName", "CampPlace", "CampOrg",
		"CampDateBegin", "CampDateEnd", "ScoutsNo", "LeadersNo",
		"AvgCosts", "CampLeader", "CampQuarter", "CampOthers"
});

#else

CampProperties::CampProperties() {
	dbsetkeys.reserve(Hdummy);
	dbsetkeys.push_back("IsDatabaseCorrect");
	dbsetkeys.push_back("CampName");
	dbsetkeys.push_back("CampPlace");
	dbsetkeys.push_back("CampOrg");
	dbsetkeys.push_back("CampDateBegin");
	dbsetkeys.push_back("CampDateEnd");
	dbsetkeys.push_back("ScoutsNo");
	dbsetkeys.push_back("LeadersNo");
	dbsetkeys.push_back("AvgCosts");
	dbsetkeys.push_back("CampLeader");
	dbsetkeys.push_back("CampQuarter");
	dbsetkeys.push_back("CampOthers");
}

#endif /* QT_VERSION >= 0x040800 */

void CampProperties::writeCampSettings() {
	QSqlQuery csq;
	QString query("INSERT OR REPLACE INTO settings(key, value) VALUES(\"%1\", \"%2\");");

	csq.exec(query.arg(dbsetkeys[CampProperties::HisCorrect]).arg(this->isCorrect));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampName]).arg(this->campName));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampPlace]).arg(this->campPlace));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampOrg]).arg(this->campOrg));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampDateBegin]).arg(this->campDateBegin.toString(Qt::ISODate)));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampDateEnd]).arg(this->campDateEnd.toString(Qt::ISODate)));
	csq.exec(query.arg(dbsetkeys[CampProperties::HscoutsNo]).arg(this->scoutsNo));
	csq.exec(query.arg(dbsetkeys[CampProperties::HleadersNo]).arg(this->leadersNo));
	csq.exec(query.arg(dbsetkeys[CampProperties::HAvgCosts]).arg(this->avgCosts));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampLeader]).arg(this->campLeader));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampQuarter]).arg(this->campQuarter));
	csq.exec(query.arg(dbsetkeys[CampProperties::HcampOthers]).arg(this->campOthers));
}

void CampProperties::readCampSettings() {
	QSqlQuery csq;
	csq.exec("SELECT * FROM settings;");
	while(csq.next()) {
		QVector<QString>::const_iterator fpos = qFind(dbsetkeys.begin(), dbsetkeys.end(), csq.value(0).toString());
		switch (fpos - dbsetkeys.begin()) {
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

void CampProperties::resetCampSettings() {
	isCorrect = false;				// is camp set correct
	campName = "";					// camp name
	campPlace = "";					// camp place
	campOrg = "";					// camp organizator
	campDateBegin = QDate();		// camp begin date
	campDateEnd = QDate();			// camp end date
	scoutsNo = 0;					// amount of scoutsNo
	leadersNo = 0;					// amount of leaders
	avgCosts = 0;					// average meal costs per day
	campLeader = "";				// leader's name
	campQuarter = "";				// quartermaster's name
	campOthers = "";				// others
}
