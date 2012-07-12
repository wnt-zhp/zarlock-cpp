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

#include "globals.h"
#include "DataParser.h"

#include "AbstractInputWidget.h"

#include <QStyle>

AbstractInputWidget::AbstractInputWidget(QWidget * parent) : QLineEdit(parent), edit_mode(false), is_ok(false), allow_empty(false) {
	defpal = this->palette();

	clearButton = new QToolButton(this);
	clearButton->setIcon(QIcon(":/resources/icons/edit-clear-locationbar-rtl.png"));
	clearButton->setCursor(Qt::ArrowCursor);
	clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	clearButton->hide();

	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
	QSize msz = minimumSizeHint();
	setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
				   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));

	connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
	connect(this, SIGNAL(textEdited(QString)), this, SLOT(doTextChanged(QString)));
	connect(this, SIGNAL(returnPressed()), this, SLOT(doReturnPressed()));
}

AbstractInputWidget::~AbstractInputWidget() {
	delete clearButton;
	FPR(__func__);
}

bool AbstractInputWidget::ok() const {
	return is_ok;
}

void AbstractInputWidget::focusOutEvent(QFocusEvent * ev) {
// 	if (edit_mode) {
// 		rawtext = QLineEdit::text();
// 	}
// 
// 	verify(rawtext);

	verify();
	this->setFont(globals::font_display);

	QLineEdit::setText(displaytext);

	edit_mode = false;
	if (ev != NULL)
		QLineEdit::focusOutEvent(ev);
}

void AbstractInputWidget::focusInEvent(QFocusEvent* ev) {
	if (!edit_mode) {
		QLineEdit::setFont(globals::font_edit);
		QLineEdit::setText(rawtext);
	}

	edit_mode = true;
	if (ev != NULL)
		QLineEdit::focusInEvent(ev);
}

bool AbstractInputWidget::verify() {
	if (edit_mode)
		rawtext = QLineEdit::text();

	return verify(rawtext);
}

bool AbstractInputWidget::verify(const QString & t) {
	updateClearButton(t);

	this->setPalette(defpal);

	if (t.isEmpty()) {
		is_ok = allow_empty;
		displaytext.clear();
	} else if (verifyText(t, displaytext)) {
		if (!t.isEmpty()) this->setPalette(globals::palette_ok);
		is_ok = true;
	} else {
		if (!t.isEmpty()) { this->setPalette(globals::palette_bad); displaytext = tr("ERROR: parse error"); }
		is_ok = false;
	}

	emit dataChanged();
	return is_ok;
}

void AbstractInputWidget::clear() {
	rawtext.clear();
	this->setPalette(defpal);
	is_ok = allow_empty;
	QLineEdit::clear();
}

void AbstractInputWidget::setText(const QString& t) {
	if (!edit_mode) {
		QLineEdit::setText(t);
	}
}

void AbstractInputWidget::setRaw(const QString& t) {
	rawtext = t;

	if (edit_mode) {
		QLineEdit::setText(t);
	} else {
// 		focusOutEvent(NULL);
		verify(t);
		QLineEdit::setText(displaytext);
	}
}

void AbstractInputWidget::doTextChanged(const QString& t) {
	if (!edit_mode) {
		QLineEdit::setFont(globals::font_edit);
		edit_mode = true;
		QLineEdit::setText(rawtext);
	} else
		rawtext = t;

	verify(rawtext);
}

void AbstractInputWidget::doReturnPressed() {
	focusOutEvent(NULL);
}

void AbstractInputWidget::doRefresh() {
	if (!edit_mode)
		focusOutEvent(NULL);
}

const QString AbstractInputWidget::text(bool placeholdertext) {
	return placeholdertext ? displaytext : rawtext;
}

void AbstractInputWidget::resizeEvent(QResizeEvent *) {
	QSize sz = clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	clearButton->move(rect().right() - frameWidth - sz.width(),
					  (rect().bottom() + 1 - sz.height())/2);
}

void AbstractInputWidget::updateClearButton(const QString& text) {
	clearButton->setVisible(!text.isEmpty());
}

void AbstractInputWidget::setEmptyAllowed(bool en) {
	is_ok = allow_empty = en;
}

bool AbstractInputWidget::emptyAllowed() {
	return allow_empty;
}


#include "AbstractInputWidget.moc"