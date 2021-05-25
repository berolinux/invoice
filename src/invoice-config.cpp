// SPDX-License-Identifier: GPL-2.0-or-later
// (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
#include <QApplication>
#include <QSettings>

#include "ConfigDialog.h"

#include <iostream>

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	app.setOrganizationName("LinDev");
	app.setOrganizationDomain("lindev.ch");
	app.setApplicationName("invoice");
	app.setApplicationVersion("1.0");
	
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, app.organizationName());

	ConfigDialog dlg(settings, nullptr);
	if(dlg.exec()) {
		settings.setValue("address", dlg.address());
		settings.setValue("shortAddress", dlg.shortAddress());
		settings.setValue("logo", dlg.logo());
		settings.setValue("terms", dlg.terms());
		settings.setValue("bankInfo", dlg.bankInfo());
		settings.setValue("defaultRecipient", dlg.defaultRecipient());
	}
}
