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

#include "DimmingWidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLayout>

#include "globals.h"

DimmingWidget::DimmingWidget(QWidget * parent) : AbstractDimmingWidget(parent), overlay_widget(new QWidget(parent)), widget(NULL), modal_mode(false) {
	setLayout(new QHBoxLayout(this));
	setOverlayWidget(overlay_widget);

	widget = NULL;

	setAnimationCurve(QEasingCurve::OutQuad);
	setDuration(500, 1000);
}

DimmingWidget::~DimmingWidget() {
// 	delete overlay_widget;
	widget->setStyleSheet("");
}

void DimmingWidget::setWidget(QWidget * widget) {
	if (!widget)
		this->widget->hide();
	else {
		layout()->removeWidget(this->widget);
		layout()->addWidget(widget);
		if (this->widget)
			this->widget->setStyleSheet(widget_stylesheet);

		widget_stylesheet = widget->styleSheet();
		this->widget = widget;
		widget->setStyleSheet(".QWidget { color:white; background-color:rgba(0,0,0,200); border: 1px solid rgba(0,0,0,200); border-radius:4px; } QWidget QLabel, .QWidget QGroupBox, QWidget QCheckBox { color:white; }");
		adjustSize();

		connect(this, SIGNAL(widgetOut()), widget, SLOT(hide()));
	}
}

void DimmingWidget::showWidget(bool modal) {
	modal_mode = modal;
	if (modal) {
		widget->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget /*| Qt::Tool*/);
		widget->setWindowModality(Qt::WindowModal);
	}
	widget->show();
	AbstractDimmingWidget::showWidget();
}

void DimmingWidget::hideWidget() {
	if (modal_mode) {
		widget->setWindowModality(Qt::NonModal);
		widget->setWindowFlags(Qt::Widget);
	}
// 	widget->hide();
	AbstractDimmingWidget::hideWidget();
}
