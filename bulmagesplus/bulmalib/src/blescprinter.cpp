/***************************************************************************
 *   Copyright (C) 2008 by Oriol Farrás Sanz.                             *
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

#include "blescprinter.h"
#include <QImage>
#include <QFile>
#include <QString>
#include <QTextCodec>
#include <QChar>
#include <math.h>

#include "blfunctions.h"

#define TWOPI 6.283185307179586476925287

BlEscPrinter::BlEscPrinter()
{
    m_device = QString ( "/dev/lp0" );
}

BlEscPrinter::BlEscPrinter ( QString device )
{
    this->m_device = device;
}

void BlEscPrinter::print()
{
    QFile file ( m_device );
    if ( !file.open ( QIODevice::WriteOnly | QIODevice::Unbuffered ) ) {
        //ERROR
    } // end if

    file.write ( m_buffer );
    file.close();
}

void BlEscPrinter::printAndClearBuffer()
{
    print();
    clearBuffer();
}

void BlEscPrinter::clearBuffer()
{
    m_buffer.clear();
}

void BlEscPrinter::initializePrinter()
{
    const char header[2] = {0x1B, 0x40};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
}

void BlEscPrinter::printText ( QString text )
{
  QTextCodec *codec = QTextCodec::codecForName("IBM 850");
  if (codec)
  {
    QByteArray encodedString = codec->fromUnicode(text);
    m_buffer.append(encodedString);
  } // end if
}

void BlEscPrinter::printImage ( QString path )
{
	BL_FUNC_DEBUG
    QImage img = QImage ( path );
	
    if ( !img.isNull() ) {
        if ( img.depth() != 1 ) {
            img = img.convertToFormat ( QImage::Format_Mono, Qt::MonoOnly );
        }
        if ( !img.isNull() ) {
            unsigned  short bytesX = img.bytesPerLine();
            unsigned  short dotsY = img.height();
            if ( img.width() / 8 > 65535 || img.height() > 2303 ) {
                //ERROR IMAGE too large
            } else {
                //int numBytes = img.sizeInBytes(); // Qt>5.10
                int numBytes = img.byteCount();
                char* data = ( char* ) img.bits();

                char XL = ( char ) ( bytesX );
                char XH = ( char ) ( bytesX >> 8 );
                char YL = ( char ) ( dotsY );
                char YH = ( char ) ( dotsY >> 8 );

                const char header[8] = {0x1D, 0x76, 0x30, 0x00, XL, XH, YL, YH};

                m_buffer.append ( QByteArray::fromRawData ( header, 8 ) );
                m_buffer.append ( QByteArray::fromRawData ( data, numBytes ) );
            }
        } else {
            //ERROR CONVERT IMAGE
        }
    } else {
        //ERROR LOADING LOGO
    }
	
	
	 
}


void BlEscPrinter::printImageRaw ( QByteArray &arr )
{
    QImage img = QImage ( );
    img.loadFromData(arr, "PNG");

    if ( !img.isNull() ) {
        if ( img.depth() != 1 ) {
            img = img.convertToFormat ( QImage::Format_Mono, Qt::MonoOnly );
        }
        if ( !img.isNull() ) {
            unsigned  short bytesX = img.bytesPerLine();
            unsigned  short dotsY = img.height();
            if ( img.width() / 8 > 65535 || img.height() > 2303 ) {
                //ERROR IMAGE too large
            } else {
                //int numBytes = img.sizeInBytes(); // Qt>5.10
                int numBytes = img.byteCount();
                char* data = ( char* ) img.bits();

                char XL = ( char ) ( bytesX );
                char XH = ( char ) ( bytesX >> 8 );
                char YL = ( char ) ( dotsY );
                char YH = ( char ) ( dotsY >> 8 );

                const char header[8] = {0x1D, 0x76, 0x30, 0x00, XL, XH, YL, YH};

                m_buffer.append ( QByteArray::fromRawData ( header, 8 ) );
                m_buffer.append ( QByteArray::fromRawData ( data, numBytes ) );
            }
        } else {
            //ERROR CONVERT IMAGE
        }
    } else {
        //ERROR LOADING LOGO
    }
}



//CHARACTER ESC CODES
void BlEscPrinter::setCharacterSpacing ( unsigned char n )
{
    const char header[3] = {0x1b, 0x20, ( char ) n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setCharacterCodeTable ( characterCodeTable n )
{
    const char header[3] = {0x1B, 0x74, ( char ) n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setCharacterPrintMode ( char mask )
{
    const char header[3] = {0x1B, 0x21, mask};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setUnderlineMode ( bool mode )
{
    char n = mode ? 1 : 0;
    const char header[3] = {0x1B, 0x2D, n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setCharacterSize ( char mask )
{
    const char header[3] = {0x1D, 0x21, mask};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setSmoothing ( bool mode )
{
    char n = mode ? 1 : 0;
    const char header[3] = {0x1D, 0x62, n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setDoubleStrike ( bool mode )
{
    char n = mode ? 1 : 0;
    const char header[3] = {0x1B, 0x47, n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::turnUpsideDown ( bool mode )
{
    char n = mode ? 1 : 0;
    const char header[3] = {0x1B, 0x7B, n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::turn90CWRotation ( bool onoff, bool extraSpacing )
{
    char n;
    onoff == 1 ? n = 1 + ( char ) extraSpacing : n = 0;
    const char header[3] = {0x1B, 0x56, n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::turnWhiteBlack ( bool mode )
{
    char n = mode ? 1 : 0;
    const char header[3] = {0x1D, 0x42, n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setColor ( printColor color )
{
    const char header[7] = {0x1D, 0x28, 0x4E, 0x02, 0x00, 48, ( char ) color};
    m_buffer.append ( QByteArray::fromRawData ( header, 7 ) );
}

//PRINT POSITION COMMANDS
void BlEscPrinter::horizontalTab()
{
    m_buffer.append ( 0x09 );
}

void BlEscPrinter::setHorizontalTabPos ( int numtabs, char* tabspos )
{
    if ( 0 >= numtabs && numtabs > 32 )
        return;
    const char header[2] = {0x1B, 0x44};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
    m_buffer.append ( QByteArray::fromRawData ( tabspos, numtabs ) );
    m_buffer.append ( ( char ) 0x00 );
}

void BlEscPrinter::setLeftMargin ( unsigned short margin )
{
    char nL = ( char ) ( margin );
    char nH = ( char ) ( margin >> 8 );
    const char header[4] = { 0x1D, 0x4C, nL, nH };
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

void BlEscPrinter::setPrintingAreaWidth ( unsigned short width ) //width default = 512
{
    char nL = ( char ) ( width );
    char nH = ( char ) ( width >> 8 );
    const char header[4] = { 0x1D, 0x57, nL, nH };
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

void BlEscPrinter::setJustification ( BlEscPrinter::justification j )
{
    const char header[3] = {0x1B, 0x61, ( char ) j};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setHAbsolutePos ( unsigned short pos )
{
    char nL = ( char ) pos;
    char nH = ( char ) ( pos >> 8 );

    const char header[4] = {0x1B, 0x24, nL, nH};
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

void BlEscPrinter::setHRelativePos ( unsigned short pos )
{
    char nL = ( char ) pos;
    char nH = ( char ) ( pos >> 8 );

    const char header[4] = {0x1B, 0x5C, nL, nH};
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

//Barcode commands
void BlEscPrinter::printBarCode ( barcodeSystem m, int n, char* codeData )
{
    int error = 0;
    switch ( m ) {
    case upca:
        if ( ( 11 <= n && n <= 12 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 48 <= codeData[i] && codeData[i] <= 57 ) )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case upce:
        if ( ( 11 <= n && n <= 12 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 48 <= codeData[i] && codeData[i] <= 57 ) )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case jan13:
        if ( ( 12 <= n && n <= 13 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 48 <= codeData[i] && codeData[i] <= 57 ) )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case jan8:
        if ( ( 7 <= n && n <= 8 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 48 <= codeData[i] && codeData[i] <= 57 ) ) {
                    error = 2; //BADBARCODE
                }
            }
        } else {
            error = 1;//BADBARCODE
        }
        break;

    case code39:
        if ( ( 1 <= n && n <= 255 ) ) {
            for ( int i = 0; i < n; i++ ) {
                int d = codeData[i];
                if ( ! ( 48 <= d && d <= 57 ) && ! ( 65 <= d && d <= 90 ) && d != 32 && d != 36 && d != 37 && d != 43 && d != 45 && d != 46 && d != 47 && d != 42 )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case itf:
        if ( ( 1 <= n && n <= 255 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 48 <= codeData[i] && codeData[i] <= 57 ) )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case codabar:
        if ( ( 1 <= n && n <= 255 ) ) {
            for ( int i = 0; i < n; i++ ) {
                int d = codeData[i];
                if ( ! ( 48 <= d && d <= 57 ) && ! ( 65 <= d && d <= 68 ) && d != 36 && d != 43 && d != 45 && d != 46 && d != 47 && d != 58 )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case code93:
        if ( ( 1 <= n && n <= 255 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 0 <= codeData[i] && codeData[i] <= 127 ) )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;

    case code128:
        if ( ( 2 <= n && n <= 255 ) ) {
            for ( int i = 0; i < n; i++ ) {
                if ( ! ( 0 <= codeData[i] && codeData[i] <= 127 ) )
                    error = 2; //BADBARCODE
            }
        } else
            error = 1;//BADBARCODE
        break;
    default:
        error = 3;
        break;//return;
    }
    if ( error ) {
        switch ( error ) {
        case 1:
            //NUMERO DE CODIGOS INCORRECTO
            break;
        case 2:
            //CARACTER INCORRECTO
            break;
        case 3:
            //TIPO DE CODIGO DE BARRAS NO SOPORTADO
            break;
        }

    } else {

        const char header[3] = {0x1D, 0x6B, ( char ) m};
        m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
        m_buffer.append ( QByteArray::fromRawData ( codeData, n ) );
        m_buffer.append ( ( char ) 0x00 );
    }
}

void BlEscPrinter::setBarCodeHeight ( int n )
{
    const char header[3] = {0x1D, 0x68, ( char ) n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setBarCodeWidth ( int n ) // 1<=n<=6 on tm-t88IV
{
    if ( ! ( 2 <= n && n <= 6 ) )
        return; //ERROR

    const char header[3] = {0x1D, 0x77, ( char ) n};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setBarcodeFormat ( int width, int height, barCodeTextPos pos, printerFont font ) // 1<=width<=6 on tm-t88IV
{
    setBarCodeHeight ( height );
    setBarCodeWidth ( width );

    const char header[3] = {0x1D, 0x48, ( char ) pos};
    const char header2[3] = {0x1D, 0x66, ( char ) font};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
    m_buffer.append ( QByteArray::fromRawData ( header2, 3 ) );
}


//Page mode
void BlEscPrinter::selectPageMode()
{
    const char header[2] = {0x1B, 0x4C};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
}

void BlEscPrinter::setPrintArea ( unsigned short x, unsigned short y, unsigned short dx, unsigned short dy )
{
    char xL = ( char ) x;
    char xH = ( char ) ( x >> 8 );
    char yL = ( char ) y;
    char yH = ( char ) ( y >> 8 );
    char dxL = ( char ) dx;
    char dxH = ( char ) ( dx >> 8 );
    char dyL = ( char ) dy;
    char dyH = ( char ) ( dy >> 8 );

    const char header[10] = {0x1B, 0x57, xL, xH, yL, yH, dxL, dxH, dyL, dyH};
    m_buffer.append ( QByteArray::fromRawData ( header, 10 ) );
}

void BlEscPrinter::setPrintDirection ( printDirection dir )
{
    const char header[3] = {0x1B, 0x54, ( char ) dir};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::setVAbsolutePos ( unsigned short pos )
{
    char nL = ( char ) pos;
    char nH = ( char ) ( pos >> 8 );

    const char header[4] = {0x1D, 0x24, nL, nH};
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

void BlEscPrinter::setVRelativePos ( unsigned short pos )
{
    char nL = ( char ) pos;
    char nH = ( char ) ( pos >> 8 );

    const char header[4] = {0x1D, 0x5C, nL, nH};
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

void BlEscPrinter::cancelPrintData()
{
    m_buffer.append ( 0x18 );
}

//MACRO FUNCTION COMMANDS
void BlEscPrinter::startEndMacro()
{
    const char header[2] = {0x1D, 0x3A};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
}

void BlEscPrinter::executeMacro ( int rep, int timePeriod, bool mode )
{
    const char header[5] = {0x1D, 0x5E, ( char ) rep, ( char ) timePeriod, ( char ) mode };
    m_buffer.append ( QByteArray::fromRawData ( header, 5 ) );
}

//MECHANISM CONTROL COMMANDS
void BlEscPrinter::turnUnidirPrintMode ( bool onoff ) //No on TM-t88II TM-L60II
{
    const char header[3] = {0x1B, 0x55, ( char ) onoff};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::returnHome() //No on TM-t88II TM-L60II
{
    const char header[2] = {0x1B, 0x3C};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
}

void BlEscPrinter::cutPaper ( bool partialcut ) //ONLY partial cut on tm-t88II
{
    const char header[3] = {0x1D, 0x56, ( char ) partialcut};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::cutPaperAndFeed ( bool partialcut, int feed ) //ONLY partial cut on tm-t88II
{
    char m = partialcut ? 66 : 65;
    const char header[4] = {0x1D, 0x56, m, ( char ) feed};
    m_buffer.append ( QByteArray::fromRawData ( header, 4 ) );
}

//Print commands
void BlEscPrinter::printAndLineFeed()
{
    m_buffer.append ( 0x0A );
}

void BlEscPrinter::printAndReturnStandardMode()
{
    m_buffer.append ( 0x0C );
}

void BlEscPrinter::printAndCarriageReturn()
{
    m_buffer.append ( 0x0D );
}

void BlEscPrinter::printDataPageMode()
{
    const char header[2] = {0x1B, 0x0C};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
}

void BlEscPrinter::printAndFeedPaper ( int feed )
{
    const char header[3] = {0x1B, 0x4A, ( char ) feed};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::printAndFeedLines ( int lines )
{
    const char header[3] = {0x1B, 0x64, ( char ) lines};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

void BlEscPrinter::printAndReverseFeed ( int feed ) //Not compatible TM-t88II III
{
    const char header[3] = {0x1B, 0x4B, ( char ) feed};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

//Line spacing commands
void BlEscPrinter::selectDefaultLineSpacing()
{
    const char header[2] = {0x1B, 0x32};
    m_buffer.append ( QByteArray::fromRawData ( header, 2 ) );
}

void BlEscPrinter::setLineSpacing ( int lspacing )
{
    const char header[3] = {0x1B, 0x33, ( char ) lspacing};
    m_buffer.append ( QByteArray::fromRawData ( header, 3 ) );
}

//2D CODES
void BlEscPrinter::printQRCode ( qrModel model, int dotSize, qrErrorCorrection error, unsigned short numData, char* data ) //1<=dotSize<=16
{
    numData += 3;
    char pl = ( char ) numData;
    char ph = ( char ) ( numData >> 8 );

    const char selectModel[9] = {0x1D, 0x28, 0x6B, 0x04, 0x00, 0x31, 0x41, ( char ) model, 0x00};
    const char setSize[8] = {0x1D, 0x28, 0x6B, 0x03, 0x00, 0x31, 0x43, ( char ) dotSize};
    const char errorCorrection[8] = {0x1D, 0x28, 0x6B, 0x03, 0x00, 0x31, 0x45, ( char ) error};


    const char storeData[8] = {0x1D, 0x28, 0x6B, pl, ph, 0x31, 0x50, 0x30};
    const char print[8] = {0x1D, 0x28, 0x6B, 0x03, 0x00, 0x31, 0x51, 0x30};

    m_buffer.append ( QByteArray::fromRawData ( storeData, 8 ) );
    m_buffer.append ( QByteArray::fromRawData ( data, numData - 3 ) );
    m_buffer.append ( QByteArray::fromRawData ( selectModel, 9 ) );
    m_buffer.append ( QByteArray::fromRawData ( setSize, 8 ) );
    m_buffer.append ( QByteArray::fromRawData ( errorCorrection, 8 ) );

    m_buffer.append ( QByteArray::fromRawData ( print, 8 ) );
}

void BlEscPrinter::test()
{
    initializePrinter();
    setCharacterCodeTable ( page19 );
    setJustification ( center );

    printImage ( "/usr/local/share/bulmages/images/splashbulmafact.jpg" );
    printText ( "\n" );
    setJustification ( left );

    setCharacterPrintMode ( CHARACTER_FONTB_SELECTED | EMPHASIZED_MODE | DOUBLE_HEIGHT | DOUBLE_WIDTH | UNDERLINE_MODE );
    printText ( "CHARACTER_FONTB_SELECTED|EMPHASIZED_MODE|DOUBLE_HEIGHT|DOUBLE_WIDTH|UNDERLINE_MODE\n" );
    setCharacterPrintMode ( CHARACTER_FONTB_SELECTED );
    printText ( "CHARACTER_FONTB_SELECTED\n" );

    setUnderlineMode ( 1 );
    printText ( "setUnderlineMode (1)\n" );
    setUnderlineMode ( 0 );
    printText ( "setUnderlineMode (0)\n" );

    setCharacterSize ( CHAR_WIDTH_3 | CHAR_HEIGHT_3 );
    printText ( "setCharacterSize ( CHAR_WIDTH_3|CHAR_HEIGHT_3 );\n" );
    setCharacterSize ( CHAR_WIDTH_1 | CHAR_HEIGHT_1 );
    printText ( "setCharacterSize ( CHAR_WIDTH_1|CHAR_HEIGHT_1 );\n" );

    setSmoothing ( 0 );
    printText ( "setSmoothing ( 0 );\n" );

    setSmoothing ( 1 );
    printText ( "setSmoothing ( 1 );\n" );

    setDoubleStrike ( 1 );
    printText ( "setDoubleStrike ( 1 )\n" );
    setDoubleStrike ( 0 );
    printText ( "setDoubleStrike ( 0 )\n" );

    turnUpsideDown ( 1 );
    printText ( "turnUpsideDown ( 1 )\n" );
    turnUpsideDown ( 0 );
    printText ( "turnUpsideDown ( 0 )\n" );

    turn90CWRotation ( true, true );
    printText ( "turn90CWRotation ( 1 )\n" );
    turn90CWRotation ( false, false );
    printText ( "turn90CWRotation ( 0 )\n" );

    turnWhiteBlack ( 1 );
    printText ( "turnWhiteBlack ( 1 )\n" );
    turnWhiteBlack ( 0 );
    printText ( "turnWhiteBlack ( 0 )\n" );

    setColor ( red );
    printText ( "setColor ( red )\n" );
    setColor ( black );
    printText ( "setColor ( black )\n" );

    //MECHANISM CONTROL COMMANDS
    turnUnidirPrintMode (  true ); //No on TM-t88II TM-L60II
    printText ( "turnUnidirPrintMode ( true )\n" );
    turnUnidirPrintMode (  false ); //No on TM-t88II TM-L60II
    printText ( "turnUnidirPrintMode ( false )\n" );

    //PRINT POSITION COMMANDS
    char tabspos[2] = {8, 30};
    setHorizontalTabPos ( 2, tabspos );
    horizontalTab();
    printText ( "Horizpos=8" );
    horizontalTab();
    printText ( "Horizpos = 30\n" );

    setLeftMargin ( 50 );
    printText ( "setLeftMargin ( 50 )\n" );
    setLeftMargin ( 0 );
    printText ( "setLeftMargin ( 0 )\n" );

    setPrintingAreaWidth ( 100 ); //width default = 512
    printText ( "setPrintingAreaWidth ( 10 )\n" );
    setPrintingAreaWidth ( 512 ); //width default = 512
    printText ( "setPrintingAreaWidth ( 512 )\n" );

    setJustification ( right );
    printText ( "setJustification ( right )\n" );
    setJustification ( center );
    printText ( "setJustification ( center )\n" );
    setJustification ( left );
    printText ( "setJustification ( left )\n" );

    //BARCODE
    setJustification ( center );
    setBarcodeFormat ( 2, 50, both, fontA );
    char barcode[11] = {'A', 'Z', 'I', 'P', 'P', 'L', 'M', 'Z', 'T', 'W', '1'};
    printBarCode ( code39, 11, barcode );

    //QRCODE
    QByteArray tmp;
    tmp = QString ( "Esto es un QRCODE. Un es pregunta quantes dades es poden arribar a posar en un qrcode" ).toLatin1();
    char* qrdata = tmp.data();
    unsigned short numData = tmp.size();
    printQRCode ( qrModel2, 8, qrlevelL, numData, qrdata );

    //pagemode

    selectPageMode();
    setPrintArea ( 100, 100, 350, 700 );

    int reps = 100;
    int radius = 150;
    int dx = 150;
    int dy = 200;
    for ( float i = 0; i < reps; i++ ) {
        float angle = TWOPI * ( ( float ) ( i / reps ) );
        unsigned short x = ( unsigned short ) ( radius * cosf ( angle ) + dx );
        unsigned short y = 2 * ( unsigned short ) ( radius * sinf ( angle ) + dy );
        setVAbsolutePos ( y );
        setHAbsolutePos ( x );
        printText ( "€" );

    }

    printAndReturnStandardMode();

    selectPageMode();
    setPrintArea ( 0, 0, 500, 600 );
    reps = 200;
    int p = 400;
    int r = 100;
    for ( float i = 0; i < reps; i++ ) {
        float angle = TWOPI * ( ( float ) ( i / reps ) );
        unsigned short x = p * ( i / ( float ) ( reps - 1 ) );
        unsigned short y = 2 * ( unsigned short ) ( r * sinf ( angle ) + dy );
        setVAbsolutePos ( y );
        setHAbsolutePos ( x );
        printText ( "." );
    }
    printDataPageMode();

    for ( float i = 0; i < reps; i++ ) {
        float angle = TWOPI * ( ( float ) ( i / reps ) );
        unsigned short x = p * ( i / ( float ) ( reps - 1 ) );
        unsigned short y = 2 * ( unsigned short ) ( r * cosf ( angle ) + dy );
        setVAbsolutePos ( y );
        setHAbsolutePos ( x );
        printText ( "." );
    }
    printAndReturnStandardMode();

    cutPaperAndFeed ( true, 10 );
    print();
}

QByteArray BlEscPrinter::buffer() {
  return m_buffer;
}
