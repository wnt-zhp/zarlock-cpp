#include "zarlok.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>

#include <QSqlRelationalDelegate>

#include <QFileDialog>
#include <QItemSelectionModel>
#include <QAbstractItemView>

#include <iostream>
#define PR(x) cout << "++DEBUG: " << #x << " = |" << x << "|\n";

using namespace std;

// public members

/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok(const QString & file, QWidget * parent) : QMainWindow(parent), db(Database::Instance()), dwm_prod(NULL), model_prod(NULL), model_batch_delegate(NULL) {
    setupUi(this);
	this->setWindowTitle(tr("Zarlok by Rafal Lalik [R&D version], 11.2010"));

	widget_add_products->setVisible(false);
	widget_add_batch->setVisible(false);

	aprw = new AddProductsRecordWidget(widget_add_products);
	abrw = new AddBatchRecordWidget(widget_add_batch);
	activateUi(false);

	connect(actionNewDB, SIGNAL(triggered(bool)), this, SLOT(createDB()));
	connect(actionOpenDB, SIGNAL(triggered(bool)), this, SLOT(openDB()));
	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));

	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(add_prod_record(bool)));
	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(aprw, SIGNAL(canceled(bool)), button_add_prod, SLOT(setChecked(bool)));

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(add_batch_record(bool)));
	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(abrw, SIGNAL(canceled(bool)), button_add_batch, SLOT(setChecked(bool)));

	if (!file.isEmpty()) {
		openDB(false, file);
	}
}

zarlok::~zarlok() {
	if (model_batch_delegate) delete model_batch_delegate;
	if (aprw) delete aprw;
	if (abrw) delete abrw;
}

// private members

/**
 * @brief Funkcja ustawia wartości wszystkich kontrolek w głownym oknie.
 * Należy wywołać funkcję za każdym razem, gdy ładujemy nową bazę.
 *
 * @param activate Gdy ustawione, powoduje deaktywację kontrolek
 * @return void
 **/
void zarlok::activateUi(bool activate) {
	MainTab->setVisible(true);
	MainTab->setEnabled(activate);

	if (activate) {
		// products
		if (model_prod = db.CachedProducts()) {
			table_products->setModel(model_prod);
			table_products->show();
			connect(edit_filter_prod, SIGNAL(textChanged(QString)), model_prod, SLOT(filterDB(QString)));
		}
		// batch
		if (model_batch = db.CachedBatch()){
			table_batch->setModel(model_batch);
			if (model_batch_delegate) delete model_batch_delegate;
			model_batch_delegate = new QSqlRelationalDelegate(table_batch);
			table_batch->setItemDelegate(model_batch_delegate);
			table_batch->show();
			connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_batch, SLOT(filterDB(QString)));
			abrw->update_model();
		}

// 		table_products->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
// 		table_products->horizontalHeader()->setStretchLastSection(true);
// 		table_batch->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
// 		table_batch->horizontalHeader()->setStretchLastSection(true);

// 		table_products->resizeColumnsToContents();
// 		table_batch->resizeColumnsToContents();
// 		if (dwm_prod) delete dwm_prod;
// 		dwm_prod = new QDataWidgetMapper;
// 		dwm_prod->setModel(model_prod);
// 		dwm_prod->addMapping(edit_products, 1);
// 		dwm_prod->toFirst();
	}
}

/**
 * @brief Slot - otwiera bazę danych po wywołaniu akcji z menu lub skrótu klawiatury
 *
 * @param recreate jeśli w bazie istnieją tabele, wpierw je usuń i stwórz na nowo
 * @param file nazwa pliku do otwarcia
 * @return bool
 **/
void zarlok::openDB(bool recreate, const QString & file) {
	QString fn;
	if (file.isEmpty()) {
		if (recreate) {
			fn = QFileDialog::getSaveFileName(this, tr("Create Database File..."),
							QString("~/projects/zarlok"), tr("SQLite3 DB-Files (*.db);;All Files (*)"));
// 			fn = "/home/dziadu/projects/zarlok/oboz.db";
		} else {
			fn = QFileDialog::getOpenFileName(this, tr("Open Database File..."),
							QString("~/projects/zarlok"), tr("SQLite3 DB-Files (*.db);;All Files (*)"));
// 			fn = "/home/dziadu/projects/zarlok/oboz.db";
		}
	} else {
		fn = file;
	}

	PR(fn.toStdString());

	if (!fn.isEmpty()) {
		if (db.open_database(fn, recreate)) {
			activateUi();
		}
	}
}

/**
 * @brief Slot - zapisuje bazę danych wraz ze wszystkimi zmianami
 *
 * @return bool - wynik wykonania QTableModel::submitAll()
 **/
void zarlok::saveDB() {
	model_batch->submitAll();
	model_prod->submitAll();
}

void zarlok::add_prod_record(bool newrec) {
	if (newrec) {
// 		table_products->setVisible(false);
		widget_add_products->setVisible(true);
	} else {
// 		table_products->setVisible(true);
		widget_add_products->setVisible(false);
	}
}

void zarlok::add_batch_record(bool newrec) {
	if (newrec) {
// 		table_products->setVisible(false);
		widget_add_batch->setVisible(true);
	} else {
// 		table_products->setVisible(true);
		widget_add_batch->setVisible(false);
	}
}

void zarlok::about() {
//      QLabel infoLabel->setText(tr("Invoked <b>Help|About</b>"));
     QMessageBox::about(this, tr("About Menu"),
             tr("The <b>Menu</b> example shows how to create "
                "menu-bar menus and context menus."));
}

#include "zarlok.moc"
