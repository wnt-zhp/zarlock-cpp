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


#ifndef MEALTABWIDGET_H
#define MEALTABWIDGET_H

#include <QTabWidget>
#include <QToolButton>
#include <QMessageBox>
#include <QMenu>
#include <QCheckBox>
#include <QListWidget>
#include <QDate>

class BatchTableModelProxy;
class MealTabInsertWidget;
class MealFoodList;
class MealManager;
class MealTabInsertWidget;
class BatchTableModelProxy;

class MealTabWidget : public QTabWidget {
Q_OBJECT
public:
	explicit MealTabWidget(QWidget* parent = 0);
	virtual ~MealTabWidget();

	virtual BatchTableModelProxy * getBatchProxyModel() const;

public slots:
	virtual void setMealDayId(int mdid);

	virtual void markOpenItem(QListWidgetItem* item);
	virtual void closeOpenItems();

	virtual void showFutureBatches(bool future);

protected slots:
	virtual void reloadTabs(int mealDayId);
	virtual void closeTab(int index);
	virtual void prepareTab(int mealDayId);

	virtual void customContextMenuEvent(const QPoint & point);

	virtual void invalidateProxy();

private:
	void prepareTab(int mealid, const QDate & mealday, int pos = 0);

private:
	BatchTableModelProxy * batch_proxy;

	QListWidgetItem * open_item;
	MealTabInsertWidget * mtiw;
	int meal_day_id;

	QMenu tab_context_menu;
	QAction * tab_action_rename;

	QVector<MealFoodList *> tab_handler;

	QDate current_selected_day;

	MealManager * mm;

	QToolButton * tb_tools;
	QAction * action_showfuture;
	QAction * action_hideexp;
	QAction * action_hideempty;
};

#endif // MEALTABWIDGET_H
