/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Rafał Lalik <rafal.lalik@ph.tum.de>

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


#include "DBReports.h"

#include "globals.h"
#include "config.h"

#include "DataParser.h"
#include "Database.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPrinter>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QStringBuilder>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

DBReports::DBReports() {

}

DBReports::~DBReports() {

}

void DBReports::printDailyReport(const QString & dbname, const QDate & date) {
	QFile batch_tpl(":/resources/report_batch.tpl");
	if (!batch_tpl.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, QObject::tr("Cannot find resources"),
			QObject::tr("Unable to find resources in ") +
						batch_tpl.fileName() + "\n" +
						QObject::tr("Check your installation and try to run again.\n"
						"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QFile sheet_css(":/resources/report.css");
	if (!sheet_css.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, QObject::tr("Cannot find resources"),
			QObject::tr("Unable to find resources in ") +
						sheet_css.fileName() + "\n" +
						QObject::tr("Check your installation and try to run again.\n"
						"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QTextStream batch_tstream(&batch_tpl);
	QTextStream sheet_tstream(&sheet_css);

	// Prepare printer
	QPrinter printer(QPrinter::HighResolution);
	printer.setPaperSize(QPrinter::A4);
	printer.setColorMode(QPrinter::Color);
	printer.setOutputFormat(QPrinter::PdfFormat);
	QString ofile =
		QDir::homePath() + QString(ZARLOK_HOME ZARLOK_REPORTS) + dbname +
		QString("/") + date.toString(Qt::ISODate) + QString("_raport.pdf");
	PR(ofile.toStdString());
	printer.setOutputFileName(ofile);

	// Prepare document
	QTextDocument doc;
	QString sheet = sheet_tstream.readAll();
	doc.setDefaultStyleSheet(sheet);
// 	PR(sheet.toStdString());
	QTextCursor cur(&doc);
	cur.movePosition(QTextCursor::Start);

	// Products table preparation

	Database & db = Database::Instance();
	int p_rows = db.CachedBatch()->rowCount();
	int p_cols = 4;//model_batch->columnCount();
	int batch_map[4] = { 0, 2, 4, 3 };
	QString tcont;

	for (int i = 0; i < p_rows; i++) {
		tcont += "<tr>";
		for (int j = 0; j < p_cols; j++) {
// 			QTextCursor cur = products_table->cellAt(i, j).firstCursorPosition();
// 			cur.insertText(model_prod->index(i, j).data().toString());
			tcont += "<td>" + db.CachedBatch()->index(i, batch_map[j]).data().toString() + "</td>";
		}
		tcont += "</tr>";
	}

	// Print document

	QString tpl = batch_tstream.readAll();
	tpl.replace("@DATE@", (QDate::currentDate()).toString());
	tpl.replace("@TABLE_CONTENT@", tcont);
	doc.setHtml(tpl);

	doc.print(&printer);
}

void DBReports::showDailyMealReport(const QString& date, QString * reportfile, bool displayPDF) {
	QFile dailymeal_tpl(":/resources/report_dailymeal.tpl");
	if (!dailymeal_tpl.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, QObject::tr("Cannot find resources"),
			QObject::tr("Unable to find resources in ") +
						dailymeal_tpl.fileName() + "\n" +
						QObject::tr("Check your installation and try to run again.\n"
						"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QFile sheet_css(":/resources/report.css");
	if (!sheet_css.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, QObject::tr("Cannot find resources"),
			QObject::tr("Unable to find resources in ") +
						sheet_css.fileName() + "\n" +
						QObject::tr("Check your installation and try to run again.\n"
						"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QTextStream dailymeal_tstream(&dailymeal_tpl);
	QTextStream sheet_tstream(&sheet_css);

	// Prepare printer
	QPrinter printer(QPrinter::HighResolution);
	printer.setPaperSize(QPrinter::A4);
	printer.setColorMode(QPrinter::Color);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOrientation(QPrinter::Landscape);

	Database & db = Database::Instance();

	QString ofile =
		QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % db.openedDatabas() %
		QString("/") % date % QString("_dailymeal.pdf");
	PR(ofile.toStdString());
	printer.setOutputFileName(ofile);

	// Prepare document
	QTextDocument doc;
	QString sheet = sheet_tstream.readAll();
	doc.setDefaultStyleSheet(sheet);
// 	PR(sheet.toStdString());
	QTextCursor cur(&doc);
	cur.movePosition(QTextCursor::Start);

	// Products table preparation
	int p_rows = db.CachedBatch()->rowCount();
	int p_cols = 4;//model_dailymeal->columnCount();
	int dailymeal_map[4] = { 0, 2, 4, 3 };
	QString tcont;

	QSqlQuery q;

	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
		QSqlDatabase::database().transaction();

	q.prepare("SELECT distdate, scouts, leaders, others FROM meal WHERE distdate=?;");
	q.bindValue(0, date);
	q.exec();

	QString distdate;
	float costs = 0.0;
	int mealpersons = 0;

	while (q.next()) {
		mealpersons = q.value(1).toInt() + q.value(2).toInt() + q.value(3).toInt();
		distdate = q.value(0).toString();
	}

	q.prepare("SELECT batch.price,distributor.quantity FROM batch,distributor where distributor.distdate=? AND batch.id=distributor.batch_id;");
	q.bindValue(0, distdate);
	q.exec();
	while (q.next()) {
		double netto, tax;
		DataParser::price(q.value(0).toString(), netto, tax);
		costs += netto*(1.0 + tax/100.0)*q.value(1).toFloat();
	}

	if (q.driver()->hasFeature(QSqlDriver::Transactions))
		if (!QSqlDatabase::database().commit())
			QSqlDatabase::database().rollback();

	for (int i = 0; i < p_rows; i++) {
		tcont += "<tr>";
		for (int j = 0; j < p_cols; j++) {
// 			QTextCursor cur = products_table->cellAt(i, j).firstCursorPosition();
// 			cur.insertText(model_prod->index(i, j).data().toString());
			tcont += "<td>" + db.CachedBatch()->index(i, dailymeal_map[j]).data().toString() + "</td>";
		}
		tcont += "</tr>";
	}

	// Print document

	QString tpl = dailymeal_tstream.readAll();
	tpl.replace("@DATE@", (QDate::currentDate()).toString());
	tpl.replace("@TABLE_CONTENT@", tcont);
	doc.setHtml(tpl);

	doc.print(&printer);
}

#include "DBReports.moc"