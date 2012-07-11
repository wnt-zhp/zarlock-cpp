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

#include "TableDelegates.h"

#include "BatchTableModel.h"

#include <QStringBuilder>
#include <QPainter>
#include <QApplication>

#include "globals.h"

void PriceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);
	opt.text = tr("%1 zl").arg(index.data(Qt::DisplayRole).toDouble(), 0, 'f', 2);

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

void QtyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
	QStyleOptionViewItemV4 opt = option;
	initStyleOption(&opt, index);

	opt.text = tr("%1").arg(index.data(Qt::DisplayRole).toDouble()/100,  0, 'f', 2);

	const QWidget * widget/* = QStyledItemDelegatePrivate::widget(option)*/;
	if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>(&option))
		widget = v3->widget;
	else
		widget = NULL;

	QStyle *style = widget ? widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

void QtyOfAllDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
// 	int progress = index.data().toInt();

	QString str = index.data(Qt::DisplayRole).toString().trimmed();
	QString qtystr = str.left(str.indexOf(' '));
	QString allstr = str.right(str.length()-str.lastIndexOf(' ', -2));

	QStyleOptionProgressBar progressBarOption;
	progressBarOption.rect = option.rect;
	progressBarOption.minimum = 0;
	progressBarOption.maximum = allstr.toDouble()*100;
	progressBarOption.progress = qtystr.toDouble()*100;
	progressBarOption.text = str;
	progressBarOption.textVisible = true;

	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
}

#include "TableDelegates.moc"