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
#include <QFileDialog>

#include "globals.h"
#include "DBImportWidget.h"
#include "Database.h"
#include "DBExportCommon.h"

DBImportWidget::DBImportWidget(QWidget *) : Ui::DBImportWidget() {
	setupUi(this);

	connect(imp_push_file, SIGNAL(clicked(bool)), this, SLOT(selectImportFile()));
	connect(imp_line_file, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	
	connect(imp_push_import, SIGNAL(clicked(bool)), this, SLOT(doImport()));
	connect(imp_push_reset, SIGNAL(clicked(bool)), this, SLOT(resetForm()));
}

DBImportWidget::~DBImportWidget() {
	DI();
}

void DBImportWidget::validateForm() {
	if (!imp_line_file->text().isEmpty()) {
		DBExportCommon::Uncompress(imp_line_file->text(), &buff);

		imp_line_dbname->setText(qUncompress(buff.dbname));
		imp_line_infoname->setText(qUncompress(buff.infodata));

		imp_chb_info->setChecked(buff.has_info);
	}

	imp_push_import->setEnabled(!imp_line_file->text().isEmpty());
}

void DBImportWidget::resetForm() {
	imp_line_file->clear();
// 	validateForm();
}

void DBImportWidget::doImport() {
	Database * db = Database::Instance();
	QString ofilestr = db->fileFromDBName(db->openedDatabase(), true, true);
	QString onfilestr;

	char import_info = imp_chb_info->isChecked();
	if (buff.has_info and import_info) {
		onfilestr = db->fileFromDBName(db->openedDatabase(), true, false) % ".info";
	}

	DBExportCommon::ImportFile(ofilestr, onfilestr, &buff);

	db->rebuild_models();
}

void DBImportWidget::selectImportFile() {
	QFileDialog dialog(this);
	dialog.setNameFilter(QString("%1 (*%2)").arg(DBExportCommon::exportArchName).arg(DBExportCommon::exportArchExt));
	
	// 	dialog.setDirectory(exp_line_dir->text());
	
	if (!dialog.exec())
		return;
	
	// 	QString dir = dialog.getExistingDirectory();
		QString file = dialog.selectedFiles().at(0);
		
		imp_line_file->setText(file);
}

#include "DBImportWidget.moc"