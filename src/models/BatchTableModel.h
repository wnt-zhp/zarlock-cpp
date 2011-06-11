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

#ifndef BATCHTABLEMODEL_H
#define BATCHTABLEMODEL_H

#include <QtSql/QSqlRelationalTableModel>

/**
 * @brief Klasa dziedziczy po QSqlRelationalTableModel i odpowiada za
 * przechowywanie danych z tabeli 'batch'. Ta klasa jest przykładem, jak można
 * w standardowym modelu  danych tabeli dostosować kilka rzeczy do naszych potrzeb.
 * Wyjaśnienie znajduje się przy opisach funkcji.
 **/
class BatchTableModel : public QSqlRelationalTableModel {
Q_OBJECT
public:
	BatchTableModel(QObject * parent = NULL, QSqlDatabase db = QSqlDatabase());
	virtual ~BatchTableModel();

	virtual QVariant data(const QModelIndex& idx, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    virtual bool select();

	enum Headers {HId = 0, HProdId, HSpec, HPrice, HUnit, HStaQty, HBook, HExpire, HUsedQty, HRegDate, HDesc, HInvoice };

public slots:
	void filterDB(const QString &);

private:
	QVariant display(const QModelIndex & idx, const int role = Qt::DisplayRole) const;
	QVariant raw(const QModelIndex & idx) const;
};

#endif // BATCHTABLEMODEL_H
