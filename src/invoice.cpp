#include <QApplication>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QPageSize>
#include <QDate>

class Item {
public:
	Item(QString const &desc, int perU):quantity(1),description(desc),perUnit(perU) {}
	Item(int quant, QString const &desc, int perU):quantity(quant),description(desc),perUnit(perU) {}
public:
	int quantity;
	QString description;
	double perUnit;
};

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	QPdfWriter pw("test.pdf");
	pw.setPageLayout(QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF(25, 10, 25, 10), QPageLayout::Millimeter));
	//pw.newPage();
	QPainter p(&pw);

	QPixmap const logo("/home/bero/Pictures/logo-small.png");

	QDate const date=QDate::currentDate();

	QString const sender = QStringLiteral(
			"LinDev\n"
			"Bernhard Rosenkränzer\n"
			"Bifang 18\n"
			"8757 Filzbach\n"
			"Switzerland\n"
			"\u260e +41 76 201 61 50\n"
			"https://lindev.ch/\n"
			"info@lindev.ch\n"
			"\n"
			"Tax ID: CHE-159.312.069\n"
			"VAT ID: CHE-159.312.069-MWST\n");

	QString const recipient = QStringLiteral(
			"Manning GmbH\n"
			"Hohenzollernstrasse 60\n"
			"80801 München\n"
			"Deutschland\n"
			"MWST-Nr. DE 813 252 387\n"
			);

	QString const company = QStringLiteral("LinDev \u2022 Bifang 18 \u2022 8757 Filzbach \u2022 Switzerland");

	QString const invoiceno = QStringLiteral("20210402");

	QString const footer = QStringLiteral("Unless agreed upon otherwise in writing, this invoice is to be paid in full by wire transfer to the account given below within 30 days.\nThank you for your business!");

	QString const bankInfo = QStringLiteral("LinDev Bernhard Rosenkränzer \u2022 IBAN CH30 8080 8003 8782 4289 4 \u2022 Raiffeisenbank Zug \u2022 SWIFT-BIC: RAIFCH22 \u2022 Clearing number: 80808");

	QList<Item> items{
		Item(176, "Hours of work as per supplied timesheet\nPrincipal Technologist, Huawei", 150)
	};

	QString const currency = QStringLiteral("€");

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

	p.drawText(QRectF(0, 3700, 8000, 400), QString(
				"Invoice for SERVICES\n"
				"Period of invoicing: 2021-04-01 - 2021-04-30"));

	pw.setTitle(QStringLiteral("LinDev - invoice #") + invoiceno);
	int const startY = 4100;
	int y = startY;
	int i = 1;
	p.drawText(QRectF(0, y, 300, 200), QStringLiteral("Item"));
	p.drawText(QRectF(300, y, 700, 200), QStringLiteral("Quantity"));
	p.drawText(QRectF(1000, y, 5000, 200), QStringLiteral("Description"));
	p.drawText(QRectF(6500, y, 500, 400), QStringLiteral("per Unit"));
	p.drawText(QRectF(7500, y, 500, 400), QStringLiteral("Price"));
	y += 200;
	double total;
	for(auto const &item: items) {
		p.drawText(QRectF(0, y, 300, 400), QString::number(i++));
		p.drawText(QRectF(300, y, 700, 400), QString::number(item.quantity));
		p.drawText(QRectF(1000, y, 5000, 400), item.description);
		p.drawText(QRectF(6500, y, 500, 400), Qt::AlignRight, locale.toString(item.perUnit, 'f', 2)+currency);
		double totalItem = item.perUnit*item.quantity;
		p.drawText(QRectF(7000, y, 1000, 400), Qt::AlignRight, locale.toString(totalItem, 'f', 2)+currency);
		p.drawLine(0, y, 8000, y);
		y += 400;
		total += totalItem;
	}
	p.drawLine(300, startY, 300, y);
	p.drawLine(1000, startY, 1000, y);
	p.drawLine(6500, startY, 6500, y);
	p.drawText(QRectF(1000, y, 5500, 400), Qt::AlignRight, QStringLiteral("Net amount"));
	p.drawText(QRectF(7000, y, 1000, 400), Qt::AlignRight, locale.toString(total, 'f', 2)+currency);
	p.drawLine(0, y, 8000, y);
	y += 200;
	p.drawText(QRectF(1000, y, 5500, 400), Qt::AlignRight, QStringLiteral("No VAT for services delivered outside of Switzerland"));
	p.drawLine(0, y, 8000, y);
	y += 200;
	p.setFont(boldFont);
	p.drawText(QRectF(1000, y, 5500, 400), Qt::AlignRight, QStringLiteral("Gross amount"));
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
