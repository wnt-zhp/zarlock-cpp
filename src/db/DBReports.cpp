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

#include <cstdio>
#include <cmath>

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
#include <qsqlrecord.h>
#include <QTextCodec>

#include "DBReports.h"

#include "globals.h"
#include "config.h"

#include "DataParser.h"
#include "Database.h"

#include "ProgramSettings.h"

#include "CampProperties.h"
#include "BatchTableModel.h"
#include "DistributorTableModel.h"

struct KMDB_entry {
	int bid;
	QString invoice;
	QDate date;
	int qty;
	int used_qty;
	void init(int id, const QString & inv, const QDate & edate, int q, int uq) {
		bid = id;
		invoice = inv;
		date = edate;
		qty = q;
		used_qty = uq;
	}
};

struct KMDB {
	int id;
	QString name;
	QString spec;
	QString unit;
	QString price_s;
	int price;
	QVector<KMDB_entry> batches;
};

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

	Database * db = Database::Instance();
	int p_rows = db->CachedBatch()->rowCount();
	int p_cols = 4;//model_batch->columnCount();
	int batch_map[4] = { 0, 2, 4, 3 };
	QString tcont;

	for (int i = 0; i < p_rows; i++) {
		tcont += "<tr>";
		for (int j = 0; j < p_cols; j++) {
// 			QTextCursor cur = products_table->cellAt(i, j).firstCursorPosition();
// 			cur.insertText(model_prod->index(i, j).data().toString());
			tcont += "<td>" + db->CachedBatch()->index(i, batch_map[j]).data().toString() + "</td>";
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

void DBReports::printReport11A(const QString& date, QString * reportfile) {
	QFile dailymeal_tpl(":/resources/report_dailymeal.tpl");
	if (!dailymeal_tpl.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, QObject::tr("Cannot find resources"),
			QObject::tr("Unable to find resources in ") +
						dailymeal_tpl.fileName() + "\n" +
						QObject::tr("Check your installation and try to run again.\n"
						"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QFile sheet_css(":/resources/report_dailymeal.css");
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

	dailymeal_tstream.setRealNumberNotation(QTextStream::FixedNotation);
	dailymeal_tstream.setRealNumberPrecision(2);
	dailymeal_tstream.setLocale(QLocale(QLocale::C));

	dailymeal_tstream.setCodec(QTextCodec::codecForName("UTF-8"));

	// Prepare printer
	QPrinter printer(QPrinter::HighResolution);
	printer.setPaperSize(QPrinter::A4);
	printer.setColorMode(QPrinter::Color);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOrientation(QPrinter::Landscape);
	printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

	Database * db = Database::Instance();

	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % db->openedDatabase());
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	QString ofile = dbsavepath.absolutePath() % QString("/") % QString("Form11a_") % date % QString(".pdf");
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
	QSqlQuery q, q2, q3;

// 	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
// 		QSqlDatabase::database().transaction();

	q.prepare("SELECT id, avcosts FROM meal_day WHERE mealdate=?;");
	q.bindValue(0, date);
	if (!q.exec())
		PR(q.lastError().databaseText().toStdString());

	if (!q.next())
		return;

	int costs = q.value(1).toInt();

	q2.prepare("SELECT id, mealkind, name, scouts, leaders, others, cost FROM meal WHERE mealday=? ORDER BY mealkind ASC;");
	q2.bindValue(0, q.value(0));
	if (!q2.exec())
		PR(q2.lastError().databaseText().toStdString());

	int id/*, kind*/;
	QString name;
	int sco = 0, lea = 0, oth = 0;
	int all_sco = 0, all_lea = 0, all_oth = 0;

	QVector<QString> headers;
	QVector<QString> contents;

	const QString header_tmp = "<td width=\"auto\">%1 | %2/%3/%4</td>";

	BatchTableModel * btm = db->CachedBatch();

	while (q2.next()) {
		id = q2.value(0).toInt();
// 		kind = q2.value(1).toInt();
		name = q2.value(2).toString();
		sco = q2.value(3).toInt();
		lea = q2.value(4).toInt();
		oth = q2.value(5).toInt();

		all_sco += sco;
		all_lea += lea;
		all_oth += oth;

		QString h = header_tmp.arg(name).arg(sco).arg(lea).arg(oth);
		headers.push_back(h);

		QString content;

		q3.prepare("SELECT id, batch_id, quantity FROM distributor WHERE disttype=2 AND disttype_a=? ORDER BY id ASC;");
		q3.bindValue(0, id);
		q3.exec();

		while (q3.next()) {
			int batch_id = q3.value(1).toInt();
			QString spec = btm->index(btm->getRowById(batch_id), BatchTableModel::HSpec).data().toString();
			double qty = q3.value(2).toInt();
			QString unit = btm->index(btm->getRowById(batch_id), BatchTableModel::HUnit).data().toString();

			if (!content.isEmpty())
				content = content % "<br />";
			content =  content % QString("%1 - %2 x %3").arg(spec).arg(qty/100).arg(unit);
		}

// 		if (q.driver()->hasFeature(QSqlDriver::Transactions))
// 			if (!QSqlDatabase::database().commit())
// 				QSqlDatabase::database().rollback();

		contents.push_back("<td>" % content % "</td>");
	}

	QString tpl = dailymeal_tstream.readAll();
	tpl.replace("@DATE@", date);
	tpl.replace("@PLACE@", db->cs()->campPlace);
	tpl.replace("@ORG@", db->cs()->campOrg);
	tpl.replace("@QUATER@", db->cs()->campQuarter);
	tpl.replace("@OTHER@", db->cs()->campOthers);
	tpl.replace("@LEADER@", db->cs()->campLeader);
	tpl.replace("@SCOUTSNO@", QString("%1").arg(all_sco));
	tpl.replace("@LEADERSNO@", QString("%1").arg(all_lea));
	tpl.replace("@OTHERSNO@", QString("%1").arg(all_oth));
	tpl.replace("@ALL@", QString("%1").arg(all_sco + all_lea + all_oth));
	tpl.replace("@AVGCOSTS@", QString("%1.%2").arg(db->cs()->avgCosts/100).arg(db->cs()->avgCosts % 100, 2, 10, QChar('0')));
	tpl.replace("@AVG@", QString("%1.%2").arg(costs/10000).arg(int(round((costs % 10000) / 100.0)), 2, 10, QChar('0')));

	QString h, c;
	for (int i = 0; i < headers.size(); ++i) {
		h = h % headers[i];
		c = c % contents[i];
	}
	tpl.replace("@TABLE_HEADERS@", h);
	tpl.replace("@TABLE_CONTENTS@", c);

	doc.setHtml(tpl);

	doc.print(&printer);
}

void DBReports::printReport13(QString * reportsdir) {
	ProgramSettings * progset = ProgramSettings::Instance();

	QString entry1_tmpl = QString::fromUtf8("@KARTOTEKA MAGAZYNOWA\n@ILOŚCIOWO-WARTOŚCIOWA\n@\n").replace("@", progset->csv_separator);
	QString entry2_tmpl = QObject::tr("@Article name:@%1 %2 %3@price:@%4\n\n").replace("@", progset->csv_separator);
	QString entry3_tmpl = QObject::tr("No.@Date@Serial number@Income@@Outcome@@Balance@\n").replace("@", progset->csv_separator);
	QString entry4_tmpl = QObject::tr("@@@quantity@value@quantity@value@quantity@value\n").replace("@", progset->csv_separator);
	QString entry5_tmpl = QString("1@2@3@4@5@6@7@8@9").replace("@", progset->csv_separator);
	QString entry6_tmpl = QString("\n%1@%2@%3@%4@%5@@@%6@%7").replace("@", progset->csv_separator);
	QString entry7_tmpl = QString("\n%1@%2@%3@@@%4@%5@%6@%7").replace("@", progset->csv_separator);


	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % Database::Instance()->openedDatabase());
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	if (reportsdir)
		*reportsdir = dbsavepath.absolutePath();

// 	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
// 		QSqlDatabase::database().transaction();

	QSqlQuery q, q2;

	QMap<QString, KMDB> kmm;
	q2.prepare("SELECT id, spec, unit, price, start_qty, used_qty, invoice, regdate FROM batch WHERE prod_id=? ORDER BY regdate ASC;");
// 	q3.prepare("SELECT quantity, distdate, reason3 FROM distributor WHERE batch_id IN ( ? ) ORDER BY distdate ASC;");

	q.exec("SELECT id, name FROM products;");
	while (q.next()) {
		int id = q.value(0).toInt();
		QString name = q.value(1).toString().trimmed().toUtf8();
// PR(id);
		q2.bindValue(0, id);
		q2.exec();
		while (q2.next()) {
			int bid = q2.value(0).toInt();
			QString spec = q2.value(1).toString().trimmed().toUtf8();
			QString unit;
			int price = q2.value(3).toInt();
			QString price_s = QObject::tr("%1.%2 zl").arg(price/100).arg(price % 100, 2, 10, QChar('0'));
			int qty = q2.value(4).toInt();
			int uqty = q2.value(5).toInt();
			QString invoice = q2.value(6).toString().trimmed();
			QDate entrydate = q2.value(7).toDate();

			QString bidname = QString().sprintf("%d", id) % "_" %
								name % "_" % spec % "_" % unit % "_" % price_s % "_";
// 			PR(bidname.toStdString());

			if (kmm.contains(bidname)) {
				KMDB & k = kmm[bidname];
				KMDB_entry ke;
				ke.init(bid, invoice, entrydate, qty, uqty);
				k.batches.push_back(ke);
			} else {
				KMDB & k = kmm[bidname];
				k.id = bid;
				k.name = name;
				k.spec = spec;
				k.unit = unit;
				k.price_s = price_s;
				k.price = price;
				KMDB_entry ke;
				ke.init(bid, invoice, entrydate, qty, uqty);
				k.batches.push_back(ke);
			}
		}
	}

// 	if (q.driver()->hasFeature(QSqlDriver::Transactions))
// 		if (!QSqlDatabase::database().commit())
// 			QSqlDatabase::database().rollback();

	for (QMap<QString, KMDB>::iterator kmm_iter = kmm.begin(); kmm_iter != kmm.end(); ++kmm_iter) {
		QString ofile = dbsavepath.absolutePath() % QString("/") % "Form13_" % QString().sprintf("%03d", kmm_iter->id) % ".csv";

		QFile file(ofile);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			std::cerr << "Unable to create file " << ofile.toStdString() << std::endl;
			continue;
		}

		QString outs;

// 		std::cout << "=================== " << ofile.toStdString() << std::endl;

		QString qrange("-1");
		for (int ii = 0; ii < kmm_iter->batches.size(); ++ii) {
			qrange = qrange % ", " % QString::number(kmm_iter->batches[ii].bid);
		}

		QString qqq = QString("SELECT batch_id, quantity, distdate, disttype, disttype_a, disttype_b FROM distributor WHERE batch_id IN ( %1 ) ORDER BY distdate ASC;").arg(qrange);
		QSqlQuery q3(qqq);
		q3.exec();

		int kmm_iteridx = 0;	// iterator index
		int gidx = 1;			// position index

		int bid = -1;
		int qty = 0;
		QDate dd = QDate::fromString("9999-99-99", Qt::ISODate);
		int disttype = 0;
		QVariant disttype_a, disttype_b;

		int tot_qty = 0;
		int tot_price = 0;

		bool bisempty = false;
		bool disempty = false;

		outs =
				entry1_tmpl %
				entry2_tmpl
					.arg(QString::fromUtf8(kmm_iter->name.trimmed().toStdString().c_str()))
					.arg(QString::fromUtf8(kmm_iter->spec.trimmed().toStdString().c_str()))
					.arg(QString::fromUtf8(kmm_iter->unit.trimmed().toStdString().c_str()))
					.arg(kmm_iter->price_s.toStdString().c_str()) %
				entry3_tmpl %
				entry4_tmpl %
				entry5_tmpl;

		// browse over all batch and distributor entries (sorted by time)
		while (!bisempty or !disempty) {/*PR(kmm_iter->id);PR(bisempty);PR(disempty);*/
			// get distribution
			if (q3.next()) {
				bid = q3.value(0).toInt();
				qty = q3.value(1).toInt();
				dd = q3.value(2).toDate();
				disttype = q3.value(3).toInt();
				disttype_a = q3.value(4).toInt();
				disttype_b = q3.value(5).toString();
			} else {
				// if no more distributions mark as empty
				disempty = true;
			}

			// iterate over batches IF
			// there are some batches AND
			// batch is earlier or equal than last distributor OR there is no more distributions
			// then print batch
			while (!bisempty and ( kmm_iter->batches[kmm_iteridx].date <= dd or disempty )) {
				tot_qty += kmm_iter->batches[kmm_iteridx].qty;
				tot_price += kmm_iter->batches[kmm_iteridx].qty * kmm_iter->price;

				QString entry = entry6_tmpl
					.arg(gidx)
					.arg(kmm_iter->batches[kmm_iteridx].date.toString(Qt::ISODate).toStdString().c_str())
					.arg(QString::fromUtf8(kmm_iter->batches[kmm_iteridx].invoice.toStdString().c_str()))
					.arg(double(kmm_iter->batches[kmm_iteridx].qty)/100)
					.arg(double(kmm_iter->batches[kmm_iteridx].qty * kmm_iter->price)/10000)
					.arg(double(tot_qty)/100)
					.arg(double(tot_price)/10000);

				outs.append(entry);
				++gidx;

				// if no more batches mark as empty
				if (++kmm_iteridx == kmm_iter->batches.size()) {
					bisempty = true;
					break;
				}
			}

			// else if there are still some distributions
			if (!disempty) {
				// if there is distribution but not batch yet
				// then we have problem
				if (kmm_iteridx == 0) {
					std::cerr << "Problem with KM for **" << kmm_iter.key().toStdString() << "**\t\tand Batch ID **" << bid << "**\n";
				}

				// print distributions
				QString reas;
				switch (disttype) {
					case 0:
						reas = QString(disttype_a.toString() % " " % disttype_b.toString()).trimmed();
						break;
					case 2:
						QString qs4 = QString("SELECT name FROM meal WHERE id=%1;").arg(disttype_a.toInt());
						QSqlQuery q4(qs4);
						q4.exec();

						if (q4.next()) {
							reas = QString("Wydanie na %1").arg(q4.value(0).toString());
						}
						break;
				}

				tot_qty -= qty;
				tot_price -= qty * kmm_iter->price;

				QString entry = entry7_tmpl
					.arg(gidx)
					.arg(dd.toString(Qt::ISODate).toStdString().c_str())
					.arg(QString::fromUtf8(reas.toStdString().c_str()))
					.arg(double(qty)/100)
					.arg(double(qty * kmm_iter->price)/10000)
					.arg(double(tot_qty)/100)
					.arg(double(tot_price)/10000);

				outs.append(entry);
				++gidx;
			}
		}

		QTextStream out(&file);
		out.setRealNumberNotation(QTextStream::FixedNotation);
		out.setRealNumberPrecision(2);
		out.setLocale(QLocale(QLocale::C));

		out.setCodec(QTextCodec::codecForName(progset->csv_encoding.toUtf8()));

		out << outs;
	}
}

void DBReports::printReport13A(const QString& date, QString * reportfile) {
	QFile dailymeal_tpl(":/resources/report_dailymeal.tpl");
	if (!dailymeal_tpl.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::critical(0, QObject::tr("Cannot find resources"),
							  QObject::tr("Unable to find resources in ") +
		dailymeal_tpl.fileName() + "\n" +
		QObject::tr("Check your installation and try to run again.\n"
		"Click Close to exit."), QMessageBox::Close);
		exit(EXIT_FAILURE);
	}

	QFile sheet_css(":/resources/report_dailymeal.css");
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

	dailymeal_tstream.setRealNumberNotation(QTextStream::FixedNotation);
	dailymeal_tstream.setRealNumberPrecision(2);
	dailymeal_tstream.setLocale(QLocale(QLocale::C));

	dailymeal_tstream.setCodec(QTextCodec::codecForName("UTF-8"));

	// Prepare printer
	QPrinter printer(QPrinter::HighResolution);
	printer.setPaperSize(QPrinter::A4);
	printer.setColorMode(QPrinter::Color);
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setOrientation(QPrinter::Landscape);
	printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

	Database * db = Database::Instance();

	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % db->openedDatabase());
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	QString ofile = dbsavepath.absolutePath() % QString("/") % QString("Form13a_") % date % QString(".pdf");
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
		QSqlQuery q, q2, q3;

		// 	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
		// 		QSqlDatabase::database().transaction();

		q.prepare("SELECT id, avcosts FROM meal_day WHERE mealdate=?;");
		q.bindValue(0, date);
		if (!q.exec())
			PR(q.lastError().databaseText().toStdString());

		if (!q.next())
			return;

		int costs = q.value(1).toInt();

		q2.prepare("SELECT id, mealkind, name, scouts, leaders, others, cost FROM meal WHERE mealday=? ORDER BY mealkind ASC;");
		q2.bindValue(0, q.value(0));
		if (!q2.exec())
			PR(q2.lastError().databaseText().toStdString());

		int id/*, kind*/;
		QString name;
		int sco = 0, lea = 0, oth = 0;
		int all_sco = 0, all_lea = 0, all_oth = 0;

		QVector<QString> headers;
		QVector<QString> contents;

		const QString header_tmp = "<td width=\"auto\">%1 | %2/%3/%4</td>";

		BatchTableModel * btm = db->CachedBatch();

		while (q2.next()) {
			id = q2.value(0).toInt();
			// 		kind = q2.value(1).toInt();
			name = q2.value(2).toString();
			sco = q2.value(3).toInt();
			lea = q2.value(4).toInt();
			oth = q2.value(5).toInt();
	
			all_sco += sco;
			all_lea += lea;
			all_oth += oth;
	
			QString h = header_tmp.arg(name).arg(sco).arg(lea).arg(oth);
			headers.push_back(h);
	
			QString content;
	
			q3.prepare("SELECT id, batch_id, quantity FROM distributor WHERE disttype=2 AND disttype_a=? ORDER BY id ASC;");
			q3.bindValue(0, id);
			q3.exec();
	
			while (q3.next()) {
				int batch_id = q3.value(1).toInt();
				QString spec = btm->index(btm->getRowById(batch_id), BatchTableModel::HSpec).data().toString();
				int qty = q3.value(2).toInt();
				QString unit = btm->index(btm->getRowById(batch_id), BatchTableModel::HUnit).data().toString();
				int price = btm->index(btm->getRowById(batch_id), BatchTableModel::HPrice).data(Qt::EditRole).toInt();
				QString price_s = btm->index(btm->getRowById(batch_id), BatchTableModel::HPrice).data().toString();
		
				if (!content.isEmpty())
					content = content % "<br />";
				content = content % QString("%1").arg(spec);
				content = content % QString("<span style=\"margin: 0 auto;\">,&nbsp;%1,&nbsp;%2.%3&nbsp;zl,&nbsp;%4, &nbsp;%5.%6&nbsp;zl</span>")
					.arg(unit)
					.arg(qty/100).arg(qty % 100, 2, 10, QChar('0'))
					.arg(price_s)
					.arg(qty*price/10000).arg(int(round(((qty*price) % 10000) / 100.0)), 2, 10, QChar('0'))
					;
			}
			contents.push_back("<td>" % content % "</td>");
		}

		QString tpl = dailymeal_tstream.readAll();
		tpl.replace("@DATE@", date);
		tpl.replace("@PLACE@", db->cs()->campPlace);
		tpl.replace("@ORG@", db->cs()->campOrg);
		tpl.replace("@QUATER@", db->cs()->campQuarter);
		tpl.replace("@OTHER@", db->cs()->campOthers);
		tpl.replace("@LEADER@", db->cs()->campLeader);
		tpl.replace("@SCOUTSNO@", QString("%1").arg(all_sco));
		tpl.replace("@LEADERSNO@", QString("%1").arg(all_lea));
		tpl.replace("@OTHERSNO@", QString("%1").arg(all_oth));
		tpl.replace("@ALL@", QString("%1").arg(all_sco + all_lea + all_oth));
		tpl.replace("@AVGCOSTS@", QString("%1.%2").arg(db->cs()->avgCosts/100).arg(db->cs()->avgCosts % 100, 2, 10, QChar('0')));
		tpl.replace("@AVG@", QString("%1.%2").arg(costs/10000).arg(int(round((costs % 10000) / 100.0)), 2, 10, QChar('0')));

		QString h, c;
		for (int i = 0; i < headers.size(); ++i) {
			h = h % headers[i];
			c = c % contents[i];
		}
		tpl.replace("@TABLE_HEADERS@", h);
		tpl.replace("@TABLE_CONTENTS@", c);

		doc.setHtml(tpl);

		doc.print(&printer);
}

