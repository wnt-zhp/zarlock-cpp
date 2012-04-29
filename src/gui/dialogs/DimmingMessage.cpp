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

#include "globals.h"

#include "DimmingMessage.h"
#include <QHBoxLayout>
#include <QLayout>

DimmingMessage::DimmingMessage(QWidget * parent) : AbstractDimmingWidget(new QWidget(parent), parent) {
	CII();
	QString stylesheet = "color:white; background-color:rgba(0,0,0,200); border: 1px solid rgba(0,0,0,200); border-radius:4px;";
	this->setStyleSheet(stylesheet);

	setLayout(new QHBoxLayout(this));

	icon = new QLabel;
	setIcon((QIcon()));
	layout()->addWidget(icon);

	// TODO: Implement this
	busy = new QProgressIndicator();
	busy->hide();
	busy->setColor(Qt::white);
	layout()->addWidget(busy);

	label = new QLabel(this);
	layout()->addWidget(label);

	setAnimationCurve(QEasingCurve::OutQuad);
	setDuration(500, 1000);
}

DimmingMessage::~DimmingMessage() {
	DII();
	delete label;
	delete busy;
	delete icon;
}



void DimmingMessage::setMessage(const QString& message) {
	if (message.isEmpty())
		label->hide();
	else {
		this->label->setText(message);
		this->label->setAlignment(Qt::AlignVCenter);
		this->adjustSize();
	}
}

void DimmingMessage::setIcon(const QIcon & icon) {
	if (!icon.isNull()) {
		this->icon->setPixmap(icon.pixmap(22, 22));
		this->icon->show();
	}
	else {
		this->icon->hide();
	}
	this->adjustSize();
}

void DimmingMessage::showBusy(bool show) {
	busy->setVisible(show);
	if (show)
		busy->startAnimation();
	else
		busy->stopAnimation();
}
