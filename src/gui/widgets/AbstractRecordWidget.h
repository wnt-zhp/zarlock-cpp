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


#ifndef ABSTRACTRECORDWIDGET_H
#define ABSTRACTRECORDWIDGET_H

#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QWidget>
#include <QPushButton>

class AbstractRecordWidget : public QWidget {
Q_OBJECT
public:
	AbstractRecordWidget(QWidget * parent = NULL);
	virtual ~AbstractRecordWidget();

signals:
	void closed(bool);

public slots:
	virtual void setInsertMode();
	virtual void setUpdateMode(const QModelIndex & idx);

protected slots:
	virtual void insertRecord() = 0;
	virtual void insertRecordAndExit();
	virtual void closeForm();
	virtual void clearForm();

private:
	virtual void prepareInsert() = 0;
	virtual void prepareUpdate() = 0;
	virtual void prepareWidget();

protected:
	int idToUpdate;
	QModelIndex idxToUpdate;

	enum WidgetMode { INSERT_MODE, UPDATE_MODE } widget_mode;
};

#endif // ABSTRACTRECORDWIDGET_H