void DBReports::printSMReport(QString * reportsdir) {
	ProgramSettings * progset = ProgramSettings::Instance();

	QString entry1_tmpl = QString::fromUtf8("Stan magazynów na dzień@%1\n\n").replace("@", progset->csv_separator);
	QString entry2_tmpl = QString("%1@%2@%3\n").replace("@", progset->csv_separator);

	QDate b_min, b_max, d_min, d_max;

	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % Database::Instance()->openedDatabase());
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	if (reportsdir)
		*reportsdir = dbsavepath.absolutePath();

	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
		QSqlDatabase::database().transaction();

	QSqlQuery q, q2;

	// create product names list
	int pnum = 0;
	q.exec("SELECT MAX(id) FROM products;");
	if (q.next())
		pnum = q.value(0).toInt()+1;
	else
		pnum = 0;

	QVector<QString> pnames(pnum);

	q.exec("SELECT id, name FROM products;");
	while (q.next()) {
		pnames[q.value(0).toInt()] = q.value(1).toString().toUtf8();
	}

	// create batches names list
	int bnum = 0;
	q.exec("SELECT MAX(id) FROM batch;");
	if (q.next())
		bnum = q.value(0).toInt()+1;
	else
		bnum = 0;

	QMap<QString, int> ubatches;
	QVector<QString> bidnames;
	QVector<QString> bnames;
	QVector<QString> bunits;
	QVector<int> bids(bnum);

	q.exec("SELECT prod_id, spec, unit, start_qty, id FROM batch;");
	while (q.next()) {
		QString bname = pnames[q.value(0).toInt()].trimmed() % " " % q.value(1).toString().toUtf8().trimmed();

		QString bunit;
		int bqty;

		DataParser::unit(q.value(2).toString(), bunit);
		DataParser::quantity(q.value(3).toString(), bqty);

		QString bidname = "_" % pnames[q.value(0).toInt()].trimmed() % "_" % q.value(1).toString().toUtf8().trimmed() % "_" % bunit % "_";
		if (!ubatches.contains(bidname)) {
			int bid = bidnames.size();
	
			bidnames.push_back(bidname);
			bnames.push_back(bname);
			bunits.push_back(bunit);
	
			ubatches.insert(bidname, bid);
			bids[q.value(4).toInt()] = bid;
		} else {
			bids[q.value(4).toInt()] = ubatches.value(bidname);
		}
	}
	bnum = bidnames.count();

	QMap<QString, int> batches_in_stock;
	QVector<int> batches_in_stock_num(bnum);
	QMap<QString, int> batches_new;
	QVector<int> batches_new_num(bnum);
	QMap<QString, int> batches_removed;
	QVector<int> batches_removed_num(bnum);
	QMap<QString, int> batches_bilans;
	QVector<int> batches_bilans_num(bnum);

	// calculate number of days to proceed
	q.exec("SELECT regdate FROM batch ORDER BY regdate ASC LIMIT 1;");
	if (q.next())
		b_min = q.value(0).toDate();
	q.exec("SELECT regdate FROM batch ORDER BY regdate DESC LIMIT 1;");
	if (q.next())
		b_max = q.value(0).toDate();
	q.exec("SELECT distdate FROM distributor ORDER BY distdate ASC LIMIT 1;");
	if (q.next())
		d_min = q.value(0).toDate();
	q.exec("SELECT distdate FROM distributor ORDER BY distdate DESC LIMIT 1;");
	if (q.next())
		d_max = q.value(0).toDate();

	// 	PR(b_min.toString(Qt::DefaultLocaleShortDate).toStdString());
		// 	PR(b_max.toString(Qt::DefaultLocaleShortDate).toStdString());
		// 	PR(d_min.toString(Qt::DefaultLocaleShortDate).toStdString());
		// 	PR(d_max.toString(Qt::DefaultLocaleShortDate).toStdString());

		int totdays;
		if (b_max.daysTo(d_max) > 0)
			totdays = b_min.daysTo(d_max);
		else
			totdays = b_min.daysTo(b_max);

		QString bidname, bunit;
		int bqty;

		q.prepare("SELECT * FROM batch WHERE regdate=?;");
		q2.prepare("SELECT * FROM distributor WHERE distdate=?;");
		// return;
		//##############################################
		for (int i = 0; i < /*b_min.daysTo(d_min)*/(totdays+1); ++i) {
			QString cdate = b_min.addDays(i).toString(Qt::ISODate);
	
			QString ofile = dbsavepath.absolutePath() % QString("/") % "SM_" % cdate % ".csv";
	
			QFile file(ofile);
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
				return;
	
			QString outs;
			outs = entry1_tmpl
			.arg(QString::fromUtf8(b_min.addDays(i).toString(Qt::DefaultLocaleLongDate).toUtf8()));
	
			batches_new.clear();
			batches_removed.clear();
			batches_bilans.clear();
	
			//*************************************************************
			// in stock
			// 		std::printf("## batches in database for day %s\n", cdate.toStdString().c_str());
			// 		out << "## batches in database for day " << cdate.toStdString().c_str() << "\n";
			// 		for (QMap<QString, int>::iterator it = batches_in_stock.begin(); it != batches_in_stock.end(); ++it) {
				// 			int idx = it.value();
			// 			if (batches_in_stock_num[idx] > 0) {
				// 				std::printf("--S %s (%s) => %.2f\n", bnames[idx].toStdString().c_str(), bunits[idx].toStdString().c_str(), batches_in_stock_num[idx]);
			// 				out << "--S " << bnames[idx] << " " << bunits[idx].toStdString().c_str() << " " << batches_in_stock_num[idx] << endl;
			// 			}
			// 		}
	
			//*************************************************************
			// new inserions
			q.bindValue(0, cdate);
			q.exec();
	
			while (q.next()) {
				DataParser::unit(q.value(BatchTableModel::HUnit).toString(), bunit);
				// 			DataParser::quantity(q.value(BatchTableModel::HStaQty).toString(), bqty);
				bqty = q.value(BatchTableModel::HStaQty).toInt();
		
				bidname = "_" % pnames[q.value(BatchTableModel::HProdId).toInt()].trimmed() %
				"_" % q.value(BatchTableModel::HSpec).toString().toUtf8().trimmed() % "_" % bunit % "_";
		
				int cbid = -1;
		
				if (ubatches.contains(bidname)) {
					cbid = ubatches.value(bidname);
				} else {
					continue;
				}
		
				if (!batches_in_stock.contains(bidname)) {
					batches_in_stock.insert(bidname, cbid);
				} else {
				}
		
				if (!batches_new.contains(bidname)) {
					batches_new.insert(bidname, cbid);
				} else {
				}
		
				batches_new_num[cbid] = batches_new_num[cbid] + bqty;
			}
	
			// 		std::printf("## batches inserted for day %s\n", cdate.toStdString().c_str());
			// 		out << "## batches inserted for day " << cdate.toStdString().c_str() << "\n";
			// 		for (QMap<QString, int>::iterator it = batches_new.begin(); it != batches_new.end(); ++it) {
				// 			int idx = it.value();
				// 			std::printf("--A %s (%s) => %.2f\n", bnames[idx].toStdString().c_str(), bunits[idx].toStdString().c_str(), batches_new_num[idx]);
				// 			out << "--A " << bnames[idx].toStdString().c_str() << " " << bunits[idx].toStdString().c_str() << " " << batches_new_num[idx] << endl;
				// 		}
		
				//*************************************************************
				// removals
				q2.bindValue(0, cdate);
				q2.exec();
		
				while (q2.next()) {
					int cbid = bids[q2.value(DistributorTableModel::HBatchId).toInt()];
					// 			DataParser::quantity(q2.value(DistributorTableModel::HQty).toString(), bqty);
					bqty = q2.value(DistributorTableModel::HQty).toInt();
			
					bidname = bidnames[cbid];
			
					if (!batches_removed.contains(bidname)) {
						batches_removed.insert(bidname, cbid);
					} else {
					}
			
					batches_removed_num[cbid] += bqty;
				}
		
				// 		std::printf("## batches removed for day %s\n", cdate.toStdString().c_str());
				// 		out << "## batches removed for day " << cdate.toStdString().c_str() << "\n";
				// 		for (QMap<QString, int>::iterator it = batches_removed.begin(); it != batches_removed.end(); ++it) {
					// 			int idx = it.value();
					// 			std::printf("--R %s (%s) => -%.2f\n", bnames[idx].toStdString().c_str(), bunits[idx].toStdString().c_str(), batches_removed_num[idx]);
					// 			out << "--R " << bnames[idx].toStdString().c_str() << " " << bunits[idx].toStdString().c_str() << " " << batches_removed_num[idx] << endl;
					// 		}
			
					addVectors(batches_in_stock_num, batches_new_num);
					batches_new_num.fill(0);
					subVectors(batches_in_stock_num, batches_removed_num);
					batches_removed_num.fill(0);
			
					// 		std::printf("## batches bilans for day %s\n", cdate.toStdString().c_str());
					// 		out << "## batches bilans for day " << cdate.toStdString().c_str() << "\n";
					for (QMap<QString, int>::iterator kmm_iter = batches_in_stock.begin(); kmm_iter != batches_in_stock.end(); ++kmm_iter) {
						int idx = kmm_iter.value();
						if (batches_in_stock_num[idx] > 0) {
							// 				std::printf("--B %3d, %s (%s) => %.2f\n", idx, bnames[idx].toStdString().c_str(), bunits[idx].toStdString().c_str(), batches_in_stock_num[idx]/100);
							QString entry = entry2_tmpl
							.arg(QString::fromUtf8(bnames[idx].toStdString().c_str()))
							.arg(QString::fromUtf8(bunits[idx].toStdString().c_str()))
							.arg(QString("%1.%2").arg(batches_in_stock_num[idx]/100).arg(batches_in_stock_num[idx] % 100, 2, 10, QChar('0')));
							outs.append(entry);
						}
					}
			
					QTextStream out(&file);
					out.setRealNumberNotation(QTextStream::FixedNotation);
					out.setRealNumberPrecision(2);
					out.setLocale(QLocale(QLocale::C));
			
					out.setCodec(QTextCodec::codecForName(progset->csv_encoding.toUtf8()));
			
					out << outs;
		}

		if (q.driver()->hasFeature(QSqlDriver::Transactions))
			if (!QSqlDatabase::database().commit())
				QSqlDatabase::database().rollback();
}

void DBReports::addVectors(QVector<int>& target, const QVector<int>& source) {
	int st = target.size();
	int ss = source.size();

	if (st < ss)
		target.resize(ss);

	for (int i = 0; i < ss; ++i)
		target[i] += source.at(i);
}

void DBReports::subVectors(QVector<int>& target, const QVector<int>& source) {
	int st = target.size();
	int ss = source.size();

	if (st < ss)
		target.resize(ss);

	for (int i = 0; i < ss; ++i)
		target[i] -= source.at(i);
}
