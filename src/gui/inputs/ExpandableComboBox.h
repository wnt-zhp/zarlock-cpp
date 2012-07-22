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


#ifndef EXPANDABLECOMBOBOX_H
#define EXPANDABLECOMBOBOX_H

#include <QComboBox>

class QLineEdit;
class EventFilter;

class ExpandableComboBox : public QComboBox {
Q_OBJECT
Q_PROPERTY(bool popupExpandable READ popupExpandable WRITE setPopupExpandable)
Q_PROPERTY(bool dynamicKeySearch READ dynamicKeySearch WRITE setDynamicKeySearch)
public:
	explicit ExpandableComboBox(QWidget* parent = 0);

	virtual void showPopup();

	inline bool dynamicKeySearch() const { return _dynamicKeySearch; }
	inline void setDynamicKeySearch(bool dynamicSearching) { _dynamicKeySearch = dynamicSearching; }

	inline bool popupExpandable() const { return _popupExpandable; }
	inline void setPopupExpandable(bool expandable) { _popupExpandable = expandable; }

	void setView(QAbstractItemView * itemView);

public slots:
	void setSearchString(const QString & str);

private slots:
	void popupEventCaptured(QEvent * evt);

signals:
	void popupAboutToBeShow();

protected:
// 	virtual void keyPressEvent(QKeyEvent* e);

private:
	bool _popupExpandable;
	bool _dynamicKeySearch;

	EventFilter * popupevf;
	QLineEdit * ledit;
};

#endif // EXPANDABLECOMBOBOX_H
