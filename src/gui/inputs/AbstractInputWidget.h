/*
    This file is part of Zarlok.

    Copyright (C) 2010  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#ifndef ABSTRACTINPUTWIDGET_H
#define ABSTRACTINPUTWIDGET_H

#include <QtGui/QLineEdit>
#include <QtGui/QToolButton>

class AbstractInputWidget : public QLineEdit {
Q_OBJECT
public:
	AbstractInputWidget(QWidget * parent);
	virtual ~AbstractInputWidget();

	virtual void focusOutEvent(QFocusEvent *);
	virtual void focusInEvent(QFocusEvent *);

	virtual void clear();

	virtual bool ok() const;

	virtual const QString text(bool placeholdertext = false);
	virtual void setText(const QString & t);
	virtual void setRaw(const QString & t);

	virtual void setEmptyAllowed(bool en = true);
	virtual bool emptyAllowed();

protected:
	void resizeEvent(QResizeEvent *);

protected:
	virtual bool verifyText(const QString &, QString & ) = 0;

public slots:
	void doTextChanged(const QString& t);
	void doReturnPressed();
	void doRefresh();

private slots:
	bool verify();
	bool verify(const QString &);
	void updateClearButton(const QString &text);

signals:
	void dataChanged();

protected:
	QString displaytext, rawtext;
	bool edit_mode, is_ok;
	QPalette defpal;
	bool allow_empty;

private:
	QToolButton * clearButton;
};

#endif // ABSTRACTINPUTWIDGET_H