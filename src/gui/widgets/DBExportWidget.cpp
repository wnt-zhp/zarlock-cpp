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

DBExportWidget::DBExportWidget(QWidget *) : Ui::DBExportWidget(), dircompleter(NULL) {
	setupUi(this);

	connect(exp_push_dir, SIGNAL(clicked(bool)), this, SLOT(selectExportDirectory()));
	connect(exp_line_dir, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	connect(exp_line_filename, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	
	connect(exp_push_export, SIGNAL(clicked(bool)), this, SLOT(doExport()));
	connect(exp_push_reset, SIGNAL(clicked(bool)), this, SLOT(resetForm()));
}

DBExportWidget::~DBExportWidget() {
	if (dircompleter)
		delete dircompleter->model();
	
	delete dircompleter;
}


void DBExportWidget::validateForm() {
	QString str_dir = exp_line_dir->text();
	// 	if (str_dir.isEmpty())
	str_dir.append("/");

	QString ofilename;
	QString str_fn = exp_line_filename->text();
	if (!str_fn.isEmpty()) {
		ofilename =  str_dir % str_fn % DBExportCommon::exportArchExt;
		exp_push_export->setEnabled(true);
	} else {
		exp_push_export->setEnabled(false);
	}

	QFile file(ofilename);
	exp_line_output->setText(file.fileName());
}

void DBExportWidget::resetForm() {
	exp_chb_info->setChecked(true);

	exp_push_export->setDisabled(true);

	exp_line_dir->setText(QDir::homePath());

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
	fsmodel->setRootPath(exp_line_dir->text());
	fsmodel->setFilter(QDir::Dirs);

	dircompleter->setModel(fsmodel);

	exp_line_dir->setCompleter(dircompleter);

	QString expdbfname = Database::Instance()->openedDatabase() % QDateTime::currentDateTime().toString("_yyMMdd_hhmmss");

	exp_line_filename->setText(expdbfname);

	validateForm();
}

void DBExportWidget::doExport() {
	Database * db = Database::Instance();
	QString ifilestr = db->fileFromDBName(db->openedDatabase(), true, true);
	QString infilestr = db->fileFromDBName(db->openedDatabase(), true, false) % ".info";
	QString ofilestr = exp_line_output->text();

	DBExportCommon::DBBuffer buff;
	DBExportCommon::Compress(ifilestr, infilestr, &buff);
	DBExportCommon::ExportFile(ofilestr, &buff);
}

void DBExportWidget::selectExportDirectory() {
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::Directory);
	dialog.setOption(QFileDialog::ShowDirsOnly, true);
	
	dialog.setDirectory(exp_line_dir->text());
	
	if (!dialog.exec())
		return;
	
// 	QString dir = dialog.getExistingDirectory();
	QString dir = dialog.selectedFiles().at(0);

	exp_line_dir->setText(dir);
}

#include "DBExportWidget.moc"