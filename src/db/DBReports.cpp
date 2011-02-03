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

#include "Database.h"
#include "globals.h"
#include "config.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QPrinter>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>

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
						QObject::tr("2Check your installation and try to run again.\n"
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
		QString("/") + date.toString("yyyy-MM-dd") + QString("_raport.pdf");
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


#include "DBReports.moc"