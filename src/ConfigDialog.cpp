// SPDX-License-Identifier: GPL-2.0-or-later
// (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
#include "ConfigDialog.h"
#include <QFileDialog>

ConfigDialog::ConfigDialog(QSettings &settings, QWidget *parent):QDialog(parent),_settings(settings),_layout(this) {
	setModal(true);

	_addressLbl=new QLabel(tr("&Address:"), this);
	_layout.addWidget(_addressLbl, 0, 0);
	QVariant v=settings.value("address");
	_address=new QTextEdit(this);
	_address->setTabChangesFocus(true);
	_address->setAcceptRichText(true);
	_address->insertPlainText(v.isValid() ? v.toString() : "Your Company\nYour name\nYour Road 1\n1234 Your Town\n\u260e +41 12 345 67 89\nhttps://yourcompany.ch/\ninfo@yourcompany.ch\n\nTax ID: CHE-123.456.789\nVAT ID: CHE-123.456.789-MWST\n");
	_address->moveCursor(QTextCursor::Start);
	_addressLbl->setBuddy(_address);
	_layout.addWidget(_address, 0, 1, 1, 2);

	_shortAddressLbl=new QLabel(tr("&Short address:"), this);
	_layout.addWidget(_shortAddressLbl, 1, 0);
	v=settings.value("shortAddress");
	_shortAddress=new QLineEdit(v.isValid() ? v.toString() : "Your Company \u2022 Your Road \u2022 Your Town \u2022 Your Country", this);
	_shortAddressLbl->setBuddy(_shortAddress);
	_layout.addWidget(_shortAddress, 1, 1, 1, 2);

	_logoLbl=new QLabel(tr("&Logo (optional):"), this);
	_layout.addWidget(_logoLbl, 2, 0);
	_logo=new QLineEdit(settings.value("logo").toString(), this);
	_logoLbl->setBuddy(_logo);
	_layout.addWidget(_logo, 2, 1);
	_logoBrowse=new QPushButton(tr("&Browse"), this);
	_layout.addWidget(_logoBrowse, 2, 2);
	connect(_logoBrowse, &QPushButton::clicked, this, &ConfigDialog::browseLogo);

	_termsLbl=new QLabel(tr("&Terms:"), this);
	_layout.addWidget(_termsLbl, 3, 0);
	v=settings.value("terms");
	_terms=new QTextEdit(this);
	_terms->setTabChangesFocus(true);
	_terms->setAcceptRichText(true);
	_terms->insertPlainText(v.isValid() ? v.toString() : tr("Unless agreed upon otherwise in writing, this invoice is to be paid in full by wire transfer to the account given below within 30 days.\nThank you for your business!"));
	_termsLbl->setBuddy(_terms);
	_layout.addWidget(_terms, 3, 1, 1, 2);

	_bankInfoLbl=new QLabel(tr("Ban&k info:"), this);
	_layout.addWidget(_bankInfoLbl, 4, 0);
	v=settings.value("bankInfo");
	_bankInfo=new QLineEdit(v.isValid() ? v.toString() : tr("Your name \u2022 IBAN CHxx xxxx xxxx xxxx xxxx x \u2022 Your bank \u2022 SWIFT-BIC: XXX \u2022 Clearing number: XXXXX"), this);
	_bankInfoLbl->setBuddy(_bankInfo);
	_layout.addWidget(_bankInfo, 4, 1, 1, 2);

	_defaultRecipientLbl=new QLabel(tr("Default &recipient:"), this);
	_layout.addWidget(_defaultRecipientLbl, 5, 0);
	v=settings.value("defaultRecipient");
	_defaultRecipient=new QTextEdit(this);
	_defaultRecipient->setTabChangesFocus(true);
	_defaultRecipient->setAcceptRichText(true);
	_defaultRecipient->insertPlainText(v.toString());
	_defaultRecipient->setPlaceholderText("Your main customer's address");
	_defaultRecipientLbl->setBuddy(_defaultRecipient);
	_layout.addWidget(_defaultRecipient, 5, 1, 1, 2);

	_ok=new QPushButton(tr("&OK"), this);
	connect(_ok, &QPushButton::clicked, this, &QDialog::accept);
	_layout.addWidget(_ok, 6, 0);
	_cancel=new QPushButton(tr("&Cancel"), this);
	connect(_cancel, &QPushButton::clicked, this, &QDialog::reject);
	_layout.addWidget(_cancel, 6, 1, 1, 2);
}

void ConfigDialog::browseLogo() {
	QString f=QFileDialog ::getOpenFileName(this, tr("Logo"), QString(), tr("Images (*.png *.jpg *.jxl *.j2k *.gif *.xpm)"));
	if(!f.isEmpty())
		_logo->setText(f);
}
