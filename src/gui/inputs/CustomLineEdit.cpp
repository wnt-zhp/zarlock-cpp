/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2010  Rafał Lalik <rafal.lalik@ph.tum.de>

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
#include "DataParser.h"

#include "CustomLineEdit.h"

#include <QStyle>

CustomLineEdit::CustomLineEdit(QWidget * parent) : QLineEdit(parent), edit_mode(false), is_ok(false) {
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(verify(QString)));
	connect(this, SIGNAL(returnPressed()), this, SLOT(doReturnPressed()));

	defpal = this->palette();

	clearButton = new QToolButton(this);
	clearButton->setIcon(QIcon(":/resources/icons/edit-clear-locationbar-rtl.png"));
// 	clearButton->setIconSize(pixmap.size());
	clearButton->setCursor(Qt::ArrowCursor);
	clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	clearButton->hide();
	connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
// 	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(updateClearButton(const QString&)));
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearButton->sizeHint().width() + frameWidth + 1));
	QSize msz = minimumSizeHint();
	setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + frameWidth * 2 + 2),
				   qMax(msz.height(), clearButton->sizeHint().height() + frameWidth * 2 + 2));
}

CustomLineEdit::~CustomLineEdit() {
	delete clearButton;
	FPR(__func__);
}

bool CustomLineEdit::ok() {
	return is_ok;
}

void CustomLineEdit::focusOutEvent(QFocusEvent * ev) {
	if (edit_mode) {
		rawtext = QLineEdit::text();
	} else {
		edit_mode = true;
		verify(rawtext);
	}

	this->setFont(globals::font_display);

	if (rawtext.isEmpty()) {
		displaytext.clear();
	} else if (!is_ok) {
		displaytext = tr("Parser error!");
	}
// 	if (ev != NULL)
// 		edit_mode = false;

	QLineEdit::setText(displaytext);

// 	if (ev != NULL)
		edit_mode = false;
    if (ev != NULL)
		QLineEdit::focusOutEvent(ev);
}

void CustomLineEdit::focusInEvent(QFocusEvent* ev) {
	// bez tego if'a kiedy znika popup completera to wywoływany
	// jest ponowny focusInEvent i psuje zawartość inputa
	if (!edit_mode) {
		QLineEdit::setFont(globals::font_edit);
		QLineEdit::setText(rawtext);
	}
	if (ev != NULL)
		QLineEdit::focusInEvent(ev);
	edit_mode = true;
}

bool CustomLineEdit::verify(const QString & t) {
	updateClearButton(t);

	if (!edit_mode) {
		QLineEdit::setText(rawtext);
		edit_mode = true;
		verify(rawtext);
	} else {
		if (t.isEmpty())
			this->setPalette(defpal);

		if (verifyText(t, displaytext)) {
			if (!t.isEmpty()) this->setPalette(globals::palette_ok);
			is_ok = true;
		} else {
			if (!t.isEmpty()) this->setPalette(globals::palette_bad);
			is_ok = false;
		}
	}

	emit dataChanged();

	return is_ok;
}

void CustomLineEdit::clear() {
	rawtext.clear();
	this->setPalette(defpal);
	is_ok = false;
	QLineEdit::clear();
}

void CustomLineEdit::setText(const QString& t) {
//	edit_mode = true;
//	rawtext = t;
//	QLineEdit::setText(t);
//	emit textChanged(t);
//	focusOutEvent(NULL);

//	rawtext = t;
	edit_mode = true;
// 	emit textChanged(t);
	QLineEdit::setText(t);
//	verify(t);
// 	edit_mode = false;
	focusOutEvent(NULL);
}

void CustomLineEdit::doReturnPressed() {
// 	edit_mode = true;
// 	verify(text());
// 	edit_mode = false;
	focusOutEvent(NULL);
}

void CustomLineEdit::doRefresh() {
	focusInEvent(NULL);
	focusOutEvent(NULL);
}

const QString CustomLineEdit::text(bool placeholdertext) {
	return placeholdertext ? displaytext : rawtext;
}

// TODO Ponoć każdy dobry kod zawiera przynajmniej jedno fuck w komentarzach.
// TODO Poniższy fuck, spolszczony oczywiście, jest niezbitym świadectwem wysokiej klasy tego kodu.
// TODO "Ja już tu k***a sam nie wiem co napisałem w tej klasie i jak to działa :) :( :/ :\"
// TODO Ja tu jeszcze wrócę!

void CustomLineEdit::resizeEvent(QResizeEvent *) {
	QSize sz = clearButton->sizeHint();
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
	clearButton->move(rect().right() - frameWidth - sz.width(),
					  (rect().bottom() + 1 - sz.height())/2);
}

void CustomLineEdit::updateClearButton(const QString& text) {
	clearButton->setVisible(!text.isEmpty());
}

#include "CustomLineEdit.moc"