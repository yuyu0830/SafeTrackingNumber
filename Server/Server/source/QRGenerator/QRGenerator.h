#pragma once

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "qrcodegen.hpp"

using namespace qrcodegen;

class QRGenerator {
public:
    QRGenerator();

    void MakeQRCode(const char* sk, int len);
    
private:
    static std::string toSvgString(const QrCode& qr, int border);
    static void printQr(const QrCode& qr); 

    void HexToChar(const char* source, char* destination, int length);
    void CharToHex(const char* source, char* destination, int length);

public:


private:


};