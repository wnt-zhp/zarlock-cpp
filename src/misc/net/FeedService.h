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


#ifndef FEEDSERVICE_H
#define FEEDSERVICE_H

#include "AbstractNetworkService.h"

class FeedService : public AbstractNetworkService {
Q_OBJECT
public:
	FeedService(QNetworkAccessManager& netmanager, QObject* parent = 0);
	virtual ~FeedService();

public slots:
	virtual void sendRequest();
	virtual void sendRequest(const QUrl & url);

private slots:
	virtual void requestFinished();
private:
};

#endif // FEEDSERVICE_H
