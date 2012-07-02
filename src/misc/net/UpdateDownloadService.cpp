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

#include "UpdateDownloadService.h"

#include <QFile>
#include <qprocess.h>
#include <QMessageBox>
#include <QStringBuilder>

UpdateDownloadService::UpdateDownloadService(QNetworkAccessManager & netmanager, QObject * parent) : AbstractNetworkService(netmanager, parent) {

	downloaded_filename = "update.exe";
	download_progress.setLabelText(tr("Downloading..."));
	download_progress.setMaximum(0);

// 	download_progress.setMinimumDuration(0);
	download_progress.setWindowModality(Qt::WindowModal);
	download_progress.setCancelButton(NULL);
}

void UpdateDownloadService::sendRequest(const QUrl & url) {
	AbstractNetworkService::sendRequest(url);
}

void UpdateDownloadService::requestFinished() {
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (!(statusCode >= 200 && statusCode < 300)) {
		return;
	}

// 	QObject::disconnect(replyDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

	download_progress.setMaximum(0);
	download_progress.hide();

	QFile localFile(downloaded_filename);
	if (!localFile.open(QIODevice::WriteOnly))
		return;
	localFile.write(reply->readAll());
	localFile.close();

	QProcess myProcess;
	// Start the QProcess instance.
	myProcess.startDetached(downloaded_filename);
	exit(0);
}

void UpdateDownloadService::downloadProgress(qint64 received, qint64 total) {
	download_progress.setMaximum(total);
	download_progress.setValue(received);
// 	PR(received);
// 	PR(total);
}

#include "UpdateDownloadService.moc"
