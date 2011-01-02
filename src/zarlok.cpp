#include "zarlok.h"
#include "globals.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>

#include <QFileDialog>
// #include <QPrintDialog>

#include <QPrinter>

// #include <QTextDocument>
#include <QTextTable>
#include <QAbstractTextDocumentLayout>

// public members

/**
 * @brief Główny konstruktor klasy Zarlok. Klasa Zarlok jest główną klasą,
 * odpowiada za wyświetlanie głównego formularza (jest pochodną klasy
 * QMainWindow.
 *
 * @param parent QMainWindow
 **/
zarlok::zarlok(const QString & file, QWidget * parent) : QMainWindow(parent), db(Database::Instance()), dwm_prod(NULL),
	model_prod(NULL), model_batch_delegate(NULL), model_batchbyid(NULL) {
    setupUi(this);
	this->setWindowTitle(tr("Zarlok by Rafal Lalik [pre-demo version], 11.2010"));

	widget_add_products->setVisible(false);
	widget_add_batch->setVisible(false);

	aprw = new AddProductsRecordWidget(widget_add_products);
	abrw = new AddBatchRecordWidget(widget_add_batch);
	activateUi(false);

	connect(actionNewDB, SIGNAL(triggered(bool)), this, SLOT(createDB()));
	connect(actionOpenDB, SIGNAL(triggered(bool)), this, SLOT(openDB()));
	connect(actionSaveDB, SIGNAL(triggered(bool)), this, SLOT(saveDB()));
	connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
	connect(actionPrintReport, SIGNAL(triggered(bool)), this, SLOT(printReport()));

	connect(button_add_prod, SIGNAL(toggled(bool)), this, SLOT(add_prod_record(bool)));
	connect(table_products, SIGNAL(addRecordRequested(bool)), button_add_prod, SLOT(setChecked(bool)));
	connect(aprw, SIGNAL(canceled(bool)), button_add_prod, SLOT(setChecked(bool)));

	connect(button_add_batch, SIGNAL(toggled(bool)), this, SLOT(add_batch_record(bool)));
	connect(table_batch, SIGNAL(addRecordRequested(bool)), button_add_batch, SLOT(setChecked(bool)));
	connect(abrw, SIGNAL(canceled(bool)), button_add_batch, SLOT(setChecked(bool)));

//	Filtrowanie na razie działa źle, robione na szybko. Trzeba przemyśleć sprawę i zająć się tym później.
// 	connect(table_products,  SIGNAL(recordsFilter(QString)), this, SLOT(set_filter(QString)));

	PR(!file.isEmpty());
	if (!file.isEmpty()) {
		QFile inf(file);
		openDB(!inf.open(QIODevice::ReadOnly), file);
	}
}

zarlok::~zarlok() {
	if (model_batch_delegate) delete model_batch_delegate;
	if (aprw) delete aprw;
	if (abrw) delete abrw;
}

