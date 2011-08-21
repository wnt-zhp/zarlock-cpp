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

#ifndef DISTRIBUTORTABLEMODEL_H
#define DISTRIBUTORTABLEMODEL_H

#include <QtSql/QSqlTableModel>
#include "ModelsCommon.h"

/**
 * @brief Klasa dziedziczy po QSqlTableModel i odpowiada za
 * przechowywanie danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym modelu  danych tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class DistributorTableModel : public QSqlTableModel, public ModelsCommon {
Q_OBJECT
public:
	DistributorTableModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase());
// 	virtual ~DistributorTableModel();

	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	virtual bool select();
	virtual void autoSubmit(bool asub = true);

	virtual int idRow(int id);

public slots:
	void filterDB(const QString &);
	void trigDataChanged(QModelIndex topleft , QModelIndex bottomright);

protected:
	QVariant display(const QModelIndex & idx, const int role = Qt::DisplayRole) const;
	QVariant raw(const QModelIndex & idx) const;

	bool autosubmit;

private:
	QVector<int> idmap;

public:
	enum Headers {HId = 0, HBatchId, HQty, HDistDate, HRegDate, HReason, HReason2, HReason3 };
	enum Reasons {RGeneral = 0, RExpired, RMeal };
	enum Meal {MBreakfast = 0, M2Breakfast, MLunch, MDiner, MOther1, MOther2 };

};

#endif // DISTRIBUTORTABLEMODEL_H
