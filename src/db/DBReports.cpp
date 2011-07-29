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
// 	PR(ofile.toStdString());
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

void DBReports::printDailyMealReport(const QString& date, QString * reportfile) {
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
	printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

	Database & db = Database::Instance();

	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % db.openedDatabase());
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	QString ofile = dbsavepath.absolutePath() % QString("/") % date % QString("_dailymeal.pdf");
// 	PR(ofile.toStdString());
	printer.setOutputFileName(ofile);

	if (reportfile)
		*reportfile = ofile;

	// Prepare document
	QTextDocument doc;
	QString sheet = sheet_tstream.readAll();
	doc.setDefaultStyleSheet(sheet);
// 	PR(sheet.toStdString());
	QTextCursor cur(&doc);
	cur.movePosition(QTextCursor::Start);

	// Products table preparation
	QSqlQuery q;

	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
		QSqlDatabase::database().transaction();

	q.prepare("SELECT scouts, leaders, others FROM meal WHERE distdate=?;");
	q.bindValue(0, date);
	q.exec();

	QString distdate;
	int sco = 0, lea = 0, oth = 0, all = 0;

	while (q.next()) {
		sco = q.value(0).toInt();
		lea = q.value(1).toInt();
		oth = q.value(2).toInt();
		all = lea+sco+oth;
		distdate = QDate::fromString(date, Qt::ISODate).toString(Qt::DefaultLocaleLongDate);
	}

	QString cont0, cont2, cont3, cont4;
	QString cont1, cont5, cont6;
	QString contadd, spec, unit;
	double qty, costs = 0;

	QModelIndexList idxl = db.CachedDistributor()->match(db.CachedDistributor()->index(0, DistributorTableModel::HDistDate), Qt::EditRole, QDate::fromString(date, Qt::ISODate).toString(Qt::DefaultLocaleShortDate), -1);
	for (int i = 0; i < idxl.count(); ++i) {
		if (db.CachedDistributor()->index(idxl.at(i).row(), DistributorTableModel::HReason3).data().toInt() != 2)
			continue;

		spec = db.CachedDistributor()->index(idxl.at(i).row(), DistributorTableModel::HBatchId).data().toString();
		qty = db.CachedDistributor()->index(idxl.at(i).row(), DistributorTableModel::HQty).data().toDouble();

		q.prepare("SELECT batch.unit,batch.price FROM batch WHERE batch.id=?;");
		q.bindValue(0, db.CachedDistributor()->index(idxl.at(i).row(), DistributorTableModel::HBatchId).data(Qt::EditRole).toInt());
		q.exec();
		if (q.next()) {
			unit = q.value(0).toString();
			double netto, tax;
			if (DataParser::price(q.value(1).toString(), netto, tax)) {
				costs += netto*(1.0 + tax/100.0)*qty;
			}
		}

		switch (db.CachedDistributor()->index(idxl.at(i).row(), DistributorTableModel::HReason).data().toInt()) {
			case 0:
				cont0.append(QString("%1 - %2 x %3<br />").arg(spec).arg(qty).arg(unit));
				break;
			case 1:
				cont1.append(QString("%1 - %2 x %3, ").arg(spec).arg(qty).arg(unit));
				break;
			case 2:
				cont2.append(QString("%1 - %2 x %3<br />").arg(spec).arg(qty).arg(unit));
				break;
			case 3:
				cont3.append(QString("%1 - %2 x %3<br />").arg(spec).arg(qty).arg(unit));
				break;
			case 4:
				cont4.append(QString("%1 - %2 x %3<br />").arg(spec).arg(qty).arg(unit));
				break;
			case 5:
				cont5.append(QString("%1 - %2 x %3, ").arg(spec).arg(qty).arg(unit));
				break;
			case 6:
				cont6.append(QString("%1 - %2 x %3, ").arg(spec).arg(qty).arg(unit));
				break;
		}
	}

	while (q.next()) {
		cont1.append("%1 - %2 x %3\n").arg(q.value(0).toString()).arg(q.value(2).toFloat()).arg(q.value(1).toString());
	}

	if (q.driver()->hasFeature(QSqlDriver::Transactions))
		if (!QSqlDatabase::database().commit())
			QSqlDatabase::database().rollback();

	// Print document

	if (!cont1.isEmpty())
		contadd.append("* " % QObject::tr("2nd breakfast") % ": " % cont1 % "<br />");
	if (!cont5.isEmpty())
		contadd.append("* " % QObject::tr("Other 1") % ": " % cont5 % "<br />");
	if (!cont6.isEmpty())
		contadd.append("* " % QObject::tr("Other 2") % ": " % cont6 % "<br />");

	QString tpl = dailymeal_tstream.readAll();
	tpl.replace("@DATE@", date);
	tpl.replace("@PLACE@", db.cs()->campPlace);
	tpl.replace("@ORG@", db.cs()->campOrg);
	tpl.replace("@QUATER@", db.cs()->campQuarter);
	tpl.replace("@OTHER@", db.cs()->campOthers);
	tpl.replace("@LEADER@", db.cs()->campLeader);
	tpl.replace("@SCOUTSNO@", QString().sprintf("%d", sco));
	tpl.replace("@LEADERSNO@", QString().sprintf("%d", lea));
	tpl.replace("@OTHERSNO@", QString().sprintf("%d", oth));
	tpl.replace("@ALL@", QString().sprintf("%d", all));
	tpl.replace("@AVGCOSTS@", QString().sprintf("%.2f", db.cs()->avgCosts));
	tpl.replace("@AVG@", QString().sprintf("%.2f", costs/all));

	tpl.replace("@TABLE_CONTENT_1@", cont0);
	tpl.replace("@TABLE_CONTENT_2@", cont2);
	tpl.replace("@TABLE_CONTENT_3@", cont3);
	tpl.replace("@TABLE_CONTENT_4@", cont4);
	tpl.replace("@TABLE_CONTENT_ADD@", contadd);
	doc.setHtml(tpl);

	doc.print(&printer);
}

void DBReports::printSMReport(QString* reportsdir) {
	QDate b_min, b_max, d_min, d_max;

	QSqlQuery q;
	q.exec("SELECT booking FROM batch ORDER BY booking ASC LIMIT 1;");
	if (q.next())
		b_min = q.value(0).toDate();
	q.exec("SELECT booking FROM batch ORDER BY booking DESC LIMIT 1;");
	if (q.next())
		b_max = q.value(0).toDate();
	q.exec("SELECT distdate FROM distributor ORDER BY distdate ASC LIMIT 1;");
	if (q.next())
		d_min = q.value(0).toDate();
	q.exec("SELECT distdate FROM distributor ORDER BY distdate DESC LIMIT 1;");
	if (q.next())
		d_max = q.value(0).toDate();

	PR(b_min.toString(Qt::DefaultLocaleShortDate).toStdString());
	PR(b_max.toString(Qt::DefaultLocaleShortDate).toStdString());
	PR(d_min.toString(Qt::DefaultLocaleShortDate).toStdString());
	PR(d_max.toString(Qt::DefaultLocaleShortDate).toStdString());

	QMap<QString, QString> blackbox;

	int totdays;
	if (b_max.daysTo(d_max) > 0)
		totdays = b_min.daysTo(d_max);
	else
		totdays = b_min.daysTo(b_max);

}

#include "DBReports.moc"