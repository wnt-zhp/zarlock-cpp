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
#include <QHeaderView>

#include "globals.h"
#include "MealDaysTableView.h"
#include "MealDayTableModel.h"
#include "BatchTableModel.h"

#include "TableDelegates.h"



/**
 * @brief Standardowy konstruktor, żaden szał.
 *
 * @param parent niby parent, ale nie wiem po co służy, czasem się ustawia, czesem nie.
 * Obiecuję się doszkolić.
 * Poczytać o signal/slot w Qt
 **/
MealDaysTableView::MealDaysTableView(QWidget * parent) : QTableView(parent) {
	CI();
	// Popup menu dla akcji usuwania rekordu z bazy.
	removeRec = new QAction(tr("&Remove record"), this);
	removeRec->setShortcut(QKeySequence::Delete);
	removeRec->setToolTip(tr("Remove record from database"));
	//  Łączymy akcję kliknięcia w menu "Remove" z funkcją (slotem), która to wykona.
	connect(removeRec, SIGNAL(triggered()), this, SLOT(removeRecord()));
	pmenu_del.addAction(removeRec);

	// Popup menu dla akcji dodawania rekordu do bazy.
	addRec = new QAction(tr("&Add record"), this);
	addRec->setShortcut(QKeySequence::New);
	addRec->setToolTip(tr("Add record to database"));
	//  Łączymy akcję kliknięcia w menu "Add" z funkcją (slotem), która to wykona.
// 	connect(addRec, SIGNAL(triggered()), this, SLOT(addRecord()));
	pmenu_add.addAction(addRec);

	PriceDelegate * price_delegate = new PriceDelegate;

	setItemDelegateForColumn(BatchTableModel::HPrice, price_delegate);

	this->setEditTriggers(NoEditTriggers);
// 	this->setEditTriggers(DoubleClicked);
}

/**
 * @brief Destruktor. Usuwamy wszystko co stworzone operatorem new.
 *
 **/
MealDaysTableView::~MealDaysTableView() {
	DI();
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
void MealDaysTableView::setModel(QAbstractItemModel * model) {
	QTableView::setModel(model);

	hideColumn(MealDayTableModel::HId);
	horizontalHeader()->setResizeMode(MealDayTableModel::HMealDate, QHeaderView::Stretch);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

//	setSortingEnabled(true);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);

	PriceDelegate * price_delegate = new PriceDelegate;
	setItemDelegateForColumn(MealDayTableModel::HAvgCost, price_delegate);
	update();
}

/**
 * @brief Ta funkcja jest wywoływana kiedy wciśniemy RMB na naszym widoku.
 *
 * @param event typ zdarzenia
 * @return void
 **/
void MealDaysTableView::contextMenuEvent(QContextMenuEvent * event) {
	// Liczba zaznaczonych itemów. Ze względu na politykę znaznaczania - całe wiersze,
	// liczba itemów jest wielokrotnością liczby wyświetlanych kolumn.

	// Jeśli mamy itemy (akcja RBM na jakimś itemie lub zaznaczyliśmy itemy  ręcznie),
	// to znaczy, żę coś chcemy to np usunąć
	if (selectedIndexes().size())
		pmenu_del.exec(event->globalPos());
	// A jeśli nie mamy żadnych itemów to znaczy, że kliknęliśmy w wolnym polu tabeli
	// i chcemy coś dodać.
	else
		pmenu_add.exec(event->globalPos());

	// Przesyłamy event do dalszego przetwarzania
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
void MealDaysTableView::removeRecord() {
	QModelIndexList l = selectedIndexes();

	QVector<int> v;
	for (QModelIndexList::iterator it = l.begin(); it != l.end(); ++it) {
		if ((*it).column() == MealDayTableModel::HMealDate)
			v.push_back(it->model()->index(it->row(), MealDayTableModel::HId).data(Qt::EditRole).toInt());
	}
// 	PR(v.size());
	emit removeRecordRequested(v, true);
}

#include "MealDaysTableView.moc"