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
#include <QBitmap>

#include "globals.h"
#include "config.h"
#include "DBImportWidget.h"
#include "DBExportCommon.h"
#include "DBBrowser.h"
#include "Database.h"

DBImportWidget::DBImportWidget(QWidget *) : Ui::DBImportWidget() {
	setupUi(this);

	connect(rb_current, SIGNAL(toggled(bool)), this, SLOT(targetSelected()));
	connect(rb_existing, SIGNAL(toggled(bool)), this, SLOT(targetSelected()));
	connect(rb_new, SIGNAL(toggled(bool)), this, SLOT(targetSelected()));

	connect(push_file, SIGNAL(clicked(bool)), this, SLOT(selectImportFile()));

	connect(line_file, SIGNAL(textChanged(QString)), this, SLOT(validateForm()));
	connect(cb_dbname, SIGNAL(currentIndexChanged(int)), this, SLOT(validateForm()));
// 	connect(line_new, SIGNAL(editingFinished()), this, SLOT(validateForm()));
	connect(line_new, SIGNAL(textChanged(QString)), this, SLOT(setTargetNew(QString)));

	connect(rb_current, SIGNAL(toggled(bool)), line_current, SLOT(setEnabled(bool)));
	connect(rb_existing, SIGNAL(toggled(bool)), cb_dbname, SLOT(setEnabled(bool)));
	connect(rb_new, SIGNAL(toggled(bool)), line_new, SLOT(setEnabled(bool)));

	connect(push_import, SIGNAL(clicked(bool)), this, SLOT(doImport()));
	connect(push_reset, SIGNAL(clicked(bool)), this, SLOT(resetForm()));
	connect(push_advanced, SIGNAL(toggled(bool)), this, SLOT(setAdvancedMode(bool)));

	connect(chb_info, SIGNAL(toggled(bool)), line_infoname, SLOT(setEnabled(bool)));

	label_hidden->setMargin(6);
// 	setAdvancedMode(true);
	push_advanced->setChecked(true);
}

DBImportWidget::~DBImportWidget() {
	DI();
}

void DBImportWidget::inputFileChanged() {
	bool fileSelected = !line_file->text().isEmpty();

	if (!fileSelected) {
		datafile.clear();
		infofile.clear();
	} else {
		DBExportCommon::UnpackData(line_file->text(), &buff);

		QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_DB));
		if (!dbsavepath.exists())
			dbsavepath.mkpath(dbsavepath.absolutePath());
	}

	push_import->setEnabled(fileSelected);
}

void DBImportWidget::existingDBChanged() {
}

void DBImportWidget::newDBChanged() {
}

void DBImportWidget::targetSelected() {
	if (rb_current->isChecked()) {
	} else
	if (rb_existing->isChecked()) {
	} else
	if (rb_new->isChecked()) {
		if (line_new->text().isEmpty()) {
			QFileInfo dbnamei(qUncompress(buff.dbname));
			QString dbname = dbnamei.baseName();
			line_new->setText(dbname);
		}
	} else {
		return;
	}

	validateForm();
}

void DBImportWidget::validateForm() {
	Database * db = Database::Instance();

	bool final_state = true;

	QString fbase;
	if (rb_current->isChecked()) {
		fbase = db->fileFromDBName(db->openedDatabase(), true, false);
		line_infoname->clear();
	} else
	if (rb_existing->isChecked()) {
		fbase = db->fileFromDBName(cb_dbname->itemData(cb_dbname->currentIndex()).toString(), true, false);
		line_infoname->setText(qUncompress(buff.infodata));PR(buff.infodata.data());
	} else
	if (rb_new->isChecked()) {
		fbase = db->fileFromDBName(line_new->text(), true, false);
		line_infoname->setText(line_new->text());
	} else {
		return;
	}

	datafile = fbase % Database::dbfilext;
	infofile = fbase % Database::infofilext;

	line_dbname->setText(datafile);

	push_import->setEnabled(final_state);
}

