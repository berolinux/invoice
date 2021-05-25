// SPDX-License-Identifier: GPL-2.0-or-later
// (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QTextEdit>

class ConfigDialog:public QDialog {
	Q_OBJECT
public:
	ConfigDialog(QSettings &settings, QWidget *parent=nullptr);
	QString address() const { return _address->toPlainText(); }
	QString shortAddress() const { return _shortAddress->text(); }
	QString logo() const { return _logo->text(); }
	QString terms() const { return _terms->toPlainText(); }
	QString bankInfo() const { return _bankInfo->text(); }
	QString defaultRecipient() const { return _defaultRecipient->toPlainText(); }
public slots:
	void browseLogo();
private:
	QSettings&	_settings;
	QGridLayout	_layout;
	QLabel*		_addressLbl;
	QTextEdit*	_address;
	QLabel*		_shortAddressLbl;
	QLineEdit*	_shortAddress;
	QLabel*		_logoLbl;
	QLineEdit*	_logo;
	QPushButton*	_logoBrowse;
	QLabel*		_termsLbl;
	QTextEdit*	_terms;
	QLabel*		_bankInfoLbl;
	QLineEdit*	_bankInfo;
	QLabel*		_defaultRecipientLbl;
	QTextEdit*	_defaultRecipient;
	QPushButton*	_ok;
	QPushButton*	_cancel;
};
