// SPDX-License-Identifier: GPL-2.0-or-later
// (C) 2022 Bernhard Rosenkränzer <bero@lindev.ch>
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QDate>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QPageSize>
#include <QSettings>

#include <iostream>

class Item {
public:
	Item(QString const &desc, double perU):quantity(1.0),description(desc),perUnit(perU) {}
	Item(double quant, QString const &desc, double perU):quantity(quant),description(desc),perUnit(perU) {}
public:
	double quantity;
	QString description;
	double perUnit;
};

int main(int argc, char **argv) {
	// We need to be a QGuiApplication to work with
	// QPixmap and friends, but let's not bother trying
	// to initialize an X11 or Wayland display in what
	// may well be run from a cron job...
	qputenv("QT_QPA_PLATFORM", "offscreen");

	QGuiApplication app(argc, argv);
	app.setOrganizationName("LinDev");
	app.setOrganizationDomain("lindev.ch");
	app.setApplicationName("invoice");
	app.setApplicationVersion("1.0");
	
	QCommandLineParser cmd;
	cmd.setApplicationDescription("Invoice generator");
	cmd.addHelpOption();
	cmd.addVersionOption();
	cmd.addOptions({
		{{"c", "currency"}, app.translate("main", "Currency"), app.translate("main", "currency"), "€"},
		{{"d", "date"}, app.translate("main", "Invoice date (default: current date)"), app.translate("main", "date"), QDate::currentDate().toString(Qt::ISODate)},
		{{"n", "number"}, app.translate("main", "Invoice number"), app.translate("main", "invoice number"), QDate::currentDate().toString("yyyyMMdd")},
		{{"o", "output"}, app.translate("main", "Output filename"), app.translate("main", "filename"), "invoice.pdf"},
		{{"t", "to"}, app.translate("main", "Postal address of recipient"), app.translate("main", "recipient")}
	});
	cmd.addPositionalArgument("num", app.translate("main", "Number of units"));
	cmd.addPositionalArgument("description", app.translate("main", "Description of item"));
	cmd.addPositionalArgument("price", app.translate("main", "Price per unit"));
	cmd.process(app);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, app.organizationName());
	if(!settings.value("address").isValid()) {
		std::cerr << "No sender address etc. configured." << std::endl
			<< "Edit " << qPrintable(settings.fileName()) << " or use invoice-config" << std::endl;
		return 1;
	}

	QPdfWriter pw(cmd.value("output"));
	pw.setPageLayout(QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF(25, 10, 25, 10), QPageLayout::Millimeter));
	QPainter p(&pw);

	QPixmap const logo(settings.value("logo").toString());

	QDate date=QDate::fromString(cmd.value("date"), Qt::ISODate);
	if(!date.isValid())
		date = QDate::fromString(cmd.value("date"), Qt::RFC2822Date);
	if(!date.isValid())
		date = QDate::fromString(cmd.value("date"), Qt::TextDate);
	if(!date.isValid())
		date = QDate::fromString(cmd.value("date"), "yyyyMMdd");
	if(!date.isValid())
		date = QDate::fromString(cmd.value("date"), "dd.MM.yyyy");
	if(!date.isValid()) {
		std::cerr << "Invalid date given, using current date." << std::endl;
		date = QDate::currentDate();
	}

	QString const sender = settings.value("address").toString();

	QString recipient = cmd.value("to").replace("\\n", "\n");
	if(recipient.isEmpty())
		recipient=settings.value("defaultRecipient").toString();

	QString const company = settings.value("shortAddress").toString();

	QString const invoiceno = cmd.value("number");

	QString const footer = settings.value("terms").toString();

	QString const bankInfo = settings.value("bankInfo").toString();

	QList<Item> items;
	for(int i=0; i<cmd.positionalArguments().count(); i+=2) {
		bool gotNumber;
		double count=cmd.positionalArguments().at(i).toDouble(&gotNumber);
		if(gotNumber)
			i++;
		else
			count = 1;
		items.append(Item(count, cmd.positionalArguments().at(i), cmd.positionalArguments().at(i+1).toDouble()));
	}

	QString const currency = cmd.value("currency");

	auto locale = QLocale("de_CH");

	QFont normalFont(QStringLiteral("Droid Sans"), 8);
	QFont boldFont(QStringLiteral("Droid Sans"), 8, 800);
	QFont smallFont(QStringLiteral("Droid Sans"), 4);
	QFont headingFont(QStringLiteral("Droid Sans"), 12, 800);
	QFont bankFont(QStringLiteral("Droid Sans"), 7);

	p.drawPixmap(QRectF(3700, 0, 2330, 1700), logo, QRectF(0, 0, logo.width(), logo.height()));

	p.setFont(normalFont);
	p.drawText(QRectF(6000, 0, 2000, 1800), Qt::AlignRight, sender);
	p.setFont(smallFont);
	p.drawText(QRectF(0, 1900, 3000, 80), company);
	p.setFont(normalFont);
	p.drawText(QRectF(0, 2100, 3000, 850), recipient);

	p.setFont(headingFont);
	p.drawText(QRectF(0, 3200, 3000, 200), QStringLiteral("INVOICE"));
	p.setFont(normalFont);
	p.drawText(QRectF(6000, 3200, 2000, 280), Qt::AlignRight, QString("%1\nInvoice #%2").arg(date.toString("yyyy-MM-dd")).arg(invoiceno));

	p.drawText(QRectF(0, 3700, 8000, 400), QString("Invoice\n"));

	pw.setTitle(QStringLiteral("LinDev - invoice #") + invoiceno);
	int const startY = 4100;
	int y = startY;
	int i = 1;
	p.drawText(QRectF(0, y, 300, 200), QStringLiteral("Item"));
	p.drawText(QRectF(300, y, 700, 200), QStringLiteral("Quantity"));
	p.drawText(QRectF(1000, y, 5000, 200), QStringLiteral("Description"));
	p.drawText(QRectF(6000, y, 1000, 400), Qt::AlignRight, QStringLiteral("per Unit"));
	p.drawText(QRectF(7000, y, 1000, 400), Qt::AlignRight, QStringLiteral("Price"));
	y += 200;
	double total = 0.0;
	for(auto const &item: items) {
		p.drawText(QRectF(0, y, 300, 400), QString::number(i++));
		p.drawText(QRectF(300, y, 700, 400), QString::number(item.quantity));
		p.drawText(QRectF(1000, y, 5000, 400), item.description);
		p.drawText(QRectF(6000, y, 1000, 400), Qt::AlignRight, locale.toString(item.perUnit, 'f', 2)+currency);
		double totalItem = item.perUnit*item.quantity;
		p.drawText(QRectF(7000, y, 1000, 400), Qt::AlignRight, locale.toString(totalItem, 'f', 2)+currency);
		p.drawLine(0, y, 8000, y);
		y += 400;
		total += totalItem;
	}
	p.drawLine(300, startY, 300, y);
	p.drawLine(1000, startY, 1000, y);
	p.drawLine(6000, startY, 6000, y);
	p.drawText(QRectF(1000, y, 5000, 400), Qt::AlignRight, QStringLiteral("Net amount"));
	p.drawText(QRectF(7000, y, 1000, 400), Qt::AlignRight, locale.toString(total, 'f', 2)+currency);
	p.drawLine(0, y, 8000, y);
	y += 200;
	p.drawText(QRectF(1000, y, 5000, 400), Qt::AlignRight, QStringLiteral("No VAT for services delivered outside of Switzerland"));
	p.drawLine(0, y, 8000, y);
	y += 200;
	p.setFont(boldFont);
	p.drawText(QRectF(1000, y, 5000, 400), Qt::AlignRight, QStringLiteral("Gross amount"));
	p.drawText(QRectF(7000, y, 1000, 400), Qt::AlignRight, locale.toString(total, 'f', 2)+currency);
	p.drawLine(0, y, 8000, y);
	p.setFont(normalFont);
	y += 200;

	p.drawRect(0, startY, 8000, y-startY);
	y += 400;
	p.drawText(QRectF(0, y, 8000, 400), footer);

	p.setFont(bankFont);
	p.drawText(QRectF(0, 13000, 8000, 200), bankInfo);
}
