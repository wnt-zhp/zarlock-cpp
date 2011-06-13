

#ifndef __CAMPPROPERTIES_H__
#define __CAMPPROPERTIES_H__

#include <QDate>
#include <QString>

struct CampProperties {
	bool isCorrect;				// is camp set correct
	QString campName;			// camp name
	QDate campDateBegin;		// camp begin date
	QDate campDateEnd;			// camp end date
	int scoutsNo;				// amount of scoutsNo
	int leadersNo;				// amount of leaders
	QString campLeader;			// leader's name
	QString campQuarter;		// quartermaster's name
	QString campOthers;			// others

	enum dbkeys {	HisCorrect		= 0x001,
					HcampName		= 0x002,
					HcampDateBegin	= 0x004,
					HcampDateEnd	= 0x010,
					HscoutsNo		= 0x020,
					HleadersNo		= 0x040,
					HcampLeader		= 0x080,
					HcampQuarter	= 0x100,
					HcampOthers		= 0x200
	};
};

#endif /* __CAMPPROPERTIES_H__ */