void DBImportWidget::resetForm() {
	line_file->clear();

	cb_dbname->clear();
	QVector<DBBrowser::DBListEntry> dble = DBBrowser::fetchDBEntryList(DBBrowser::s_name, DBBrowser::o_asc);
	for (int i = 0; i < dble.size(); ++i) {
		cb_dbname->addItem(dble[i].infoContent, dble[i].fileInfo.baseName());
	}

	line_current->setText(Database::Instance()->openedDatabase());

	int idx = cb_dbname->findText(Database::Instance()->openedDatabase(), Qt::MatchExactly);
	// 	if (idx != -1)
	cb_dbname->setCurrentIndex(idx);

	rb_new->setEnabled(false);
	rb_existing->setEnabled(false);
	rb_current->setEnabled(false);

	cb_dbname->setEnabled(false);
	line_new->setEnabled(false);

	rb_new->setChecked(false);
	rb_existing->setChecked(false);
	rb_current->setChecked(false);

	chb_info->setEnabled(false);

	push_import->setEnabled(false);

	initial = true;
}

void DBImportWidget::doImport() {
	PR(datafile.toStdString());

	int ret = QMessageBox::question(this, tr("Database import dialog"), QObject::tr("This action will overwrite all existing data. Do you want continue?"), QMessageBox::Yes | QMessageBox::No);
	if (ret != QMessageBox::Yes)
		return;

	if (chb_info->isChecked())
		DBExportCommon::ImportFile(datafile, infofile, &buff);
	else
		DBExportCommon::ImportFile(datafile, "", &buff);

	Database::Instance()->rebuild_models();

	QMessageBox::information(this, tr("Database import dialog"), tr("Database imported"), QMessageBox::Ok);
}

void DBImportWidget::setTargetCurrent() {
	rb_current->setChecked(true);

	validateForm();
}

void DBImportWidget::setTargetDatabase(int idx) {
	if ( (idx >= 0) and (idx < cb_dbname->count()) )
		cb_dbname->setCurrentIndex(idx);

	rb_existing->setChecked(true);
	validateForm();
}

void DBImportWidget::setTargetNew(const QString& newdb) {
	line_new->setText(newdb);
	rb_new->setChecked(true);

	QString fbase = Database::Instance()->fileFromDBName(line_new->text(), true, false) % Database::dbfilext;
	PR(QFile::exists(fbase));
	if (QFile::exists(fbase)) {
		QPixmap pwarn(":/resources/icons/printer.png");// = this->style()->standardPixmap(QStyle::SP_MessageBoxWarning);
		rb_new->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxWarning));
	} else {
		rb_new->setIcon(QIcon());
	}

	validateForm();
}

void DBImportWidget::setAdvancedMode(bool advanced) {
	if (!advanced) {
// 		rb_current->setChecked(true);
	}

// 	rb_current->setVisible(advanced);
	rb_existing->setVisible(advanced);
	cb_dbname->setVisible(advanced);
	rb_new->setVisible(advanced);
	line_new->setVisible(advanced);
}

void DBImportWidget::selectImportFile() {
	QFileDialog dialog(this);
	dialog.setNameFilter(QString("%1 (*%2)").arg(DBExportCommon::exportArchName).arg(DBExportCommon::exportArchExt));

	// 	dialog.setDirectory(exp_line_dir->text());

	if (!dialog.exec())
		return;

// 	QString dir = dialog.getExistingDirectory();
	QString file = dialog.selectedFiles().at(0);

	if (file.isEmpty()) {
		datafile.clear();
		infofile.clear();
	} else {
		DBExportCommon::UnpackData(file, &buff);

		QDir dbsavepath(QDir::homePath() % QString(ZARLOK_HOME ZARLOK_DB));
		if (!dbsavepath.exists())
			dbsavepath.mkpath(dbsavepath.absolutePath());
	}

	if (initial) {
		rb_current->setChecked(true);
		rb_new->setEnabled(true);
		rb_existing->setEnabled(true);
		rb_current->setEnabled(true);
		chb_info->setEnabled(true);
		initial = false;
	}

	line_file->setText(file);
	chb_info->setChecked(buff.has_info);
}

#include "DBImportWidget.moc"