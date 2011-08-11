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

#include <cstdio>

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

struct KMDB {
	int id;
	QString name;
	QString spec;
	QString unit;
	QString price;
	QVector<double> qty;
	QVector<double> qused;
	QVector<QString> invoice;
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
		cont1.append("%1 - %2 x %3\n").arg(q.value(0).toString()).arg(q.value(2).toDouble()).arg(q.value(1).toString());
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

void DBReports::printKMReport(QString * reportsdir) {
	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % Database::Instance().openedDatabase());
	if (!dbsavepath.exists())
		dbsavepath.mkpath(dbsavepath.absolutePath());

	if (reportsdir)
		*reportsdir = dbsavepath.absolutePath();

	if (QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions))
		QSqlDatabase::database().transaction();

	QSqlQuery q, q2;

	QMap<QString, KMDB> kmm;
	q2.prepare("SELECT spec, unit, price, start_qty, used_qty, invoice_no FROM batch WHERE prod_id=?");

	q.exec("SELECT id, name FROM products;");
	while (q.next()) {
		int id = q.value(0).toInt();
		QString name = q.value(1).toString().trimmed().toUtf8();
// PR(id);
		q2.bindValue(0, id);
		q2.exec();
		while (q2.next()) {
			QString spec = q2.value(0).toString().trimmed().toUtf8();
			QString unit;
			DataParser::unit(q2.value(1).toString().trimmed(), unit);
			QString price;
			DataParser::price(q2.value(2).toString().trimmed(), price);
			double qty = q2.value(3).toDouble();
			double uqty = q2.value(4).toDouble();
			QString invoice = q2.value(5).toString().trimmed();

			QString bidname = QString().sprintf("%d", id) % "_" %
								name % "_" % spec % "_" % unit % "_" % price % "_";
// 			PR(bidname.toStdString());

			if (kmm.contains(bidname)) {
				KMDB & k = kmm[bidname];
				k.invoice.push_back(invoice);
				k.qty.push_back(qty);
				k.qused.push_back(uqty);
			} else {
				KMDB & k = kmm[bidname];
				k.id = id;
				k.name = name;
				k.spec = spec;
				k.unit = unit;
				k.price = price;
				k.invoice.push_back(invoice);
				k.qty.push_back(qty);
				k.qused.push_back(uqty);
			}
		}
	}

	if (q.driver()->hasFeature(QSqlDriver::Transactions))
		if (!QSqlDatabase::database().commit())
			QSqlDatabase::database().rollback();

	for (QMap<QString, KMDB>::iterator it = kmm.begin(); it != kmm.end(); ++it) {
		QString ofile = dbsavepath.absolutePath() % QString("/") % "KM_" % QString().sprintf("%03d", it->id) % ".csv";

		QFile file(ofile);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			std::cerr << "Unable to create file " << ofile.toStdString() << std::endl;
			continue;
		}

		QTextStream out(&file);
		out.setRealNumberNotation(QTextStream::FixedNotation);
		out.setRealNumberPrecision(2);

		std::cout << "===================\n";
		std::printf("%s;%s;%s;%s;%s\n", QObject::tr("ID").toStdString().c_str(), QObject::tr("Name").toStdString().c_str(),
					QObject::tr("Spec").toStdString().c_str(), QObject::tr("Unit").toStdString().c_str(),
					QObject::tr("Price").toStdString().c_str());
		out << QObject::tr("ID").toStdString().c_str() << ";" << QObject::tr("Name").toStdString().c_str() << ";" << 
					QObject::tr("Spec").toStdString().c_str() << ";" << QObject::tr("Unit").toStdString().c_str() << ";" << 
					QObject::tr("Price").toStdString().c_str() << endl;
		std::printf("%03d;%s;%s;%s;%s\n", it->id, it->name.toStdString().c_str(), it->spec.toStdString().c_str(),
					it->unit.toStdString().c_str(), it->price.toStdString().c_str());
		out << it->id << ";" << it->name.toStdString().c_str() << ";" << it->spec.toStdString().c_str() << ";" << 
					it->unit.toStdString().c_str() << ";" << it->price.toStdString().c_str() << endl;
		std::printf("%s;%s;%s;%s\n", QObject::tr("Invoice").toStdString().c_str(), QObject::tr("Quantity").toStdString().c_str(),
					QObject::tr("Distributed").toStdString().c_str(), QObject::tr("Bilans").toStdString().c_str());
		out << QObject::tr("Invoice").toStdString().c_str() << ";" << QObject::tr("Quantity").toStdString().c_str() << ";" << 
					QObject::tr("Distributed").toStdString().c_str() << ";" << QObject::tr("Bilans").toStdString().c_str() << endl;
		for (int i = 0; i < it->invoice.size(); ++i) {
			std::printf("%s;%.2f;%.2f;%.2f\n", it->invoice.at(i).toStdString().c_str(),
						it->qty.at(i), it->qused.at(i), it->qty.at(i) - it->qused.at(i));
			out << it->invoice.at(i).toStdString().c_str() << ";" << it->qty.at(i) << ";" << 
						it->qused.at(i) << ";" << it->qty.at(i) - it->qused.at(i) << endl;
		}
	}
}

