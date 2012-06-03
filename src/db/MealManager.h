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


#ifndef MEALMANAGER_H
#define MEALMANAGER_H

#include <QObject>
#include "globals.h"
#include "Database.h"

class MealManager : public QObject {
Q_OBJECT
public:
	static MealManager * Instance();
	static void Destroy();
	virtual ~MealManager();

	enum MealKind {MBreakfast = 0, MBrunch, MLunch, MTea,  MDiner, MOther };

	int insertMeal(int mealdayid, MealKind mk);
	int insertMeal(int mealdayid, MealKind mk, const QString & name) throw (int);

	virtual bool addMealDayRecord(const QDate & mealday, int avgcost);
	virtual bool updateMealDayRecord(const int row, const QDate & mealday, int avgcost);
	virtual bool getMealDayRecord(const int row, int & mdid, QDate & mealday, int & avgcost);
	
	virtual bool addMealRecord(int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes);
	virtual bool updateMealRecord(int mid, int mealday, int mealkind, const QString & name, int scouts, int leaders, int others, int avgcosts, const QString & notes);
	virtual bool removeMealRecord(const QVector<int> & ids);

	virtual void selectMealDay();
	virtual void selectMeal();

	virtual QVariant getMealDayId(const QDate & date);
	virtual QVariant getMealDayDate(int id);

	virtual void mealDayDefauls(int mealDayId);

public slots:
	void reinit();
	virtual bool removeMealDayRecord(QVector<int> & ids, bool askForConfirmation = true);

signals:
	void mealInserted(int);

private:
	MealManager();
	MealManager(const MealManager &);

public:
	QVector<QString> defMealsNames;

private:
	static MealManager * manager;
	Database * db;

	MealDayTableModel * model_mealday;
	MealTableModel * model_meal;
};

#endif // MEALMANAGER_H
