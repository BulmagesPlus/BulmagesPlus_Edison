/***************************************************************************
 *   Copyright (C) 2008 by Oriol Farrás Sanz.                              *
 *   Copyright (C) 2012 by Fco. Javier Miró Carmona.                       *
 *                                                                         *
 *   https://bulmagesplus.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef BLESCPRINTER_H
#define BLESCPRINTER_H

#include <QString>
#include <QByteArray>
#include "bldefs.h"


#define CHARACTER_FONTA_SELECTED 0x00
#define CHARACTER_FONTB_SELECTED 0x01
#define EMPHASIZED_MODE 0x08
#define DOUBLE_HEIGHT 0x10
#define DOUBLE_WIDTH 0x20
#define UNDERLINE_MODE 0x80

//Character width height defines
#define CHAR_WIDTH_1 0x00
#define CHAR_WIDTH_2 0x10
#define CHAR_WIDTH_3 0x20
#define CHAR_WIDTH_4 0x30
#define CHAR_WIDTH_5 0x40
#define CHAR_WIDTH_6 0x50
#define CHAR_WIDTH_7 0x60
#define CHAR_WIDTH_8 0x70
#define CHAR_HEIGHT_1 0x00
#define CHAR_HEIGHT_2 0x01
#define CHAR_HEIGHT_3 0x02
#define CHAR_HEIGHT_4 0x03
#define CHAR_HEIGHT_5 0x04
#define CHAR_HEIGHT_6 0x05
#define CHAR_HEIGHT_7 0x06
#define CHAR_HEIGHT_8 0x07

enum printColor {
    black = 49,
    red
};

enum characterCodeTable {
    page0 = 0, // [PC437 (U.S.A., Standard Europe)]
    page1, // [Katakana]
    page2, //[PC850 (Multilingual)]
    page3, // [PC860 (Portuguese)]
    page4, // [PC863 (Canadian-French)]
    page5, // [PC865 (Nordic)]
    page6, // [Simplified Kanji, Hirakana]
    page7, // [Simplified Kanji]
    page8, // [Simplified Kanji]
    page16 = 16, // [WPC1252]
    page17, // [PC866 (Cyrillic #2)]
    page18, // [PC852 (Latin 2)]
    page19, // [PC858 (Euro)]
    page20, // [Thailand character code 42]
    page21, // [Thailand character code 11]
    page22, // [Thailand character code 13]
    page23, // [Thailand character code 14]
    page24, // [Thailand character code 16]
    page25, // [Thailand character code 17]
    page26, // [Thailand character code 18]
    page254 = 254,
    page255


};


enum printerFont {
    fontA = 0,
    fontB
};



enum barcodeSystem {
    upca = 0, //UPC-A
    upce,
    jan13, //EAN13
    jan8, //EAN8
    code39,
    itf, //interleaved 2 of 5
    codabar, //NW7
    code93,
    code128
};

enum barCodeTextPos {
    notPrinted = 0,
    above, //above the bar code
    below,
    both //below and above
};

enum printDirection {
    leftToRight = 0,
    bottomTotop,
    rightToLeft,
    topToBottom
};

enum qrModel {
    qrModel1 = 49,
    qrModel2
};

enum qrErrorCorrection {
    qrlevelL = 48, // 7%
    qrlevelM, //15%
    qrlevelQ, //25%
    qrlevelH //30%

};

class BL_EXPORT BlEscPrinter
{

public:
enum justification {
    left = 0,
    center,
    right
};

public:
    BlEscPrinter();
    BlEscPrinter ( QString device );
    ~BlEscPrinter() {}

    void test();

    void print();
    void printAndClearBuffer();
    void clearBuffer();

    void initializePrinter();

    void printImage ( QString path );
    void printImageRaw ( QByteArray &arr );
    void printText ( QString text );

    //Character ops
    void setCharacterSpacing ( unsigned char n );
    void setCharacterCodeTable ( characterCodeTable n );
    void setCharacterPrintMode ( char mask );
    void setUnderlineMode ( bool mode );
    void setCharacterSize ( char mask );
    void setSmoothing ( bool mode );
    void setDoubleStrike ( bool mode );
    void turnUpsideDown ( bool mode );
    void turn90CWRotation ( bool onoff, bool extraSpacing );
    void turnWhiteBlack ( bool mode );
    void setColor ( printColor color );

    //PRINT POSITION COMMANDS
    void horizontalTab();
    void setHorizontalTabPos ( int numtabs, char* tabspos );
    void setLeftMargin ( unsigned short margin );
    void setPrintingAreaWidth ( unsigned short width ); //width default = 512
    void setJustification ( BlEscPrinter::justification j );
    void setHAbsolutePos ( unsigned short pos );
    void setHRelativePos ( unsigned short pos );

    //Barcode commands
    void setBarcodeFormat ( int width, int height, barCodeTextPos pos, printerFont font );
    void printBarCode ( barcodeSystem m, int n, char* codeData );
    void setBarCodeHeight ( int n );
    void setBarCodeWidth ( int n );

    //Page mode
    void selectPageMode();
    void setPrintArea ( unsigned short x, unsigned short y, unsigned short dx, unsigned short dy );
    void setPrintDirection ( printDirection dir );
    void setVAbsolutePos ( unsigned short pos );
    void setVRelativePos ( unsigned short pos );
    void cancelPrintData();

    //MACRO FUNCTION COMMANDS
    void startEndMacro();
    void executeMacro ( int rep, int timePeriod, bool mode );

    //MECHANISM CONTROL COMMANDS
    void turnUnidirPrintMode ( bool onoff ); //No on TM-t88II TM-L60II
    void returnHome();//No on TM-t88II TM-L60II
    void cutPaper ( bool partialcut ); //ONLY partial cut on tm-t88II
    void cutPaperAndFeed ( bool partialcut, int feed );//ONLY partial cut on tm-t88II


    //Print commands
    void printAndLineFeed();
    void printAndReturnStandardMode();
    void printAndCarriageReturn();
    void printDataPageMode();
    void printAndFeedPaper ( int feed );
    void printAndFeedLines ( int lines );
    void printAndReverseFeed ( int feed ); //Not compatible TM-t88II III ,

    //Line spacing commands
    void selectDefaultLineSpacing();
    void setLineSpacing ( int lspacing );

    //2D CODES
    void printQRCode ( qrModel model, int dotSize, qrErrorCorrection error, unsigned short numData, char* data ); //1<=dotSize<=16
    void printTest();

    QByteArray buffer();

private:
    QByteArray m_buffer;
    QString m_device;
};
#endif
