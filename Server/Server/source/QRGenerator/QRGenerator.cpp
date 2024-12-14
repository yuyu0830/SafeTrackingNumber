#include "QRGenerator.h"

QRGenerator::QRGenerator()
{
}

void QRGenerator::MakeQRCode(const char* sk, int len)
{
	/*const QrCode qr3 = QrCode::encodeText(
		"Alice was beginning to get very tired of sitting by her sister on the bank, "
		"and of having nothing to do: once or twice she had peeped into the book her sister was reading, "
		"but it had no pictures or conversations in it, 'and what is the use of a book,' thought Alice "
		"'without pictures or conversations?' So she was considering in her own mind (as well as she could, "
		"for the hot day made her feel very sleepy and stupid), whether the pleasure of making a "
		"daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly "
		"a White Rabbit with pink eyes ran close by her.", QrCode::Ecc::HIGH);
	
	printQr(qr3);*/

	char Text[384 * 4 + 2];
	HexToChar(sk, Text, len);

	const QrCode Qr = QrCode::encodeText(Text, QrCode::Ecc::LOW);
	printQr(Qr);
}

std::string QRGenerator::toSvgString(const QrCode& qr, int border)
{
	if (border < 0)
		throw std::domain_error("Border must be non-negative");
	if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
		throw std::overflow_error("Border too large");

	std::ostringstream sb;
	sb << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	sb << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
	sb << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
	sb << (qr.getSize() + border * 2) << " " << (qr.getSize() + border * 2) << "\" stroke=\"none\">\n";
	sb << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
	sb << "\t<path d=\"";
	for (int y = 0; y < qr.getSize(); y++) {
		for (int x = 0; x < qr.getSize(); x++) {
			if (qr.getModule(x, y)) {
				if (x != 0 || y != 0)
					sb << " ";
				sb << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
			}
		}
	}
	sb << "\" fill=\"#000000\"/>\n";
	sb << "</svg>\n";
	return sb.str();
	return std::string();
}

void QRGenerator::printQr(const QrCode& qr)
{
	int border = 4;
	for (int y = -border; y < qr.getSize() + border; y++) {
		for (int x = -border; x < qr.getSize() + border; x++) {
			std::cout << (qr.getModule(x, y) ? "##" : "  ");
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void QRGenerator::HexToChar(const char* source, char* destination, int length)
{
	for (int i = 0; i < length; i++) {
		char c = source[i];

		char first = (c & 0b11110000) >> 4;
		char second = (c & 0b1111);

		destination[i * 2]		= first + (first < 10 ? 48 : 55);
		destination[i * 2 + 1]	= second + (second < 10 ? 48 : 55);
	}

	destination[length * 2 + 1] = '\0';
}

void QRGenerator::CharToHex(const char* source, char* destination, int length)
{
	for (int i = 0; i < length; i++) {
		char first = source[i * 2];
		char second = source[i * 2 + 1];

		first = first - (first >= 'A' ? 55 : 48);
		second = second - (second >= 'A' ? 55 : 48);

		destination[i] = (first << 4) + second;
	}
}
