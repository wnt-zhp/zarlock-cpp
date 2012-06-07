/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Rafał Lalik <rafal.lalik@ph.tum.de>

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

#include "globals.h"
#include "config.h"
#include "version.h"

#include "ApplicationUpdater.h"

#include <QFile>
#include <qprocess.h>
#include <QMessageBox>
#include <QStringBuilder>

ApplicationUpdater::ApplicationUpdater(QObject * parent) : QObject(parent), replyUpdateCheckRequest(NULL), replyDownloadRequest(NULL) {
	nam = new QNetworkAccessManager(this);

	downloaded_filename = "update.exe";
	download_progress.setLabelText(tr("Downloading..."));
	download_progress.setMaximum(0);

// 	download_progress.setMinimumDuration(0);
	download_progress.setWindowModality(Qt::WindowModal);
	download_progress.setCancelButton(NULL);

	QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedRequest(QNetworkReply*)));
// 	QObject::connect(replyDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

// 	QMessageBox mbox;
// 	mbox.setWindowTitle(tr("Updates available"));
// 	mbox.setTextFormat(Qt::RichText);
// 	mbox.setText(tr("Updates of Zarlok is available.<br><br>You can download it for from here:<br>"
// 					 "<a href='http://"  "www.test.com"  "/"  "file.exe"  "'>" 
// 					 "http://www.test.com"  "/"  "file.exe"  "<br>" 
// 					 "<a href='http://"  "www.test.com"  "/"  "file.exe"  "'>" 
// 					 "http://www.test.com"  "/"  "file.exe"  "<br>" 
// 					 "<a href='http://"  "www.test.com"  "/"  "file.exe"  "'>" 
// 					 "http://www.test.com"  "/"  "file.exe"  ""));
// 	
// 	mbox.exec();
}

ApplicationUpdater::~ApplicationUpdater()  {
}

void ApplicationUpdater::checkForUpdates() {
	QUrl url("http://zarlok.zhp.pl/zarlok_update.php");
	url.addQueryItem("version", ZARLOK_VERSION);
// 	PR(url.toString().toStdString());
	replyUpdateCheckRequest = nam->get(QNetworkRequest(url));
	// NOTE: Store QNetworkReply pointer (maybe into caller). 
	// When this HTTP request is finished you will receive this same 
	// QNetworkReply as response parameter. 
	// By the QNetworkReply pointer you can identify request and response.
}

void ApplicationUpdater::downloadUpdates(const QString & download_url) {
	QUrl url(download_url);

	replyDownloadRequest = nam->get(QNetworkRequest(url));
	QObject::connect(replyDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

	// NOTE: Store QNetworkReply pointer (maybe into caller). 
	// When this HTTP request is finished you will receive this same 
	// QNetworkReply as response parameter. 
	// By the QNetworkReply pointer you can identify request and response.
}

void ApplicationUpdater::finishedRequest(QNetworkReply* reply) {
	// Reading attributes of the reply
	// e.g. the HTTP status code
	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	// Or the target URL if it was a redirect:
	QVariant redirectionTargetUrl =
	reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	// see CS001432 on how to handle this

	QString reply_string;

	// no error received?
	if (reply->error() != QNetworkReply::NoError) {
			// handle errors here
	}

	if (reply == replyUpdateCheckRequest) {
		// read data from QNetworkReply here

		// Example 1: Creating QImage from the reply
// 		QImageReader imageReader(reply);
// 		QImage pic = imageReader.read();

		// Example 2: Reading bytes form the reply
		QByteArray bytes = reply->readAll();  // bytes
		reply_string = bytes; // string

		PR(reply_string.toStdString());

		if (reply_string != "no update") {

			QStringList dwvalues = reply_string.split('|', QString::KeepEmptyParts);
			for (int i = 0; i < dwvalues.size(); ++i)
				url_download_values[i] = dwvalues[i];

#ifdef _WIN32
			QMessageBox mbox(QMessageBox::Information, tr("Updates available"),
			tr("Updates of Zarlok is available.\n\n"
				"To finish upgrade process this zarlok session must be closed (this will be handled automaticly).\n\n"
				"Do you want download and install updates?"),
			   QMessageBox::Yes | QMessageBox::No);
			
			int ret = mbox.exec();
			
			if (ret == QMessageBox::Yes) {
				this->downloadUpdates(url_download_values[DURL] % "/" % url_download_values[DWIN]);
				
			}
#endif /* _WIN32 */
#ifdef __unix__
			QString msgtext = tr(
				"Updates of Zarlok is available.<br /><br />You can download it for from here:<br />"
				"<a href='%1/%2'>%1/%2</a><br />"
				"<a href='%1/%2'>%1/%3</a><br />"
				"<a href='%1/%4'>%1/%4</a><br />")
				.arg(url_download_values[DURL])
				.arg(url_download_values[DDEB])
				.arg(url_download_values[DRPM])
				.arg(url_download_values[DTBZ2]);

			QMessageBox mbox(QMessageBox::Information, tr("Updates available"), msgtext, QMessageBox::Ok);
			mbox.setTextFormat(Qt::RichText);

			int ret = mbox.exec();
#endif /* __unix__ */
		}
	}
	else
	if (reply == replyDownloadRequest) {
		QObject::disconnect(replyDownloadRequest, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));

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
	// Some http error received

	// We receive ownership of the reply object
	// and therefore need to handle deletion.
	reply->deleteLater();
// 	delete reply;
}

void ApplicationUpdater::downloadProgress(qint64 received, qint64 total) {
	download_progress.setMaximum(total);
	download_progress.setValue(received);
// 	PR(received);
// 	PR(total);
}


#include "ApplicationUpdater.moc"
