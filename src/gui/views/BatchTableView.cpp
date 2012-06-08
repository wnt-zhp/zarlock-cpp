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

#include "globals.h"
#include "BatchTableView.h"
#include "BatchTableModelProxy.h"

#include "TableDelegates.h"

#include <QHeaderView>
#include <QMessageBox>

/**
 * @brief Standardowy konstruktor, żaden szał.
 *
 * @param parent niby parent, ale nie wiem po co służy, czasem się ustawia, czesem nie.
 * Obiecuję się doszkolić.
 * Poczytać o signal/slot w Qt
 **/
BatchTableView::BatchTableView(QWidget * parent) : AbstractTableView(parent) {
	CI();

	PriceDelegate * price_delegate = new PriceDelegate;
	setItemDelegateForColumn(BatchTableModel::HPrice, price_delegate);

	QtyOfAllDelegate * qty_delegate = new QtyOfAllDelegate;
	setItemDelegateForColumn(BatchTableModel::HStaQty, qty_delegate);

	reloadPalette();
}

/**
 * @brief Destruktor. Usuwamy wszystko co stworzone operatorem new.
 *
 **/
BatchTableView::~BatchTableView() {
	DI();
}

/**
 * @brief Ta funkcja odpowiada za ustawianie modelu dla widoku (poczytać w qtassistanto o model/view in Qt).
 * Najpierw wywołujemy standarowy QTableView::setModel() a następnie wprowadzamy do naszego widoku małe zmiany.
 * Robimy je tutaja nie w konstruktorze, gdyż konstruktor nie posiada jeszcze żadnych informacji o modelu,
 * więc np.nie możemy ukryć kolumn
 *
 * @param model model z danymi do wyświetlania
 * @return void
 **/
void BatchTableView::setModel(QAbstractItemModel * model) {
	QTableView::setModel(model);

// 	hideColumn(BatchTableModel::HId);
	hideColumn(BatchTableModel::HProdId);
	hideColumn(BatchTableModel::HEntryDate);
	hideColumn(BatchTableModel::HNotes);
	hideColumn(BatchTableModel::HUsedQty);

	horizontalHeader()->setResizeMode(BatchTableModel::HId, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HProdId, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HSpec, QHeaderView::Stretch);
	horizontalHeader()->setResizeMode(BatchTableModel::HPrice, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HUnit, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HStaQty, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HEntryDate, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HRegDate, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HExpiryDate, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HInvoice, QHeaderView::ResizeToContents);
	horizontalHeader()->setResizeMode(BatchTableModel::HNotes, QHeaderView::ResizeToContents);

	sortByColumn(BatchTableModel::HId, Qt::AscendingOrder);
	setSortingEnabled(true);
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
void BatchTableView::removeRecord() {
	QModelIndexList l = selectedIndexes();

	int yestoall = false;

	QMessageBox mbox;
	mbox.setIcon(QMessageBox::Question);
	mbox.setWindowTitle(tr("Batch remove"));
	mbox.setInformativeText(tr("This batch contains distributed parties. You must first remove distributions."));
	mbox.setStandardButtons(QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::No);
	mbox.setDetailedText(tr("This batch will not be attached to removal list. Do you want continue?"));

	QVector<int> v;
	for (QModelIndexList::iterator it = l.begin(); it != l.end(); ++it) {
		if ( (*it).column() == BatchTableModel::HId ) {
			QVariant bid = (*it).data(Qt::EditRole);
			int dist = Database::Instance()->CachedDistributor()->find(DistributorTableModel::HBatchId, bid, Qt::EditRole, 0).size();
			if (dist > 0) {
				if (!yestoall) {
					mbox.setText(tr("Batch: %1, Distributions: %2").arg(model()->index((*it).row(), BatchTableModel::HSpec).data(Qt::DisplayRole).toString()).arg(dist));
					int res = mbox.exec();

					switch (res) {
						case QMessageBox::YesToAll:
							yestoall = true;
							break;
						case QMessageBox::Yes:
							continue;
							break;
						case QMessageBox::No:
						default:
							return;
					}
				}
			} else {
				v.push_back((*it).data(Qt::EditRole).toInt());
			}
		}
	}

	emit removeRecordRequested(v, true);
}

#include "BatchTableView.moc"