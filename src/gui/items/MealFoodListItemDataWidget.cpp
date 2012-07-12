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

#include <QStringBuilder>
#include <QStyledItemDelegate>
#include <QMessageBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QDesktopWidget>

#include "globals.h"

#include "Database.h"

#include "MealFoodListItemDataWidget.h"
#include "MealFoodList.h"
#include "MealTabWidget.h"

#include "BatchTableView.h"
#include "MealFoodList.h"
#include "BatchTableModelProxy.h"
#include "Database.h"

#include "EventFilter.h"
#include "TextInput.h"

const int new_width = 1000;

MealFoodListItemDataWidget::MealFoodListItemDataWidget(QWidget* parent, QListWidgetItem * item, Qt::WindowFlags f):
	QWidget(parent, f), empty(true), editable(true), lock(true),
	batch_row(-1), dist_id(-1), dist_row(-1),
	owner(item), tv(NULL), evf(NULL), ledit(NULL)
{
	CII();

	mfl = dynamic_cast<MealFoodList *>(parent);
	setupUi(this);

	filter_string.reserve(100);
	evf = new EventFilter;

	evf->registerFilter(QEvent::KeyPress);
	evf->registerFilter(QEvent::Show);
	evf->registerFilter(QEvent::Hide);

	batch->installEventFilter(evf);

	connect(evf, SIGNAL(eventFiltered(QEvent*)), this, SLOT(eventCaptured(QEvent*)));

	vevf = new EventFilter;
	vevf->registerFilter(QEvent::Show);

	connect(vevf, SIGNAL(eventFiltered(QEvent*)), this, SLOT(viewEventCaptured(QEvent*)));

	ledit = new TextInput(this);
	ledit->setVisible(false);
	proxy = ((MealTabWidget *)(mfl->parent()->parent()))->getBatchProxyModel();

	batch->setModel(proxy);
	batch->setModelColumn(BatchTableModel::HSpec);

	batch->setAutoCompletion(true);
	batch->setAutoCompletionCaseSensitivity(Qt::CaseInsensitive);

	addB->setMaximumSize(32, 32);
	addB->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	addB->setIconSize(QSize(16,16));
	updateB->setMaximumSize(32, 32);
	updateB->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	updateB->setIconSize(QSize(16,16));
	removeB->setMaximumSize(32, 32);
	removeB->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
	removeB->setIconSize(QSize(16,16));
	closeB->setMaximumSize(32, 32);
	closeB->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
	closeB->setIconSize(QSize(16,16));

	addB->setFlat(true);
	updateB->setFlat(true);
	removeB->setFlat(true);
	closeB->setFlat(true);

	qty->setMaximumSize(128, 32);
	qty->setMaximum(9999);
	qty_label->setMaximumSize(128, 32);

	connect(batch, SIGNAL(activated(int)), this, SLOT(validateBatchAdd()));
	connect(qty, SIGNAL(valueChanged(int)), this, SLOT(validateAdd()));

	connect(addB, SIGNAL(clicked(bool)), this, SLOT(buttonAdd()));
	connect(updateB, SIGNAL(clicked(bool)), this, SLOT(buttonUpdate()));
	connect(removeB, SIGNAL(clicked(bool)), this, SLOT(buttonRemove()));
	connect(closeB, SIGNAL(clicked(bool)), this, SLOT(buttonClose()));

	connect(this, SIGNAL(itemRemoved(QListWidgetItem*)), owner->listWidget(), SLOT(doItemRemoved(QListWidgetItem*)));

	resetWidget();
	render();
}

MealFoodListItemDataWidget::~MealFoodListItemDataWidget() {
	DII();
	deleteView();
}

void MealFoodListItemDataWidget::resetWidget() {
	batch_row = -1;
	invalidateProxy();

// 	PR(batch_row);PR(&batch_row);

	batch->setCurrentIndex(batch_row);
	addB->setEnabled(false);
}

void MealFoodListItemDataWidget::convertToEmpty() {
	resetWidget();

	batch_label->setText("---");
	qty_label->setText("---");
	label_price->setText("---");
	label_unit->setText("---");

	qty->setValue(0);
	qty->setMaximum(0);

	empty = true;
	editable = true;

	prepareView();
	render();
}

/** @brief Render widget
 * @param doRender if thre then render 'readable' version of widget, otherwise render 'editable' version
 */
