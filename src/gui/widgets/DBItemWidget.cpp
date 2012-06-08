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
// */


#include "DBItemWidget.h"
#include "globals.h"

#include <QStringBuilder>

DBItemWidget::DBItemWidget(QWidget* parent, Qt::WindowFlags f): QWidget(parent, f) {
	if (parent == NULL)
		setupUi(this);
	else
		setupUi(parent);
}

DBItemWidget::~DBItemWidget() {
	FPR(__func__);
}

void DBItemWidget::update(const CampProperties * camp) {
// 	QString name = camp->campName % " ( " % tr("commander") % QString(": ") %  % QString(")");
	QString period = camp->campDateBegin.toString(Qt::DefaultLocaleShortDate) % QString(" - ") % camp->campDateEnd.toString(Qt::DefaultLocaleShortDate);
	QString scouts = QString("%1 + %2 scouts").arg(camp->scoutsNo).arg(camp->leadersNo);

	this->label_info->setText("<b>" % camp->campName % "</b><br/>" % camp->campQuarter % "<br/>" % period % "<br/>" % scouts);

}

