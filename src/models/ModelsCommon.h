/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#ifndef MODELSCOMMON_H
#define MODELSCOMMON_H

#include <QObject>

class ModelsCommon {
public:
	bool productRemoveConfirmation(const QString & prod);
	bool productRemoveConfirmation(const QString & prod, const QString & details);

	bool batchRemoveConfirmation(const QString & batch);
	bool batchRemoveConfirmation(const QString & batch, const QString & details);

	bool distributeRemoveConfirmation(const QString & batch);
	bool distributeRemoveConfirmation(const QString & batch, const QString & details);

protected:
	void inputErrorMsgBox(const QString & val);
};

#endif // MODELSCOMMON_H
