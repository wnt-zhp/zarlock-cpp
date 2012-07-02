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

#include "FeedService.h"
#include "AbstractNetworkService.h"

#include <QFile>
#include <qprocess.h>
#include <QMessageBox>
#include <QStringBuilder>

FeedService::FeedService(QNetworkAccessManager& netmanager, QObject* parent): AbstractNetworkService(netmanager, parent) {
}


FeedService::~FeedService()  {
}

void FeedService::sendRequest() {
	sendRequest(QUrl("http://zarlok.zhp.pl/zarlok_update.php"));
}

void FeedService::sendRequest(const QUrl & url) {
	QUrl updateurl("http://zarlok.zhp.pl/zarlok_update.php");
	if (!url.isEmpty()) {
		updateurl = url;
	}

	updateurl.addQueryItem("version", ZARLOK_VERSION);
#if defined(__unix__)
	updateurl.addQueryItem("OS", "UNIX");
#elif defined(__WIN32)
	updateurl.addQueryItem("OS", "WIN32");
#endif

	AbstractNetworkService::sendRequest(url);

}

void FeedService::requestFinished() {
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (!(statusCode >= 200 && statusCode < 300)) {
		return;
	}

}

#include "FeedService.moc"
