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

#include "globals.h"
#include "ProductsTableView.h"

#include <QHeaderView>

/**
 * @brief Standardowy konstruktor, żaden szał.
 *
 * @param parent niby parent, ale nie wiem po co służy, czasem się ustawia, czesem nie.
 * Obiecuję się doszkolić.
 * Poczytać o signal/slot w Qt
 **/
ProductsTableView::ProductsTableView(QWidget * parent) : QTableView(parent), db(Database::Instance()) {
	removeRec = new QAction(tr("&Remove record"), this);
	removeRec->setShortcut(QKeySequence::Delete);
	removeRec->setToolTip(tr("Remove record from database"));
	connect(removeRec, SIGNAL(triggered()), this, SLOT(removeRecord()));
	pmenu_del.addAction(removeRec);

	addRec = new QAction(tr("&Add record"), this);
	addRec->setShortcut(QKeySequence::New);
	addRec->setToolTip(tr("Add record to database"));
	connect(addRec, SIGNAL(triggered()), this, SLOT(addRecord()));
	pmenu_add.addAction(addRec);

	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(filterRecords(QModelIndex)));
	this->setEditTriggers(0);
}

/**
 * @brief Destruktor. Usuwamy wszystko co stworzone operatorem new.
 *
 **/
ProductsTableView::~ProductsTableView() {
	delete removeRec;
	delete addRec;
}

/**
 * @brief Ta funkcja odpowiada za ustawianie modelu dla widoku (poczytać w qtassistanto o model/view in Qt).
 * Najpierw wywołujemy standarowy QTableView::setModel() a następnie wprowadzamy do naszego widoku małe zmiany.
 * Robimy je tutaja nie w konstruktorze, gdyż konstruktor nie posiada jeszcze żadnych informacji o modelu,
 * więc np.nie możemy ukryć kolumn
 *
 * @param model model z danycmi do wyświetlania
 * @return void
 **/
void ProductsTableView::setModel(QAbstractItemModel * model) {
    QTableView::setModel(model);

	hideColumn(ProductsTableModel::HId);

// 	horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	horizontalHeader()->setStretchLastSection(true);
// 	horizontalHeader()->setMinimumSectionSize(0);
	horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void ProductsTableView::filterRecords(const QModelIndex &) {
	QModelIndexList l = selectedIndexes();
// 	PR(l.size());
	QString filter, q;
	for (QModelIndexList::iterator it = l.begin(); it != l.end(); it++) {
		if (it->column() == ProductsTableModel::HName) {
			q.sprintf("prod_id=%d", model()->data(model()->index(it->row(), 0)).toInt());

			if (it != l.begin())
				filter.append(" OR ");
			else
				filter.append(" WHERE ");
			filter.append(q);
		}
	}
	PR(filter.toStdString());
	emit recordsFilter(filter);
}

/**
 * @brief Ta funkcja jest wywoływana kiedy wciśniemy RMB na naszym widoku.
 *
 * @param event typ zdarzenia
 * @return void
 **/
void ProductsTableView::contextMenuEvent(QContextMenuEvent * event) {
	if (selectedIndexes().size())
		pmenu_del.exec(event->globalPos());
	else
		pmenu_add.exec(event->globalPos());
	QAbstractScrollArea::contextMenuEvent(event);
}

/**
 * @brief Usuwanie rekordów z bazy. Jako, że ze względu na politykę wybierania itemów
 * (wiersze a nie pojedyncze), żeby nie usuwać tych samych wierszy, filtrujemy wszystko
 * przez jedną wybraną kolumnę, np BatchTableModel::HSpec.
 * Ze względu na politykę zaywierdzania danych QSqlTableModel::onManualSubmit, dane nie
 * znikają od razu z modelu (więc nie mamy problemu z indeksami) ale dopiero kiedy
 * wywołane zostanie submitAll()
 *
 * @return void
 **/
void ProductsTableView::removeRecord() {
	QModelIndexList l = selectedIndexes();
	for (QModelIndexList::iterator it = l.begin(); it != l.end(); it++) {
// 		PR((*it).column());
// 		PR((*it).row());
		if ((*it).column() == ProductsTableModel::HName)
			model()->removeRow((*it).row());
	}
	db.CachedProducts()->submitAll();
}

#include "ProductsTableView.moc"