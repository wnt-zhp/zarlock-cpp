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

#include <QFileDialog>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QStringBuilder>

#include "globals.h"
#include "DBExportWidget.h"
#include "Database.h"
#include "DBExportCommon.h"
#include "DBBrowser.h"

DBExportWidget::DBExportWidget(QWidget *) : Ui::DBExportWidget(), dircompleter(NULL) {
	setupUi(this);

	connect(cb_dbname, SIGNAL(currentIndexChanged(int)), this, SLOT(validateDBForm(int)));
	connect(chb_current, SIGNAL(stateChanged(int)), this, SLOT(setUseCurrent(int)));

	connect(push_dir, SIGNAL(clicked(bool)), this, SLOT(selectExportDirectory()));
	connect(line_dir, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	connect(line_filename, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));

	connect(push_export, SIGNAL(clicked(bool)), this, SLOT(doExport()));
	connect(push_reset, SIGNAL(clicked(bool)), this, SLOT(resetForm()));
}

DBExportWidget::~DBExportWidget() {
	if (dircompleter)
		delete dircompleter->model();

	delete dircompleter;
}

void DBExportWidget::validateDBForm(int idx) {
	if (idx >= 0) {
		infofile = cb_dbname->itemText(idx);
		datafile = cb_dbname->itemData(idx).toString();
	} else {
		datafile.clear();
		infofile.clear();
	}

	QString expdbfname = infofile % QDateTime::currentDateTime().toString("_yyMMdd_hhmmss");
	line_filename->setText(expdbfname);

	validateForm();
}

void DBExportWidget::validateForm() {
	QString str_dir = line_dir->text();
	// 	if (str_dir.isEmpty())
	str_dir.append("/");

	QString ofilename;
	QString str_fn = line_filename->text();
	if (!str_fn.isEmpty()) {
		ofilename =  str_dir % str_fn % DBExportCommon::exportArchExt;
		push_export->setEnabled(true);
	} else {
		push_export->setEnabled(false);
	}

	QFile file(ofilename);
	line_output->setText(file.fileName());
}

void DBExportWidget::resetForm() {
	chb_info->setChecked(true);

	push_export->setDisabled(true);

	line_dir->setText(QDir::homePath());

	// 	cb_format->setCurrentIndex(1);

	if (dircompleter)
		delete dircompleter->model();
	delete dircompleter;

	dircompleter = new QCompleter(this);
	// 	dircompleter->setMaxVisibleItems(maxVisibleSpinBox->value());
	dircompleter->setMaxVisibleItems(10);
	dircompleter->setCaseSensitivity(Qt::CaseInsensitive);
	dircompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	dircompleter->setWrapAround(true);

	QFileSystemModel * fsmodel = new QFileSystemModel(dircompleter);
	fsmodel->setRootPath(line_dir->text());
	fsmodel->setFilter(QDir::Dirs);

	dircompleter->setModel(fsmodel);

	line_dir->setCompleter(dircompleter);

// 	validateForm();

	cb_dbname->clear();
	QVector<DBBrowser::DBListEntry> dble = DBBrowser::fetchDBEntryList(DBBrowser::s_name, DBBrowser::o_asc);
	for (int i = 0; i < dble.size(); ++i) {
		cb_dbname->addItem(dble[i].infoContent, dble[i].fileInfo.fileName());
	}

	int idx = cb_dbname->findText(Database::Instance()->openedDatabase(), Qt::MatchExactly);
// 	if (idx != -1)
	cb_dbname->setCurrentIndex(idx);
}

void DBExportWidget::doExport() {
	Database * db = Database::Instance();
	QString ifilestr = db->fileFromDBName(db->openedDatabase(), true, true);
	QString infilestr = db->fileFromDBName(db->openedDatabase(), true, false) % ".info";
	QString ofilestr = line_output->text();

	DBExportCommon::DBBuffer buff;
	DBExportCommon::PackData(ifilestr, infilestr, &buff);
	DBExportCommon::ExportFile(ofilestr, &buff);

	QMessageBox::information(this, tr("Database export dialog"), tr("Database exported"), QMessageBox::Ok);
}

void DBExportWidget::selectExportDirectory() {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);

	dialog.setDirectory(line_dir->text());

	if (!dialog.exec())
		return;

// 	QString dir = dialog.getExistingDirectory();
	QString dir = dialog.selectedFiles().at(0);

	line_dir->setText(dir);
}

void DBExportWidget::setUseCurrent(int useCurrent) {
	if (useCurrent) {
		int idx = cb_dbname->findText(Database::Instance()->openedDatabase(), Qt::MatchExactly);
		// 	if (idx != -1)
		cb_dbname->setCurrentIndex(idx);
	}

	cb_dbname->setDisabled(useCurrent);
}

#include "DBExportWidget.moc"