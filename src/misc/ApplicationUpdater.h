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


#ifndef APPLICATIONUPDATER_H
#define APPLICATIONUPDATER_H

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QString>
#include <QProgressDialog>

class ApplicationUpdater : public QObject {
Q_OBJECT
public:
	ApplicationUpdater(QObject * parent = NULL);
	virtual ~ApplicationUpdater();

public slots:
	void checkForUpdates();
	void downloadUpdates(const QString & download_url);

private slots:
	void finishedRequest(QNetworkReply*);
	void downloadProgress(qint64, qint64);

private:
	QNetworkAccessManager* nam;

	QNetworkReply * replyUpdateCheckRequest;
	QNetworkReply * replyDownloadRequest;

	QString downloaded_filename;

	QProgressDialog download_progress;
};

#endif // APPLICATIONUPDATER_H
