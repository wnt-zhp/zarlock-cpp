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

#include "DimmingWidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLayout>

#include "globals.h"

DimmingWidget::DimmingWidget(QWidget * parent) : AbstractDimmingWidget(new QWidget(parent), parent) {
	setLayout(new QHBoxLayout(this));

	widget = NULL;

	setAnimationCurve(QEasingCurve::OutQuad);
	setDuration(500, 1000);
}

DimmingWidget::~DimmingWidget() {
}

void DimmingWidget::setWidget(QWidget * widget) {
	if (!widget)
		this->widget->hide();
	else {
		layout()->removeWidget(this->widget);
		layout()->addWidget(widget);
		this->widget = widget;
		widget->setStyleSheet(".QWidget { color:white; background-color:rgba(0,0,0,200); border: 1px solid rgba(0,0,0,200); border-radius:4px; } .QWidget QLabel, .QWidget QGroupBox { color:white; }");
		adjustSize();
	}
}

void DimmingWidget::go() {
	widget->show();
	adjustSize();
	AbstractDimmingWidget::go();
}
