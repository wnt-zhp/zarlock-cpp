/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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
	virtual void update_model();
	virtual void prepareInsert(bool visible);
	virtual void prepareUpdate(const QModelIndex & idx);

protected slots:
	virtual void insertRecord() = 0;
	virtual void insertRecordAndExit();
	virtual void closeForm();
	virtual void clearForm();

protected:
	int idToUpdate;
	QString button_label_insert_and_next;
	QString button_label_insert_and_exit;
	QString button_label_close;
	QString button_label_clear;
	QString button_label_update;
};

#endif // ABSTRACTRECORDWIDGET_H
