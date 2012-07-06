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


#ifndef DBEXPORTCOMMON_H
#define DBEXPORTCOMMON_H

#include <QByteArray>
#include <QString>

namespace DBExportCommon {
	struct DBBuffer {
		char has_info;
		QByteArray dbname, infoname;
		QByteArray dbdata, infodata;
	};

	void convertToCharArray(int number, char * array, int size);
	void convertFromCharArray(int & number, char * array, int size);

	void Compress(const QString & dbfilestr, const QString & infofilestr, DBBuffer * buff);
	void Uncompress(const QString & archFile, DBBuffer * buff);

	void ExportFile(const QString & archFile, DBBuffer * buff);
	void ImportFile(const QString & dbFile, const QString & infoFile, DBBuffer * buff);

	extern const QString exportArchName;
	extern const QString exportArchExt;

	const char exportIntSize = 4;
};

#endif // DBEXPORTCOMMON_H
