/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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


#ifndef TABMEALWIDGET_H
#define TABMEALWIDGET_H

#include "ui_TabMealWidget.h"

#include "Database.h"
#include "DistributorTableModel.h"
#include "MealTableModel.h"

#include <QtSql>
#include <QCompleter>
#include <QDataWidgetMapper>

class TabMealWidget : public QWidget, public Ui::TabMealWidget {
Q_OBJECT
public:
	TabMealWidget(QWidget * parent = NULL);
	virtual ~TabMealWidget();

private slots:
	void activateUi(bool activate = true);
	void add_mealday();
	void toggle_calendar(bool show);
	void hightlight_day(const QDate & date);
	void selectDay(const QModelIndex& idx);
	void validateSpins();
	void doUpdate();
	void mealTabChanged(int tab);

	void doPrepareReport();
	void doPrepareReports();
	void doBrowseReports();

private:
	void checkForDirty();

private slots:

public:
	enum MealColumns { MId = 0, MDate, MType, MName, MPersons, MCosts };
	QString mealcode[4];

private:
	Database * db;
	DistributorTableModel * model_dist;
	QDataWidgetMapper * wmap;
	QString seldate;

	QAction * createPDF;
	QAction * createPDFAll;
	QAction * browsePDF;

	bool lock;
	int current_meal_row;
};

#endif // TABMEALWIDGET_H
