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


#ifndef ABSTRACTNETWORKSERVICE_H
#define ABSTRACTNETWORKSERVICE_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QProgressDialog>

class AbstractNetworkService : public QObject {
Q_OBJECT
public:
	AbstractNetworkService(QNetworkAccessManager & netmanager, QObject * parent = NULL);
	virtual ~AbstractNetworkService();

public slots:
	virtual QNetworkReply * sendRequest(const QUrl & url);
	virtual QNetworkReply * sendRequest(const QString & url);

protected slots:
	virtual void requestFinished() = 0;

	virtual void error(QNetworkReply::NetworkError);
	virtual void metaDataChanged();

protected:
	QNetworkReply * reply;

private:
	QNetworkAccessManager & nam;

	QString request;
};

#endif // ABSTRACTNETWORKSERVICE_H
