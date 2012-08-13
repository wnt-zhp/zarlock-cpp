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
#include "config.h"
#include "version.h"

#include "AbstractNetworkService.h"

AbstractNetworkService::AbstractNetworkService(QNetworkAccessManager & netmanager, QObject * parent) : QObject(parent), reply(NULL), nam(netmanager) {
	CI();
}

AbstractNetworkService::~AbstractNetworkService() {
	DI();
}

QNetworkReply * AbstractNetworkService::sendRequest(const QUrl & url) {
	if (reply) {
		reply->disconnect(this);
		reply->deleteLater();
	}

	QUrl _url = url;
	QByteArray iknow = qgetenv("I_KNOW_WHAT_I_AM_DOING");

	if (iknow.size())
		_url.addQueryItem("test_mode","true");


	QNetworkRequest nr;
	nr.setUrl(_url);
	nr.setRawHeader("User-Agent", "Zarlok");
	reply = nam.get(nr);

	connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
	connect(reply, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

	request = _url.toString();

	return reply;
}

QNetworkReply * AbstractNetworkService::sendRequest(const QString & url) {
	return sendRequest(QUrl(url));
}

void AbstractNetworkService::error(QNetworkReply::NetworkError) {
// 	qWarning("error retrieving data for request");
	qWarning(QString("error retrieving data for request %1").arg(request).toLocal8Bit());

	QNetworkReply * currentReply = qobject_cast<QNetworkReply *>(sender());

	currentReply->disconnect(this);
	currentReply->deleteLater();
	currentReply = 0;
}

void AbstractNetworkService::metaDataChanged() {
	QUrl redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (redirectionTarget.isValid()) {
		sendRequest(redirectionTarget);
	}
}

#include "AbstractNetworkService.moc"