void zarlok::set_filter(const QString& str) {
	QString q("select \"spec\", \"curr_qty\" from batch");
	q.append(str);
	if (table_batchbyid->model() != NULL) {
		((QSqlQueryModel *)table_batchbyid->model())->setQuery(q);
// 		model_batchbyid->setQuery(q);
	}
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
	std::cout << "++ zarlok::activateUi\n";

	MainTab->setVisible(true);
	MainTab->setEnabled(activate);

	if (activate) {
		// products
		if ((model_prod = db.CachedProducts())) {
			table_products->setModel(model_prod);
			table_products->show();
			connect(edit_filter_prod, SIGNAL(textChanged(QString)), model_prod, SLOT(filterDB(QString)));
			aprw->update_model();
		}
		// batch
		if ((model_batch = db.CachedBatch())){
			table_batch->setModel(model_batch);
			if (model_batch_delegate) delete model_batch_delegate;
			model_batch_delegate = new QSqlRelationalDelegate(table_batch);
			table_batch->setItemDelegate(model_batch_delegate);
			table_batch->show();
			connect(edit_filter_batch, SIGNAL(textChanged(QString)), model_batch, SLOT(filterDB(QString)));
			abrw->update_model();
			
// 			table_batchbyid->setModel(model_batch);
// 			table_batchbyid->show();
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

// 		if (model_batchbyid != NULL) delete model_batchbyid;
// 		if (model_batchbyid = new QSqlQueryModel()) {
// 			model_batchbyid->setQuery("select \"spec\", \"curr_qty\" from batch");
// 			table_batchbyid->setModel(model_batchbyid);
// 			table_batchbyid->show();
// 		}
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
	std::cout << "++ zarlok::openDB()\n";
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
		PR(recreate);
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
	PR(model_batch->query().executedQuery().toStdString());
	model_prod->submitAll();
}

void zarlok::printReport() {
// 	QPrinter printer;
// 
// 	QPrintDialog *dialog = new QPrintDialog(&printer, this);
// 	dialog->setWindowTitle(tr("Print Document"));
// 	if (editor->textCursor().hasSelection())
// 		dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);
// 	if (dialog->exec() != QDialog::Accepted)
// 		return;

// 	int numberOfPages = 2;

	// Prepare printer
	QPrinter printer(QPrinter::HighResolution);
	printer.setPaperSize(QPrinter::A4);
// 	printer.setOrientation(QPrinter::Landscape);
	printer.setOutputFileName("print.pdf");

	// Prepare document
	QTextDocument doc;
	QTextCursor cur(&doc);
	cur.movePosition(QTextCursor::Start);

	// Products table preparation

	cur.insertHtml("<div style='width: 100%; text-align: center;'>Summary of products in the camp storage</div>");
	cur.insertHtml("<hr /><br />");
	cur.insertText(tr("Products list"));

// 	cur.insertHtml("<span style=\"width: 200px; text-align: right; border: 1px solid red;\">Data</span>");
// 	cur.insertHtml("<br /><em>Data</em>");

	int p_rows = model_prod->rowCount();
	int p_cols = model_prod->columnCount();

	QTextTableFormat tabf;
	tabf.setBorderStyle(QTextFrameFormat::BorderStyle_Outset);
	tabf.setBorder(1);
	tabf.setCellSpacing(0);
	tabf.setCellPadding(5);
	tabf.setAlignment(Qt::AlignTop);
	tabf.setWidth(QTextLength(QTextLength::PercentageLength, 100));

	QTextTable * products_table = cur.insertTable(p_rows, p_cols, tabf);

	for (int i = 0; i < p_rows; i++) {
		for (int j = 0; j < p_cols; j++) {
			QTextCursor cur = products_table->cellAt(i, j).firstCursorPosition();
			cur.insertText(model_prod->index(i, j).data().toString());
		}
	}

// 	QPainter painter(&printer);
// 	QAbstractTextDocumentLayout * lay = report.documentLayout();
// 	lay->setPaintDevice(painter.device());
// 	report.setHtml("htmlcontent");
// 	report.drawContents(&painter/*, printer.pageRect()*/);

	// Print document
	doc.print(&printer);

// 	QPainter painter;
// 	painter.begin(&printer);
// 
// 	for (int page = 0; page < numberOfPages; ++page) {
// 		// Use the painter to draw on the page.
// 		painter.drawText(0, 0, trUtf8("text próbny"));
// // 		report.drawContents(&painter, QRectF(10, 10, 100, 100));
// 		report.drawContents(&painter, printer.paperRect());
// // 		report.print(&printer);
// // db.CachedProducts()
// // 		painter.draw
// 		if (page != numberOfPages-1)
// 			printer.newPage();
// 	}
// 	painter.end();

// 	QPrintDialog * dialog = new QPrintDialog(&printer, this);
// 	dialog->show();
// 	delete dialog;
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