void MealFoodListItemDataWidget::render() {FII();
	addB->setVisible(editable);
	updateB->setVisible(!editable);
	removeB->setVisible(!empty);
	closeB->setVisible(editable and !empty);

	qty->setVisible(editable);
	qty_label->setVisible(!editable);
	qty->setSuffix(tr(" of %1.%2").arg(qty->maximum()/100).arg(qty->maximum() % 100, 2, 10, QChar('0')));

	batch->setVisible(editable);
	batch_label->setVisible(!editable);
}

void MealFoodListItemDataWidget::update() {
	int tmp_batch_row = proxy->mapToSource(proxy->index(batch->currentIndex(), BatchTableModel::HId)).row();

	int tmp_qty_max = 1000000;

	BatchTableModel * btm = Database::Instance()->CachedBatch();
	DistributorTableModel * dtm = Database::Instance()->CachedDistributor();

	int ball = btm->index(tmp_batch_row, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
	int used = btm->index(tmp_batch_row, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
	int dqty = dtm->index(dist_row, DistributorTableModel::HQty).data(Qt::EditRole).toInt();
	if (tmp_batch_row >= 0) {
		if (tmp_batch_row != batch_row) {
			qty->setValue(0.0);
			tmp_qty_max = double(ball-used);
		} else {
			qty->setValue(quantity);
			tmp_qty_max = double(ball-used + dqty);
		}
	}

	QString tmp_batch_label = btm->index(tmp_batch_row, BatchTableModel::HSpec).data(Qt::DisplayRole).toString();
	batch_label->setText(tmp_batch_label);

	qty->setMaximum(tmp_qty_max);
	qty->setSuffix(tr(" of %1.%2").arg(tmp_qty_max/100).arg(tmp_qty_max % 100, 2, 10, QChar('0')));

	qty_label->setText(QString("%1.%2").arg(qty->value()/100).arg(qty->value() % 100, 2, 10, QChar('0')));

	QString unit = btm->index(tmp_batch_row, BatchTableModel::HUnit).data(Qt::DisplayRole).toString();
	QString price = btm->index(tmp_batch_row, BatchTableModel::HPrice).data(Qt::DisplayRole).toString();

	label_price->setText(price);
	label_unit->setText(unit);
}

void MealFoodListItemDataWidget::validateBatchAdd() {
	if (batch->currentIndex() < 0)
		return;

	update();
}

void MealFoodListItemDataWidget::validateAdd() {
	if ((qty->value() != 0) && batch->currentIndex() >= 0) {
		addB->setEnabled(true);
	} else {
		addB->setEnabled(false);
	}
}

/** @brief Insert/replace record
 */
void MealFoodListItemDataWidget::buttonAdd() {
	Database * db = Database::Instance();

	// Map current batch proxy model selection to general batch model
	QModelIndex bidx = proxy->mapToSource(proxy->index(batch->currentIndex(), BatchTableModel::HId));
	batch_row = bidx.row();
	quantity = qty->value();

	QDate reference_date = mfl->proxyModel()->refDate();

	// If item is empty then we perform insert action
	if (empty) {
		// Get proxy model of distributions for current meal and fetch list of distributions of the same batch
		MealTableModelProxy * pr = dynamic_cast<MealFoodList *>(owner->listWidget())->proxyModel();
		if (pr == NULL) return;
		pr->invalidate();

		QModelIndexList same_meal_list = pr->match(pr->index(0, DistributorTableModel::HBatchId), Qt::EditRole, bidx.data(Qt::EditRole), -1, Qt::MatchExactly);

		// Ask if we merge them or we add as a new entry
		int ans = mergeBox(same_meal_list);
		// If canceled then exit
		if (ans == -2)
			return;

		// If selected new entry
		if (ans == -1) {
			// Try add to database
			if (db->addDistributorRecord(bidx.data().toInt(), quantity, reference_date, reference_date,
				DistributorTableModel::RMeal, QString("%1").arg(mfl->proxyModel()->key()), "")) {
				// Reload widget data from database and ask for new empty slot
				dist_row = db->CachedDistributor()->rowCount()-1;
				setWidgetData(db->CachedDistributor()->index(dist_row, DistributorTableModel::HId).data(Qt::EditRole).toInt());
				invalidateProxy();
				//TODO: this must be outside widget
				mfl->insertEmptySlot();
			} else
				// If insertion failed then return
				return;
		}
		// If we requestested merge with another widget
		else {
			// Get row to update, get old quantity value and...
			int update_row = pr->mapToSource(same_meal_list.at(ans)).row();
			int old_qty = pr->index(same_meal_list.at(ans).row(), DistributorTableModel::HQty).data(Qt::EditRole).toInt();
			// ...update record with new quantity
			if (db->updateDistributorRecord(update_row, bidx.data().toUInt(), old_qty + quantity, reference_date,
				reference_date, DistributorTableModel::RMeal, QString("%1").arg(mfl->proxyModel()->key()), "")) {
				// Refresh modified widget
				mfl->refreshItem(pr->index(same_meal_list.at(ans).row(), DistributorTableModel::HId).data(Qt::EditRole).toInt());
				// Convert current to empty and reload proxy
				convertToEmpty();
				invalidateProxy();
			} else
				// If insertion failed then return
				return;
		}
	}
	// otherwise we have update action
	else {
		// Try update record in database
		if (db->updateDistributorRecord(dist_row, bidx.data().toUInt(), quantity, reference_date,
			reference_date, DistributorTableModel::RMeal, QString("%1").arg(mfl->proxyModel()->key()), "")) {
			// Update record data
			invalidateProxy();
			setWidgetData(db->CachedDistributor()->index(dist_row, DistributorTableModel::HId).data(Qt::EditRole).toInt());
		} else
			// If update failed then return
			return;
	}

	// Refresh meal model data
	db->CachedMealDay()->select();
}

/** @brief Prepare widget to update data.
 */
void MealFoodListItemDataWidget::buttonUpdate() {
	// if empty then no update available
	if (empty)
		return;

	// Close other open items and this as open
	dynamic_cast<MealTabWidget *>(mfl->parent()->parent())->closeOpenItems();
	dynamic_cast<MealTabWidget *>(mfl->parent()->parent())->markOpenItem(owner);

	// prepare proxy model and prepare table view
	invalidateProxy();
	prepareView();

	// set proper index of batch combo box
	batch->setCurrentIndex(proxy->mapFromSource(Database::Instance()->CachedBatch()->index(batch_row, BatchTableModel::HId)).row());

	// mart as editable and render widget
	editable = true;
	render();
}

/** @brief Close widget and display data
 */
void MealFoodListItemDataWidget::buttonClose() {
	// if button is empty then do not close it
	if (!empty) {
		// remove view model and reset to oryginal data
		deleteView();
		resetWidgetData();
	}
}

/** @brief Remove widget and item.
 *  This will result with remove data from database
 */
void MealFoodListItemDataWidget::buttonRemove() {
	Database * db = Database::Instance();

	// Prepare list of records to remove, delete and refresh batch model
	QVector<int> v;
	v.push_back(dist_id);
	db->removeDistributorRecord(v);
	db->CachedBatch()->selectRow(batch_row);

	// inform about removal
	emit itemRemoved(owner);
}

/** @brief Prepare data for widget
 * @param idx index of batch
 */
void MealFoodListItemDataWidget::setWidgetData(int did) {
	dist_id = did;

	BatchTableModel * btm = Database::Instance()->CachedBatch();
	DistributorTableModel * dtm = Database::Instance()->CachedDistributor();

	dist_row = dtm->getRowById(dist_id);

	// set batch
	int tmp_dist_batchid = dtm->index(dtm->getRowById(did), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt();
	batch_row = btm->getRowById(tmp_dist_batchid);

	// set quantity
	quantity = dtm->index(dist_row, DistributorTableModel::HQty).data(Qt::EditRole).toInt();

	int ball = btm->index(batch_row, BatchTableModel::HStaQty).data(Qt::EditRole).toInt();
	int used = btm->index(batch_row, BatchTableModel::HUsedQty).data(Qt::EditRole).toInt();
	int dqty = dtm->index(dist_row, DistributorTableModel::HQty).data(Qt::EditRole).toInt();

	int tmp_qty_max = double(ball - used + dqty);

	qty->setMaximum(tmp_qty_max);
	qty->setValue(quantity);
	qty->setSuffix(tr(" of %1.%2").arg(tmp_qty_max/100).arg(tmp_qty_max % 100, 2, 10, QChar('0')));
	qty_label->setText(QString("%1.%2").arg(quantity/100).arg(quantity % 100, 2, 10, QChar('0')));

	editable = false;
	empty = false;

	invalidateProxy();
	batch->setCurrentIndex(proxy->mapFromSource(btm->index(batch_row, BatchTableModel::HSpec)).row());

	update();
	render();
}

bool MealFoodListItemDataWidget::isEmpty() {
	return empty;
}

void MealFoodListItemDataWidget::deleteView() {
	delete tv;
	tv = NULL;
}

void MealFoodListItemDataWidget::prepareView() {
	FII();

	delete tv;
	tv = new BatchTableView;
	tv->installEventFilter(evf);

	tv->verticalHeader()->setDefaultSectionSize(20);
	tv->horizontalHeader()->setVisible(true);
	tv->verticalHeader()->setVisible(false);

// 	tv->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
// 	tv->resizeColumnsToContents();
// 	tv->resizeColumnToContents(BatchTableModel::HSpec);
	tv->setSelectionBehavior(QAbstractItemView::SelectRows);

	batch->setView(tv);

	tv->selectRow(proxy->mapFromSource(proxy->sourceModel()->index(batch_row, BatchTableModel::HSpec)).row());
	tv->sortByColumn(BatchTableModel::HSpec, Qt::AscendingOrder);

	tv->installEventFilter(vevf);
}

int MealFoodListItemDataWidget::mergeBox(const QModelIndexList& list) {
	int lsize = list.size();
	if (lsize == 0)
		return -1;

	QMessageBox * mbox = new QMessageBox(QMessageBox::Question, tr("Add distribution"),
												tr("There are existing distributions of this batch on the meal list. "
												"It is recommended to merge your new distribution with existing one. "
												"Please select your action from the list below."),
												QMessageBox::Save | QMessageBox::Discard);

	QGridLayout * glay = dynamic_cast<QGridLayout *>(mbox->layout());

	int items_num = glay->count();
	QLayoutItem * li = glay->takeAt(items_num-1);

	QWidget * radioWidget = new QWidget;
	QVBoxLayout * layout = new QVBoxLayout;
	QScrollArea * scroll = new QScrollArea;

	QVector<QRadioButton *> radios(list.size()+1);

	radios[0] = new QRadioButton(tr("Add new record"), radioWidget);
	mbox->button(QMessageBox::Save)->setEnabled(false);
	connect(radios[0], SIGNAL(toggled(bool)), mbox->button(QMessageBox::Save), SLOT(setEnabled(bool)));

	layout->addWidget(radios[0]);

	BatchTableModel * btm = Database::Instance()->CachedBatch();
	const MealTableModelProxy * p = dynamic_cast<MealFoodList *>(owner->listWidget())->proxyModel();

	for (int i = 0; i < lsize; ++i) {
		int row = btm->getRowById(list.at(i).data(Qt::EditRole).toInt());
		QString n = btm->index(row, BatchTableModel::HSpec).data().toString();
		double q = p->index(list.at(i).row(), DistributorTableModel::HQty).data().toDouble()/100;

		radios[1+i] = new QRadioButton(tr("Merge with %1 ( Qty: %2 )").arg(n).arg(q), radioWidget);
		connect(radios[1+i], SIGNAL(toggled(bool)), mbox->button(QMessageBox::Save), SLOT(setEnabled(bool)));
		layout->addWidget(radios[1+i]);
	}

	radioWidget->setLayout(layout);
	radioWidget->show();
	scroll->setWidget(radioWidget);
	scroll->setMaximumHeight(layout->itemAt(0)->geometry().height()*6);

	glay->addWidget(scroll, glay->rowCount(), 0, 1, -1);
	glay->addItem(li, glay->rowCount(), 0, 1, -1);
	int ans = mbox->exec();
	int res = -1;

	if (ans == QMessageBox::Save) {
		for (int r = 0; r < radios.size(); ++r) {
			if (radios[r]->isChecked()) {
				res = r-1;
				break;
			}
		}
	} else
	if (ans == QMessageBox::Discard) {
		res = -2;
	}

	glay->takeAt(glay->indexOf(scroll));

	while (layout->itemAt(0) != NULL)
		layout->takeAt(0);

	qDeleteAll(radios);

	delete layout;
	delete radioWidget;
	delete scroll;
	delete mbox;

	return res;
}

/** @brief Get id of distributed record.
 * @return id of distributor record. -1 if no data (empty item or header item).
 */
int MealFoodListItemDataWidget::distributorId() {
	return dist_id;
}

/** @brief Restore default widget data
 */
void MealFoodListItemDataWidget::resetWidgetData() {
	setWidgetData(dist_id);
}

/** @brief Make widget a header - temporary solution
 */
void MealFoodListItemDataWidget::convertToHeader() {
	editable = false;
	batch_row = -1;
	batch_label->setText(tr("Batch"));
	qty_label->setText(tr("Quantity"));
	label_price->setText(tr("Price"));
	label_unit->setText(tr("Unit"));

	QPalette bkg, text;
	bkg.setBrush(QPalette::Window, QBrush(Qt::gray));
	this->setAutoFillBackground(true);
	this->setPalette(bkg);

	text.setBrush(QPalette::Base, QBrush(Qt::darkGray));
	text.setBrush(QPalette::Text, QBrush(Qt::white));

	text.setBrush(QPalette::Button, QBrush(Qt::darkGray));
	text.setBrush(QPalette::ButtonText, QBrush(Qt::white));
	text.setBrush(QPalette::ButtonText, QBrush(Qt::white));

	batch_label->setAutoFillBackground(true);
	qty_label->setAutoFillBackground(true);
	label_price->setAutoFillBackground(true);
	label_unit->setAutoFillBackground(true);

	batch_label->setPalette(text);
	qty_label->setPalette(text);
	label_price->setPalette(text);
	label_unit->setPalette(text);

	batch_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	qty_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	label_price->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	label_unit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	delete addB;
	addB = reinterpret_cast<QPushButton *>(new QLabel(tr("Actions")));
	this->layout()->addWidget(addB);
// 	addB->setText(tr("Actions"));
// 	addB->setIcon(QIcon());
// 	addB->setFlat(true);
// 	addB->setCheckable(false);
	addB->setVisible(true);
	addB->setMinimumSize(QSize(64, 16));
	reinterpret_cast<QLabel *>(addB)->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	addB->setAutoFillBackground(true);
	addB->setPalette(text);

// 	addB->setVisible(false);
	updateB->setVisible(false);
	removeB->setVisible(false);
	closeB->setVisible(false);

	qty->setVisible(false);
	qty_label->setVisible(true);

	batch->setVisible(false);
	batch_label->setVisible(true);
}

void MealFoodListItemDataWidget::invalidateProxy() {
	proxy->setItemNum(&batch_row);
	proxy->invalidate();
}

void MealFoodListItemDataWidget::eventCaptured(QEvent * evt) {
	if (evt->type() == QEvent::KeyPress) {
		QKeyEvent * kevt = dynamic_cast<QKeyEvent *>(evt);
		int key = kevt->key();

		switch (key) {
			case Qt::Key_Escape:
				filter_string.clear();
				break;
			case Qt::Key_Enter:
				break;
			case Qt::Key_Backspace:
				filter_string.remove(filter_string.size()-1,1);
				break;
			default:
				if (key < 0xffff) {
					filter_string.append(kevt->text());
				}
		}

		this->setFilterString(filter_string);
	} else
	if (evt->type() == QEvent::Show) {
// 		ledit->show();
// 		this->setFilterString(QString());
	}
	if (evt->type() == QEvent::Hide) {
		ledit->hide();
		this->setFilterString(QString());
	}
}

void MealFoodListItemDataWidget::viewEventCaptured(QEvent * evt) {
	if (evt->type() == QEvent::Show) {
		QFrame * popup = batch->findChild<QFrame*>();

		popup->resize(new_width, popup->rect().height());

		QRect screen;
#ifndef Q_WS_S60
		int screenid = QApplication::desktop()->screenNumber(this);
		screen = QApplication::desktop()->screenGeometry(screenid);
#else
		screen = qt_TRect2QRect(static_cast<CEikAppUi*>(S60->appUi())->ClientRect());
#endif

		QPoint p = popup->pos();
		int dxpos = screen.right() - (p.x() + new_width);
		if (dxpos < 0)
			popup->move(p.x()+dxpos, p.y());
	}
}

void MealFoodListItemDataWidget::setFilter() {
	this->invalidateProxy();
// 	table_batch->setModel(proxy_model);
}

void MealFoodListItemDataWidget::setFilterString(const QString& string) {
	ledit->setText(string);

	if (string.size() > 0) {
		ledit->show();
	}

	QString f = string;
	f.replace(' ', '*');
//	proxy_model->setFilterRegExp(QRegExp(f, Qt::CaseInsensitive, QRegExp::Wildcard));
	proxy->setFilterWildcard(f);
	proxy->setFilterKeyColumn(BatchTableModel::HSpec);
	proxy->setFilterRole(Qt::UserRole);
	proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

#include "MealFoodListItemDataWidget.moc"
