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

#include <QFile>
#include <qprocess.h>
#include <QMessageBox>
#include <QStringBuilder>

#include "globals.h"
#include "config.h"
#include "version.h"

#include "NetworkServicesManager.h"
#include "UpdateCheckService.h"
#include "UpdateDownloadService.h"
#include "FeedService.h"

NetworkServicesManager::NetworkServicesManager(QObject * parent) : QObject(parent) {
	QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

NetworkServicesManager::~NetworkServicesManager()  {
	qDeleteAll(regservice);
	regservice.clear();
}

void NetworkServicesManager::checkForUpdates() {
	UpdateCheckService * ucs = new UpdateCheckService(nam);
	ucs->sendRequest();
	connect(ucs, SIGNAL(downloadUrlFound(QString)), this, SLOT(downloadUpdates(QString)));
	regservice.push_back(ucs);
// 	ucs->deleteLater();
}

void NetworkServicesManager::downloadUpdates(const QString & downloadurl) {
	UpdateDownloadService * uds = new UpdateDownloadService(nam);
	uds->sendRequest(downloadurl);
	regservice.push_back(uds);
}

void NetworkServicesManager::getFeed() {
	FeedService * feeds = new FeedService(nam);
	feeds->sendRequest(QUrl("http://zarlok.zhp.pl/feeds"));
	regservice.push_back(feeds);
}

void NetworkServicesManager::finished(QNetworkReply* reply) {
// 	// Reading attributes of the reply
// 	// e.g. the HTTP status code
// 	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
// 	// Or the target URL if it was a redirect:
// 	QVariant redirectionTargetUrl =
// 	reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
// 	// see CS001432 on how to handle this
// 
// 	// 	// no error received?
// 	// 	if (reply->error() != QNetworkReply::NoError) {
// 		// 			// handle errors here
// 	// 			return;
// 	// 	}
// 	reply->deleteLater();
	Q_UNUSED(reply);
	reply->deleteLater();
}

#include "NetworkServicesManager.moc"
