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

#include "UpdateCheckService.h"

#include <QFile>
#include <qprocess.h>
#include <QMessageBox>
#include <QStringBuilder>

UpdateCheckService::UpdateCheckService(QNetworkAccessManager& netmanager, QObject* parent): AbstractNetworkService(netmanager, parent) {
}

void UpdateCheckService::sendRequest() {
	sendRequest(QUrl("http://zarlok.zhp.pl/zarlok_update.php"));
}

void UpdateCheckService::sendRequest(const QUrl & url) {
	QUrl _url = url;
	_url.addQueryItem("version", ZARLOK_VERSION);
#if defined(__unix__)
	_url.addQueryItem("OS", "UNIX");
#elif defined(__WIN32)
	_url.addQueryItem("OS", "WIN32");
#endif

	AbstractNetworkService::sendRequest(_url);
}

void UpdateCheckService::requestFinished() {
	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (!(statusCode >= 200 && statusCode < 300)) {
		return;
	}

	QByteArray bytes = reply->readAll();

	QString reply_string = bytes; // string

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
// 			this->downloadUpdates(url_download_values[DURL] % "/" % url_download_values[DWIN]);
			emit downloadUrlFound(url_download_values[DURL] % "/" % url_download_values[DWIN]);
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

#include "UpdateCheckService.moc"
