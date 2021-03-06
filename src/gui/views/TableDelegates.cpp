/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

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

#include "TableDelegates.h"

#include "BatchTableModel.h"

#include <QStringBuilder>
#include <QPainter>
#include <QApplication>


void PriceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	opt.text = tr("%1 zl").arg(index.data(Qt::DisplayRole).toDouble(), 0, 'g', 10);

	const QWidget * widget/* = QStyledItemDelegatePrivate::widget(option)*/;
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		widget = v3->widget;
	else
		widget = NULL;

	QStyle *style = widget ? widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

void UnitDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);

	opt.text = tr("%1 %2").arg(index.data(Qt::DisplayRole).toDouble(), 0, 'g', 10).arg("");

	const QWidget * widget/* = QStyledItemDelegatePrivate::widget(option)*/;
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		widget = v3->widget;
	else
		widget = NULL;

	QStyle *style = widget ? widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

#include "TableDelegates.moc"