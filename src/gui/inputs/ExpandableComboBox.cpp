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

#include <QAbstractItemView>
#include <QScrollBar>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QLineEdit>

#include "globals.h"
#include "ExpandableComboBox.h"
#include "BatchTableModel.h"
#include "EventFilter.h"

ExpandableComboBox::ExpandableComboBox(QWidget* parent): QComboBox(parent) {
	popupevf = new EventFilter;
	popupevf->registerFilter(QEvent::KeyPress);
	popupevf->registerFilter(QEvent::Show);
	popupevf->registerFilter(QEvent::Hide);
	connect(popupevf, SIGNAL(eventFiltered(QEvent*)), this, SLOT(popupEventCaptured(QEvent*)));

	ledit = new QLineEdit(this);
	ledit->setVisible(false);
	connect(ledit, SIGNAL(textChanged(QString)), this, SLOT(setSearchString(QString)));

	ledit->resize(this->sizeHint());
}

void ExpandableComboBox::showPopup() {
	emit popupAboutToBeShow();

	QComboBox::showPopup();

	if (!_popupExpandable)
		return;

	QFrame * popup = this->findChild<QFrame*>();

// 	qDebug() << view()->baseSize();
// 	qDebug() << view()->size();
// 	qDebug() << view()->frameSize();
// 	qDebug() << view()->sizeHint();

// 	int fw = popup->frameWidth();
// 	QRect geo = popup->normalGeometry();

#ifndef Q_WS_S60
	int screenid = QApplication::desktop()->screenNumber(this);
	QRect screen = QApplication::desktop()->screenGeometry(screenid);
#else
	QRect screen = qt_TRect2QRect(static_cast<CEikAppUi*>(S60->appUi())->ClientRect());
#endif

	int new_width = 1000;//geo.width() + 200;
	if (new_width > screen.width())
		new_width = screen.width();

	popup->resize(new_width, popup->rect().height());

	QPoint p = popup->pos();
	int dxpos = screen.right() - (p.x() + new_width);
	if (dxpos < 0) {
		int new_xpos = p.x() + dxpos;

		if (new_xpos < 0)
			new_xpos = 0;

		popup->move(p.x()+dxpos, p.y());
	}
}

void ExpandableComboBox::setSearchString(const QString& str) {
	QSortFilterProxyModel * proxy = dynamic_cast<QSortFilterProxyModel *>(model());
	if (!proxy)
		return;

	QString f = str;
	f.replace(' ', '*');

	//	proxy_model->setFilterRegExp(QRegExp(f, Qt::CaseInsensitive, QRegExp::Wildcard));
	proxy->setFilterWildcard(f);
	proxy->setFilterKeyColumn(BatchTableModel::HSpec);
	proxy->setFilterRole(Qt::UserRole);
	proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

// void ExpandableComboBox::keyPressEvent(QKeyEvent* e) {
// 	QString filter_string;// = ledit->text();
// 
// 	int key = e->key();
// PR(e);
// 	switch (key) {
// 		case Qt::Key_Escape:
// 			filter_string.clear();
// 			break;
// 		case Qt::Key_Enter:
// 			break;
// 		case Qt::Key_Backspace:
// 			filter_string.remove(filter_string.size()-1,1);
// 			break;
// 		default:
// 			if (key < 0xffff) {
// 				filter_string.append(e->text());
// 			} else {
// 				QComboBox::keyPressEvent(e);
// 			}
// 	}
// 
// 	setSearchString(filter_string);
// }

void ExpandableComboBox::popupEventCaptured(QEvent * evt) {
	QString filter_string = ledit->text();

	if (evt->type() == QEvent::KeyPress) {
		QKeyEvent * kevt = dynamic_cast<QKeyEvent *>(evt);
		int key = kevt->key();
	
		switch (key) {
			case Qt::Key_Escape:
				filter_string.clear();
				break;
			case Qt::Key_Enter:
				break;
			case Qt::Key_Backspace:
				filter_string.remove(filter_string.size()-1,1);
				break;
			default:
				if (key < 0xffff) {
					filter_string.append(kevt->text());
				}
		}
	
		if (filter_string.size() > 0) {
			ledit->show();
		}
		ledit->setText(filter_string);
		// 		this->setFilterString(filter_string);
	} else
		if (evt->type() == QEvent::Show) {
			// 		ledit->show();
			// 		this->setFilterString(QString());
		}
	if (evt->type() == QEvent::Hide) {
		ledit->hide();
		// 			this->setFilterString(QString());
		ledit->clear();
	}
}

void ExpandableComboBox::setView(QAbstractItemView* itemView) {
	view()->removeEventFilter(popupevf);

	QComboBox::setView(itemView);
	itemView->installEventFilter(popupevf);
}

#include "ExpandableComboBox.moc"