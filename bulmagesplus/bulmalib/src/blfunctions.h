/***************************************************************************
 *   Copyright (C) 2003 by Tomeu Borrás Riera.                             *
 *   Copyright (C) 2003 by Josep Burción.                                  *
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

/// Fichero de definicion de funciones auxiliares que no es preciso encapsular
/// En este fichero se definen todas aquellas funciones que por su brevedad o aislamiento son
/// utilizadas de forma regular en el programa. Implementadas en blfunctions.cpp
/// Dichas funciones normalmente son de uso general, por lo que es normal ver este
/// archivo incluido en la practica totalidad de los demas ficheros.
#ifndef BLFUNCTIONS_H
#define BLFUNCTIONS_H

#include <QDateTime>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QString>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QProcessEnvironment>

#include <stdio.h>
#include <sstream>
#include <string>
#include <math.h>
#include <cstddef>

#include <QMainWindow>
#include <QStatusBar>
#ifdef Q_OS_WIN32
	#include <qt_windows.h>
#endif

#include "bldefs.h"
#include "blapplication.h"
#include "blconfig.h"
#include "bli18n.h"

class BL_EXPORT BlDebug 
{
public:
  static QFile *m_file;
  static QFile *m_fileXML;
  static QTextStream *m_out;
  static QTextStream *m_outXML;

  static int m_auxxml;
  static int m_supnivel;
  static int m_indice;
  static QString m_mensajesanulados[7000];
  static QString m_clasesanuladas[7000];
  static int m_indiceclases;
  
  QTime   m_time;
  QString m_func;
  int m_level;
  QString m_params;  
  
  BlDebug(const QString &func, int level = 0, const QString & params = "");
  virtual ~BlDebug();
  static void blDebug(const QString &text, int level = 0 , const QString &params = "");
};

/// Extiende un string a un numero de cuenta sustituyendo los '.' por ceros.
QString BL_EXPORT blExtendStringWithZeros ( QString, unsigned int );
/// Redondeo de numeros en punto flotante.
float BL_EXPORT blFloatRound ( float, unsigned );
/// Esta funcion convierte un numero con decimales a un entero.
int blDoubleToInt ( double );
/// Procesa el string pasado como parametro y devuelve una estructura del tipo QDate.
QDate BL_EXPORT blNormalizeDate ( QString );


/// Escapa los mínimos caracteres necesarios para 
/// escribir a cadena a XML, sin usar CDATA porque puede no 
/// estar permitido en todos los lugares de un fichero xml 
QString BL_EXPORT blXMLEscape ( const QString& text );

/// Escapa una cadena para que pueda ser embebida en un script python.
QString BL_EXPORT blPythonEscape ( const QString &text);

/// Sistema general de escape
QString BL_EXPORT blStringEscape (const QString &text, int tipoEscape = 0);

/// Proteje cadenas de texto pasandoles una sustitucion de codigos especiales de XML.
QString BL_EXPORT blXMLEncode ( const QString& );
QString BL_EXPORT blXMLDecode ( const QString& );
/// Ajusta un codigo de cuenta a una longitud determinada pasada como parametro.
QString BL_EXPORT blExtendCodeLength ( QString, unsigned int );
void BL_EXPORT blReplaceStringInFile ( QString, QString, QString, QString );
/// Esta funcion permite editar un texto en un QTextEdit y devuelve el texto editado.
QString BL_EXPORT blTextEditor ( QString texto );

#if CONFIG_DEBUG == true
#ifdef OLD_DEBUG
void BL_EXPORT blDebug ( const QString &cad, int nivel = 0, const QString & param = "" );
#endif
#define BL_FUNC_DEBUG  BlDebug __llamada_debug(Q_FUNC_INFO, 0);
#else
#ifdef OLD_DEBUG
#define blDebug(...)       // sin debug
#endif
#define BL_FUNC_DEBUG      // sin debug
#endif
void BL_EXPORT blDebugOn();
void BL_EXPORT blDebugOff();


void BL_EXPORT blMsgInfo ( QString cad, QWidget *parent = 0 );
void BL_EXPORT blMsgWarning ( QString cad, QWidget *parent = 0 );
void BL_EXPORT blMsgError ( QString cad, QWidget *parent = 0 );

/// En la impresion de documentos 'RML' esta funcion hace casi todo el trabajo.
/// de la invocacion de 'bgtrml2pdf'.
/// Para evitar trabajo duplicado. El archivo debe estar ubicado ya en el directorio.
/// especificado por CONF_DIR_USER.
void BL_EXPORT blCreateAndLoadPDF ( const QString arch );
void BL_EXPORT blCreatePDF ( const QString arch );
void BL_EXPORT blCreateAndLoadODS ( const QString arch );
void BL_EXPORT blCreateODS ( const QString arch );

QString BL_EXPORT blNumberToText ( QString numero, QString moneda = "euros", QString singular = "euro" );
void BL_EXPORT blCenterOnScreen ( QWidget *ventana );
QString BL_EXPORT blWindowId ( const QString & );
/// Convierte un string en un string valido para ser escrito desde un script.
QString BL_EXPORT blScriptEscape ( const QString &cad );
QString BL_EXPORT blStringToUsAscii ( const QString &orig );
QString BL_EXPORT blStringToPython(QString string, bool eraseQuotes = true);
bool BL_EXPORT blValidateSpainCIFCode(QString cif1, QChar &digit);
bool BL_EXPORT blValidateSpainNIFCode(QString nif1, QChar &digit);
int BL_EXPORT blSumAllDigits(int val);
bool BL_EXPORT blValidateSpainCIFNIFCode(QString nifcif, QChar &digit);

void BL_EXPORT blRawPrint(const QString &archivo, bool diruser=true, const QString &defprinter="");

int BL_EXPORT blWebBrowser(const QString &uri, const QString &defbrowser="");

QString BL_EXPORT blGetEnv( const char *varName );

#ifdef Q_OS_WIN32
wchar_t* BL_EXPORT BlQStringToWCHAR (QString inString);
bool BL_EXPORT BlRawDataToPrinter(QString printerName, QByteArray dataToPrint);
#endif
  
  
///Copies a file from one location to another
bool BL_EXPORT blCopyFile(const QString &oldName, const QString &newName);

///Moves a file from one location to another
bool BL_EXPORT blMoveFile(const QString &oldName, const QString &newName);

///Delets a file
bool BL_EXPORT blRemove(const QString &filetoremove);

///Delets a directory and all its contents
bool BL_EXPORT blRemoveDirectory(const QString &directorytoremove);



extern "C" BL_EXPORT int Thunderbird ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached );
extern "C" BL_EXPORT int Kmail ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached );
extern "C" BL_EXPORT int Evolution ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached );

#ifdef Q_OS_WIN32
extern "C" BL_EXPORT int Outlook ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached );
#endif

extern "C" BL_EXPORT int blSendEmail ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached );


///Delets a directory and all its contents
QString BL_EXPORT blGetLang();
bool BL_EXPORT isPluginActive(const QString &theplugin);

bool BL_EXPORT blStartDetachedProcess(const QString path, const QString command, const QList<QString> checkExistingFiles = QList<QString>(), const bool forceNewConsole = false, QProcessEnvironment *env = nullptr);

#endif