void DBReports::printSMReport(QString * reportsdir) {
	QDate b_min, b_max, d_min, d_max;

	QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_REPORTS) % Database::Instance().openedDatabase());
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
		double bqty;

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
	QVector<double> batches_in_stock_num(bnum);
	QMap<QString, int> batches_new;
	QVector<double> batches_new_num(bnum);
	QMap<QString, int> batches_removed;
	QVector<double> batches_removed_num(bnum);
	QMap<QString, int> batches_bilans;
	QVector<double> batches_bilans_num(bnum);

	// calculate number of days to proceed
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
	double bqty;

	q.prepare("SELECT * FROM batch WHERE booking=?;");
	q2.prepare("SELECT * FROM distributor WHERE distdate=?;");

	//##############################################
	for (int i = 0; i < /*b_min.daysTo(d_min)*/(totdays+1); ++i) {
		QString cdate = b_min.addDays(i).toString(Qt::ISODate);

		QString ofile = dbsavepath.absolutePath() % QString("/") % "SM_" % cdate % ".csv";

		QFile file(ofile);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		out.setRealNumberNotation(QTextStream::FixedNotation);
		out.setRealNumberPrecision(2);

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
			DataParser::quantity(q.value(BatchTableModel::HStaQty).toString(), bqty);

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
			DataParser::quantity(q2.value(DistributorTableModel::HQty).toString(), bqty);

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
		for (QMap<QString, int>::iterator it = batches_in_stock.begin(); it != batches_in_stock.end(); ++it) {
			int idx = it.value();
			if (batches_in_stock_num[idx] > 0.0) {
// 				std::printf("--B %3d, %s (%s) => %.2f\n", idx, bnames[idx].toStdString().c_str(), bunits[idx].toStdString().c_str(), batches_in_stock_num[idx]);
				out << QString::fromUtf8(bnames[idx].toStdString().c_str()) << ";" << QString::fromUtf8(bunits[idx].toStdString().c_str()) << ";" << batches_in_stock_num[idx] << endl;
			}
		}
	}

	if (q.driver()->hasFeature(QSqlDriver::Transactions))
		if (!QSqlDatabase::database().commit())
			QSqlDatabase::database().rollback();
}

void DBReports::addVectors(QVector< double >& target, const QVector< double >& source) {
	int st = target.size();
	int ss = source.size();

	if (st < ss)
		target.resize(ss);

	for (int i = 0; i < ss; ++i)
		target[i] += source.at(i);
}

void DBReports::subVectors(QVector< double >& target, const QVector< double >& source) {
	int st = target.size();
	int ss = source.size();

	if (st < ss)
		target.resize(ss);

	for (int i = 0; i < ss; ++i)
		target[i] -= source.at(i);
}

#include "DBReports.moc"