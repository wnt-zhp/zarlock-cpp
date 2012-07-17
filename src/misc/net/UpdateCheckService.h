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


#ifndef UPDATECHECKSERVICE_H
#define UPDATECHECKSERVICE_H

#include "AbstractNetworkService.h"

class UpdateCheckService : public AbstractNetworkService {
Q_OBJECT
public:
	UpdateCheckService(QNetworkAccessManager& netmanager, QObject* parent = 0);

signals:
	void downloadUrlFound(const QString & url);

public slots:
	virtual QNetworkReply * sendRequest();
	virtual QNetworkReply * sendRequest(const QUrl & url);

protected slots:
	virtual void requestFinished();

private:
	enum DWCodes { DURL, DWIN, DDEB, DRPM, DTBZ2, DDUMMY };
	QString url_download_values[DDUMMY];
// 	QString url_download_win;
// 	QString url_download_deb;
// 	QString url_download_rpm;
};

#endif // UPDATECHECKSERVICE_H