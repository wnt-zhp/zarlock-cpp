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

#include "globals.h"
#include "version.h"
#include "AboutDialog.h"

#include <QDialog>

AboutDialog::AboutDialog(QDialog * /*parent*/) : Ui::AboutDialog() {
	setupUi(this);

	label_icon->setPixmap(QPixmap(":/resources/zarlok.png"));
	label_program->setText(tr("Zarlok"));
	label_version->setText(tr("version %1").arg(ZARLOK_VERSION));
	label_authors->setText(tr(
		"Project manager:\tJacek Bzdak <jacek.bzdak@wnt.zhp.pl>\n"
		"Maintainer:\t\tRafal Lalik <rafal.lalik@zhp.net.pl>"
	));
	label_info->setText(tr(
		"The Zarlok is the food resource management application."
	));

	label_wnt_icon->setPixmap(QPixmap(":/resources/images/wnt_logo.png"));
	label_wnt_info->setText(tr(
		"Wydzial Nowych Technologii\n"
		"Glowna Kwatera Zwiazku Harcerstwa Polskiego\n"
	));
	label_wnt_link->setText(tr(
		"Find us <a href=\"%1\">%1</a>"
	).arg("http://zarlok.zhp.pl"));		// "http://wnt.zhp.pl/index.php?id=3&p=17"
	label_wnt_link->setTextFormat(Qt::RichText);

	label_gpl_info->setText(tr(
		"Copyright (C) 2012  Rafal Lalik <rafal.lalik@zhp.net.pl>\n"
		"\n"
		"Zarlok is free software: you can redistribute it and/or modify it "
		"under the terms of the GNU General Public License as published by "
		"the Free Software Foundation, either version 3 of the License, or "
		"(at your option) any later version.\n"
		"\n"
		"Zarlok is distributed in the hope that it will be useful, "
		"but WITHOUT ANY WARRANTY; without even the implied warranty "
		"of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. "
		"See the GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License "
		"along with this program. If not, see <http://www.gnu.org/licenses/>."
	));

	label_gnu_icon->setPixmap(QPixmap(":/resources/images/gnu_logo.png"));
	label_gpl_icon->setPixmap(QPixmap(":/resources/images/gpl_3_logo.png"));
	label_gpl_link->setText(tr(
		"<a href=\"%1\">%1</a>"
	).arg("http://www.gnu.org/licenses/gpl.html"));
}

AboutDialog::~AboutDialog() {
}

#include "AboutDialog.moc"