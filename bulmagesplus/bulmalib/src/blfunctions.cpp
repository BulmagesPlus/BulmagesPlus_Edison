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
#include <QString>
#include <QTextEdit>
#include <QDir>
#include <QTextStream>
#include <QLocale>
#include <QProcess>
#include <QDesktopServices>
#include <QUrl>
#include <QIODevice>

#include "blfunctions.h"
#include "blconfiguration.h"
#include "blmainwindow.h"
#include "local_blI18n.h"
#include "blplugins.h"

/// Definiciones para el email.
#ifdef Q_OS_WIN32
    #include <windows.h>
    #include <userenv.h>
    
    #define CAD_COMILLAS QString("\"")
    #define background QString("")
#else
    #define CAD_COMILLAS QString("")
    #define background QString(" &")
#endif





/** Las variables estaticas de clase deben ser delcaras para reservar memoria */
QFile *BlDebug::m_file;
QFile *BlDebug::m_fileXML;
QTextStream *BlDebug::m_out;
QTextStream *BlDebug::m_outXML;
int BlDebug::m_auxxml;
int BlDebug::m_supnivel;
int BlDebug::m_indice;
QString BlDebug::m_mensajesanulados[7000];
QString BlDebug::m_clasesanuladas[7000];
int BlDebug::m_indiceclases;


BlDebug::BlDebug(const QString &func, int level, const QString &params) {
    try {
      
	/// Si el objeto confpr no esta creado puede dar segmentation fault.
	if (!g_confpr ) return;

	/// Si no hay modo debug salimos directamente
	if (g_confpr->value(CONF_DEBUG) == "FALSE" ) return;

	m_func = func;
	m_level = level;
	m_params = params;
	m_time.start();

	static int semaforo = 0;
	if (!semaforo) {
	  BlDebug::m_auxxml = 0;
	  BlDebug::m_supnivel = 0;
	  BlDebug::m_indice = 0;
	  BlDebug::m_indiceclases = 0;
	  BlDebug::m_file = new QFile ( g_confpr->value( CONF_DIR_USER ) + "bulmagesout.txt" );
	  BlDebug::m_out = new QTextStream ( BlDebug::m_file );

	  BlDebug::m_fileXML = new QFile( g_confpr->value( CONF_DIR_USER ) + "bulmagesout.xml" );
	  BlDebug::m_outXML = new QTextStream ( BlDebug::m_fileXML );

	  if ( !BlDebug::m_file->open ( QIODevice::WriteOnly | QIODevice::Text ) )
	      throw -1;
	  if ( !BlDebug::m_fileXML->open ( QIODevice::WriteOnly | QIODevice::Text ) )
	      throw -1;
	  semaforo = 1;
	} // end if


      
	if ( m_level == 5 ) {
	    BlDebug::m_supnivel = 0;
	    m_level = 2;
	} // end if
	if ( m_level == 4 ) {
	    BlDebug::m_supnivel = 2;
	    m_level = 2;
	} // end if
	if ( m_level == 0 || m_level == 1 ) {
	    /// Si la cadena contiene END bajamos el nivel
	    BlDebug::m_auxxml ++;
	    if ( BlDebug::m_auxxml > 20 ) BlDebug::m_auxxml = 1;

	    for ( int i = 0; i < BlDebug::m_auxxml; i++ ) {
		*BlDebug::m_outXML << "    ";
		*BlDebug::m_out << "    ";
	    } // end for

	    QString cad1 = func;

		cad1 = "<" + cad1 + " time=\"" + QString::number ( m_time.elapsed() ) + "\" param=\"" + m_params + "\" >";


	    *BlDebug::m_outXML << cad1  << "\n" << flush;
	    *BlDebug::m_out << "\\" << func << " " << m_params << "\n" << flush;
	    
	} // end if
	for ( int i = 0; i < BlDebug::m_indice; i++ ) {
	    if ( func == BlDebug::m_mensajesanulados[i] ) {
		return;
	    } // end if
	} // end for
	for ( int i = 0; i < BlDebug::m_indiceclases; i++ ) {
	    if ( func.left ( func.indexOf ( "::" ) ) == BlDebug::m_clasesanuladas[i] ) {
		return;
	    } // end if
	} // end for

	if ( m_level == 2 || ( BlDebug::m_supnivel == 2 && m_level == 0 ) || m_level == 3 ) {
	    *BlDebug::m_out << func << " " << m_params << "\n" << flush;
	    int err = QMessageBox::information ( NULL,
						  _ ( "Informacion de depuracion" ),
						  func + " " + m_params,
						  _ ( "&Continuar" ),
						  _ ( "&Omitir" ),
						  _ ( "Omitir &clase" ),
						  0, 1 );
	    if ( err == 1 ) {
		BlDebug::m_mensajesanulados[BlDebug::m_indice++] = func;
	    } // end if
	    if ( err == 2 ) {
		BlDebug::m_clasesanuladas[BlDebug::m_indiceclases++] = func.left ( func.indexOf ( "::" ) );
	    } // end if
	} // end if
    } catch(...) {
	  fprintf(stderr, "Error en el tratamiento de la depuracion");
    } // end try
    
}

void BlDebug::blDebug(const QString &text, int level, const QString &params) {

    try {
      
      /// Si el objeto confpr no esta creado puede dar segmentation fault.
      if (!g_confpr ) throw -1;
      
      /// Si no hay modo debug salimos directamente
      if (g_confpr->value(CONF_DEBUG) == "FALSE" ) return;

      static int semaforo = 0;
      if (!semaforo) {
	BlDebug::m_auxxml = 0;
	BlDebug::m_supnivel = 0;
	BlDebug::m_indice = 0;
	BlDebug::m_indiceclases = 0;
	BlDebug::m_file = new QFile ( g_confpr->value( CONF_DIR_USER ) + "bulmagesout.txt" );
	BlDebug::m_out = new QTextStream ( BlDebug::m_file );

	BlDebug::m_fileXML = new QFile( g_confpr->value( CONF_DIR_USER ) + "bulmagesout.xml" );
	BlDebug::m_outXML = new QTextStream ( BlDebug::m_fileXML );

	if ( !BlDebug::m_file->open ( QIODevice::WriteOnly | QIODevice::Text ) )
	    throw -1;
	if ( !BlDebug::m_fileXML->open ( QIODevice::WriteOnly | QIODevice::Text ) )
	    throw -1;
	    semaforo = 1;
	} // end if
      
      
	for ( int i = 0; i <= BlDebug::m_auxxml; i++ ) {
	    *BlDebug::m_outXML << "    ";
	    *BlDebug::m_out << "    ";
	} // end for

	QString cad1;
	cad1 = "<comment  result=\"" + text + "\"  params=\"" + params + "\"></comment>";


	*BlDebug::m_outXML << cad1  << "\n" << flush;
	*BlDebug::m_out << "|" << text << " " << params << "\n" << flush;
    } catch(...) {
	  fprintf(stderr, "Error en el tratamiento de la depuracion");
    } // end try
}

BlDebug::~BlDebug() {

    try {
      
      /// Si el objeto confpr no esta creado puede dar segmentation fault.
      if (!g_confpr ) return;
      
      /// Si no hay modo debug salimos directamente
      if (g_confpr->value(CONF_DEBUG) == "FALSE" ) return;

       if ( m_level == 0 || m_level == 1 ) {
            for ( int i = 0; i < BlDebug::m_auxxml; i++ ) {
                *BlDebug::m_outXML << "    ";
                *BlDebug::m_out << "    ";
            } // end for

            QString cad1 = m_func;
            cad1 = "</" + cad1 + " time=\"" + QString::number ( m_time.elapsed() ) + "\" result=\"" + m_params + "\" >";


            *BlDebug::m_outXML << cad1  << "\n" << flush;
            *BlDebug::m_out << "/" << m_func << " " << m_params << "\n" << flush;
	    

            BlDebug::m_auxxml --;
            if ( BlDebug::m_auxxml < 0 ) BlDebug::m_auxxml = 20;
        } // end if

    } catch(...) {
	  fprintf(stderr, "Error en el tratamiento de la depuracion");
    } // end try
      
}


/// Esta funcion permite editar un texto en un QTextEdit y devuelve el texto editado.
QString blTextEditor ( QString texto )
{
    BL_FUNC_DEBUG
    QTextEdit *ed = new QTextEdit ();
    ed->setFixedSize ( 450, 250 );
    ed->setPlainText ( texto );
    g_main->setEnabled ( false );
    ed->show();
    while ( !ed->isHidden() ) {
        g_theApp->processEvents();
    } // end while
    g_main->setEnabled ( true );
    QString vuelta = ed->toPlainText();
    delete ed;
    return vuelta;
}


/// Reformatea una cadena de caracteres para poder ser escrita desde un script.
/// La conversion es a strings de C
/// cambia los slash por slash + slash
/// cambia las comillas por slash + comillas
/// cambia los tabuladores por slash + t
QString blScriptEscape ( const QString &cad )
{
    BL_FUNC_DEBUG
    QString result = cad;
    result.replace ( "\\", "\\\\" );
    result.replace ( "\"", "\\\"" );
    result.replace ( "\t", "\\t" );
    return result;
}


/// Escapa los mínimos carácteres necesarios para
/// escribir a cadena a XML, sin usar CDATA, para ahorrar bytes y porque puede que no
/// estén permitidos en todos los lugares de un fichero XML según esquemas o DTDs específicos.
QString blXMLEscape ( const QString& param )
{
    BL_FUNC_DEBUG
    QString text = param;
    text.replace ( "'", "&apos;" );
    text.replace ( "&", "&amp;" );
    text.replace ( '"', "&quot;" );
    text.replace ( "<", "&lt;" );
    text.replace ( ">", "&gt;" );

    text.replace ( "\n", "<br/>" );

    return text;
}


/// Escapa los caracteres necesarios para que la cadena pueda ser embebida dentro de un
/// script en python
QString blPythonEscape ( const QString& param )
{
    BL_FUNC_DEBUG
    QString text = param;
    text.replace ( "'", "\\'" );
    text.replace ( "\n", "\\n" );
    return text;
}


QString blStringEscape ( const QString &param, int tipoEscape )
{
    BL_FUNC_DEBUG
    QString param1;
    switch ( tipoEscape ) {
    case 1:
        param1 = blXMLEscape ( param );
        break;
    case 2:
        param1 = blPythonEscape ( param );
        break;
    default:
        param1 = param;

    } // end switch
    return param1;
}


/// Proteje cadenas de texto pasandoles una sustitucion de codigos especiales de XML.
/// Nota: ahora ficha.cpp ya respeta el encoding del XML
/// original y pone referencias numericas de caracter solo a los caracteres
/// no soportados en el encoding. Ya no hace tanta falta
/// esta funcion, y creo que con escapeXml pasamos (para
/// listados rml al menos). Podemos tener el buffer en
/// memoria en unicode hasta escribirlo a fichero en ficha::generateRML
QString blXMLEncode ( const QString &string )
{
    BL_FUNC_DEBUG
    /// Recorre todo el QString y sustituye los caracteres NO ASCII y
    /// los caracteres que no van muy bien en un XML.

    QString cadena = string;
    QString cadenatmp = "";
    QChar *data = cadena.data();

    /// Cambia tambien otros caracteres no adecuados.
    /*
        cadenatmp.replace ( "&", "&#38;" );
        cadenatmp.replace ( ">", "&#62;" );
        cadenatmp.replace ( "<", "&#60;" );
        cadenatmp.replace ( "\"", "&#34;" );
        cadenatmp.replace ( "\'", "&#39;" );
    */
    int i;
    for ( i = 0; i < cadena.length(); i++ ) {
        if ( data->unicode() > 127 ) {
            cadenatmp = cadenatmp + QString ( "&#" ) + QString::number ( data->unicode() ) + QString ( ";" );
        } else if ( (data->unicode() == 10) | (data->unicode() == 13) ) {
            /// Cambiamos los Intros por el formato HTML.
            cadenatmp = cadenatmp + QString ( "\n" );

        } else if ( data->unicode() == 37 ) { // %
            cadenatmp = cadenatmp + QString ( "&#37;" );
        } else if ( data->unicode() == 38 ) { // &
            cadenatmp = cadenatmp + QString ( "&#38;" );
        } else if ( data->unicode() == 62 ) { // >
            cadenatmp = cadenatmp + QString ( "&#62;" );
        } else if ( data->unicode() == 60 ) { // <
            cadenatmp = cadenatmp + QString ( "&#60;" );
        } else if ( data->unicode() == 34 ) { // "
            cadenatmp = cadenatmp + QString ( "&#34;" );
        } else if ( data->unicode() == 39 ) { // '
            cadenatmp = cadenatmp + QString ( "&#39;" );
        } else {
            cadenatmp = cadenatmp + QString ( *data );
        } // end if
        ++data;
    } // end if
    return cadenatmp;
}


/// Proteje cadenas de texto pasandoles una sustitucion de codigos especiales de XML.
QString blXMLDecode ( const QString &string )
{
    BL_FUNC_DEBUG
    /// Recorre todo el QString y sustituye los caracteres NO ASCII y
    /// los caracteres que no van muy bien en un XML.
    QString cadenatmp = string;

    /// Cambia tambien otros caracteres no adecuados.
    cadenatmp.replace ( "&#38;", "&" );
    cadenatmp.replace ( "&#62;", ">" );
    cadenatmp.replace ( "&#60;", "<" );
    cadenatmp.replace ( "&#34;", "\"" );
    cadenatmp.replace ( "&#39;", "\'" );
    cadenatmp.replace ( "<br />", "\n" );
    cadenatmp.replace ( "<br/>", "\n" );

    for ( int i = 126; i < 256; i++ ) {
        cadenatmp.replace ( "&#" + QString::number ( i ) + ";", QChar ( i ) );
    } // end for

    return cadenatmp;
}


/// Extiende un string a un numero de cuenta sustituyendo los '.' por ceros.
/// cad = Cadena inicial
/// num1 = Numero de digitos totales de la cuenta.
/// Devuelve un QString con la cuenta extendida al nmero de digitos indicado.
QString blExtendStringWithZeros ( QString cad, unsigned int num1 )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "BlFunctions::blExtendStringWithZeros", 0, cad + "--" + QString::number ( num1 ) );
    QString cod = cad;
    int num = num1;
    if ( cod.length() < num ) {
        QString str7 ( num - cod.length() + 1, '0' );
        int pos = cod.indexOf ( ".", 0 );
        if ( pos > 0 ) {
            cod.replace ( pos, 1, str7 );
        } // end if
    } // end if
    
    return ( cod );
}


/// Redondeo de numeros en punto flotante.
/// n = Numero a redondear
/// d = Numero de decimales
/// Devuelve numero redondeado
float blFloatRound ( float n, unsigned d )
{
    BL_FUNC_DEBUG
    return floor ( n * pow ( 10., d ) + .5 ) / pow ( 10., d );
}


/// Esta función convierte un número con decimales a un entero. Usando la regla
/// que si el el primer decimal es mayor o igual a 5 se devuelve el entero superior.
/// value = Número a convertir.
int blDoubleToInt ( double value )
{
    BL_FUNC_DEBUG
    int retorno;
    double mayor = floor ( value );
    if ( ( mayor - value ) >= 0.5 )
        retorno = ( int ) mayor - 1;
    else
        retorno = ( int ) mayor;
    return retorno;
}


/// Procesa el string pasado como parametro y devuelve una estructura del tipo QDate
/// Esta funcion extiende la fecha pasada como parametro
/// QString y devuelve la fecha en formato QDate.
/// fechaintro string con la fecha a ser normalizada.
QDate blNormalizeDate ( QString fechaintro )
{
    BL_FUNC_DEBUG
    QDate fecharesult;
    int d, M, y;
    switch ( fechaintro.length() ) {
    case 4: /// fecha tipo ddMM
        d = fechaintro.mid ( 0, 2 ).toInt();
        M = fechaintro.mid ( 2, 2 ).toInt();
        y = QDate::currentDate().year();
        break;
    case 5:/// fecha tipo dd/MM
        d = fechaintro.mid ( 0, 2 ).toInt();
        M = fechaintro.mid ( 3, 2 ).toInt();
        y = QDate::currentDate().year();
        break;
    case 6: /// fecha tipo ddMMyy
        d = fechaintro.mid ( 0, 2 ).toInt();
        M = fechaintro.mid ( 2, 2 ).toInt();
        y = 2000 + fechaintro.mid ( 4, 2 ).toInt();
        break;
    case 8:
        if ( fechaintro.contains ( "/", Qt::CaseSensitive ) || fechaintro.contains ( "-", Qt::CaseSensitive ) ) {
            /// fecha tipo dd/MM/yy o dd-MM-yy
            d = fechaintro.mid ( 0, 2 ).toInt();
            M = fechaintro.mid ( 3, 2 ).toInt();
            y = 2000 + fechaintro.mid ( 6, 2 ).toInt();
        } else {
            /// o bien tipo ddMMyyyy
            d = fechaintro.mid ( 0, 2 ).toInt();
            M = fechaintro.mid ( 2, 2 ).toInt();
            y = fechaintro.mid ( 4, 4 ).toInt();
        } // end if
        break;
    case 10: /// fecha tipo dd/MM/yyyy
        d = fechaintro.mid ( 0, 2 ).toInt();
        M = fechaintro.mid ( 3, 2 ).toInt();
        y = fechaintro.mid ( 6, 4 ).toInt();
        break;
    default:
        d = QDate::currentDate().day();
        M = QDate::currentDate().month();
        y = QDate::currentDate().year();
    } // end switch
    if ( !fecharesult.setDate ( y, M, d ) )
        fecharesult = QDate::currentDate();
    return ( fecharesult );
}


/// Esta funcion ajusta el codigo pasado al numero de digitos especificado.
/// Para ello busca los ceros intermedios y los amplia hasta que el numero de caracteres sea el deseado.
/// Lo hace a partir del quinto digito por defecto. Aunque este parametro deberia ser configurable.
/// cod = string con el codigo actual.
/// num1 = numero de digitos que debe tener el codigo final.
/// Devuelve un QString con el codigo ajustado.

/// BUG: --- Esta funcion tiene un uso especifico de bulmacont y por eso no
/// deberia estar en bulmalib.
QString blExtendCodeLength ( QString cad, unsigned int num1 )
{
    BL_FUNC_DEBUG
    QString cod = cad;
    unsigned int longcad = cad.length();
    if ( longcad > 4 ) {
        if ( longcad < num1 ) {
            QString str7 ( num1 - longcad, '0' );
            cod = cad.left ( 4 );
            cod += str7;
            cod += cad.right ( longcad - 4 );
        } // end if
        if ( longcad > num1 ) {
            cod = cad.left ( 4 );
            cod += cad.right ( num1 - 4 );
        } // end if
    } // end if
    return ( cod );
}


/// Sustituye cadenas en un archivo.
void blReplaceStringInFile ( QString archivo, QString texto1, QString texto2, QString archivo2 )
{
    QString cadena = " sed -e \"s&" + texto1 + "&" + texto2 + "&g\"  " + archivo + " > " + archivo2 + "";
    int result = system ( cadena.toLatin1().data() );
    if (result == -1) {
	blMsgError(_("Error al ejecutar el comando 'sed' [ blfunctions.cpp->blReplaceStringInFile() ]."));
    } // end if
}


/// En la impresi&oacute;n de documentos con bgtrml2pdf esta funci&oacute;n hace casi todo
/// el trabajo de la invocaci&oacute;n de bgtrml2pdf para evitar trabajo duplicado.
void blCreatePDF ( const QString arch )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "blCreatePDF " + arch, 0 );

    QDir::setCurrent ( g_confpr->value( CONF_DIR_USER ) );
    QString cadsys;

#ifdef Q_OS_WIN32

    cadsys = "\"" + g_confpr->value( CONF_PYTHON ) + "\" \"" + g_confpr->value( CONF_PROGDATA ) + "bgtrml2pdf\\bgtrml2pdf\" " + arch + ".rml > \"" + g_confpr->value( CONF_DIR_USER ) + arch + ".pdf\"";
    cadsys = "\"" + cadsys + "\"";

    int result1 = system ( cadsys.toLatin1() );
    if (result1 == -1) {
	blMsgError(_("Error en PYTHON [ blfunctions->blCreatePDF() ]"));
    } // end if
	
    /// Para MS Windows es necesario el uso de FLIP para convertir los saltos de carro al formato UNIX. En caso contrario los documentos no son legibles por programas como Adobe Acrobat
    /// Visita https://ccrma.stanford.edu/~craig/utility/flip/ para mas informacion sobre este comando y obtener su versión para Windows.
    BlDebug::blDebug ( cadsys, 0 );
    cadsys = "\"" + g_confpr->value( CONF_FLIP ) + "\" -u \"" + g_confpr->value( CONF_DIR_USER ) + arch + ".pdf\"";
    cadsys = "\"" + cadsys + "\"";
    
    int result2 = system ( cadsys.toLatin1().data() );
    if (result2 == -1) {
	blMsgError(_("Error en FLIP [ blfunctions->blCreatePDF() ]"));
    } // end if

    BlDebug::blDebug ( cadsys, 0 );

#else

    cadsys = "bgtrml2pdf " + arch + ".rml > " + arch + ".pdf";
    int result3 = system ( cadsys.toLatin1().data() );
    if (result3 == -1) {
	blMsgError(_("Error en bgtrml2pdf [ blfunctions->blCreatePDF() ]"));
    } // end if

#endif

    
}


/// Genera un ODS a partir de un pys sin abrirlo.
void blCreateODS ( const QString arch )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "blCreateODS " + arch, 0 );
    /// Entramos en el directorio correspondiente
    QDir::setCurrent ( g_confpr->value( CONF_DIR_USER ) );
    QString cadsys;

    /// Borramos algun archivo que pudiera haber
#ifdef Q_OS_WIN32
    QString cadena = g_confpr->value( CONF_DIR_USER );
    cadena.replace("/", "\\");
    cadena = "\"del \"" + arch + ".ods\"\"";
#else
    QString cadena = "rm " + g_confpr->value( CONF_DIR_USER ) + arch + ".ods";
#endif
    int result1 = system ( cadena.toLatin1() );
    if (result1 == -1) {
	blMsgError(_("Error al borrar archivo .ods [ blfunctions->blCreateODS() ]"));
    } // end if

    /// Hacemos la invocacion del python
#ifdef Q_OS_WIN32

    cadena = g_confpr->value( CONF_DIR_USER );
    cadena.replace("/", "\\");

    cadena = "\"cd \"" + cadena + "\" & ";
    cadena = cadena + "\"" + g_confpr->value( CONF_PYTHON ) + "\" \"" + arch + ".pys\"\"";

#else
    cadena = " cd " + g_confpr->value( CONF_DIR_USER ) + "; python " + arch + ".pys";
#endif
    int result2 = system ( cadena.toLatin1() );
    if (result2 == -1) {
	blMsgError(_("Error al ejecutar PYTHON [ blfunctions->blCreateODS() ]"));
    } // end if

}


/// Genera un ODS a partir de un pys usand python. y ademas lo muestra.
void blCreateAndLoadODS ( const QString arch )
{
    BL_FUNC_DEBUG
    blCreateODS ( arch );
    if (QFile::exists(g_confpr->value( CONF_DIR_USER ) + arch + ".ods")) {
      QString cadena = "";
#ifdef Q_OS_WIN32
    if (g_confpr->value( CONF_ODS ).isEmpty()) {
	    /// Abre con el programa por defecto del sistema.
	    cadena = "\"start \"\" \"" + g_confpr->value( CONF_DIR_USER ) + arch +  ".ods" + "\"\"";
    } else {
	    /// Abre con la configuracion forzada.
	    cadena = "\"start \"\" \"" + g_confpr->value( CONF_ODS ) + "\" \"" + g_confpr->value( CONF_DIR_USER ) +  arch + ".ods" + "\"\"";
    } // end if
#else
      cadena = g_confpr->value( CONF_ODS ) + " " + g_confpr->value( CONF_DIR_USER ) + arch + ".ods &";
#endif
      int result = system ( cadena.toLatin1() );
      if (result == -1) {
	  blMsgError(_("Error al ejecutar oocalc [ blfunctions->blCreateAndLoadODS() ]"));
      } // end if
    } // end if

}

/// Genera un 'PDF' a partir de un 'RML' usando 'bgtrml2pdf' y adem&aacute;s lo muestra con el visor
/// de 'PDF' establecido en la configuraci&oacute;n.
/// arch = Nombre del archivo 'RML'.
void blCreateAndLoadPDF ( const QString arch )
{
    BL_FUNC_DEBUG
    blCreatePDF ( arch );

#ifdef Q_OS_WIN32
    QString cadsys = "\"start \"\" \"" + g_confpr->value( CONF_PDF ) + "\" \"" + g_confpr->value( CONF_DIR_USER ) + arch + ".pdf\"\"";
#else
    QString cadsys = g_confpr->value( CONF_PDF ) + " " + g_confpr->value( CONF_DIR_USER ) + arch + ".pdf &";C:
#endif
    
    int result = system ( cadsys.toLatin1().data() );
    if (result == -1) {
	blMsgError(_("Error al ejecutar el visor de PDF [ blfunctions->blCreateAndLoadPDF() ]"));
    } // end if
    
}


/// De momento no se usa, pero sirve para enviar documentos por e-mail a un destinatario.
void blSendPDFMail ( const QString arch, const QString to, const QString subject, const QString message )
{
    BL_FUNC_DEBUG
    //FIXME: REVISAR PARAMETROS de mailsend o la posibilidad de anyadir otros programas
    //para enviar correo desde la ventana de configuracion del programa.
    QString cadsys = "mailsend -h " + arch + " -d " + to + " -f bulmages@iglues.org -t test@iglues.org -sub " + subject + " -m " + message;
    int result = system ( cadsys.toLatin1().data() );
    if (result == -1) {
	blMsgError(_("Error al ejecutar mailsend [ blfunctions->blSendPDFMail() ]"));
    } // end if
    
}


QString blWindowId ( const QString &app )
{
    BL_FUNC_DEBUG
    QString cad = "";

    if ( app != "" ) {
        cad = "xwininfo -int -name \"" + app + "\" | grep xwininfo | awk '{print $4}' > /tmp/xwinfo";
    } else {
        cad = "xwininfo -int | grep Window | awk '{print $4}' > /tmp/xwinfo";
    } // end if

    int result = system ( cad.toLatin1() );
    if (result == -1) {
	blMsgError(_("Error al ejecutar xwininfo [ blfunctions->blWindowId() ]"));
    } // end if
    

    QString winId = "";

    QFile file ( "/tmp/xwinfo" );
    if ( !file.open ( QIODevice::ReadOnly | QIODevice::Text ) )
        return "";

    QTextStream in ( &file );
    if ( !in.atEnd() ) {
        winId = in.readLine();
    } // end if


    file.close();

    return winId;
}


void blDebugOn ()
{
    BL_FUNC_DEBUG
    g_confpr->setValue ( CONF_DEBUG, "TRUE" );
}


void blDebugOff ()
{
    BL_FUNC_DEBUG
    g_confpr->setValue ( CONF_DEBUG, "FALSE" );
}


/// cad = String a presentar como texto de depuracion o como mensaje de error.
/// nivel 0 = normal.
/// nivel 1 = Bajo.
/// nivel 2 = Alto (sale un popup).
/// nivel 4 = Comienza depuracion indiscriminada.
/// nivel 5 = Termina depuracion indiscriminada.
/// nivel 10 = Salida a terminal.
#if CONFIG_DEBUG == true
#ifdef OLD_DEBUG
void blDebug ( const QString &cad, int nivel, const QString &param )
{
    /// Si el objeto confpr no esta creado puede dar segmentation fault.
    if ( g_confpr == NULL ) {
        return;
    } // end if

    static bool semaforo = 0;


    if ( g_confpr->value( CONF_DEBUG ) == "TRUE" ) {
        static QFile file ( g_confpr->value( CONF_DIR_USER ) + "bulmagesout.txt" );
        static QTextStream out ( &file );

        static QFile filexml ( g_confpr->value( CONF_DIR_USER ) + "bulmagesout.xml" );
        static QTextStream outxml ( &filexml );

        static int auxxml = 0;
        static int supnivel = 0;
        static int indice = 0;
        static QString mensajesanulados[7000];
        static QString clasesanuladas[7000];
        static int indiceclases = 0;
        static QTime t;

        if ( !semaforo ) {
            t.start();
            if ( !file.open ( QIODevice::WriteOnly | QIODevice::Text ) )
                return;
            if ( !filexml.open ( QIODevice::WriteOnly | QIODevice::Text ) )
                return;
            semaforo = 1;
        } // end if


        if ( nivel == 5 ) {
            supnivel = 0;
            nivel = 2;
        } // end if
        if ( nivel == 4 ) {
            supnivel = 2;
            nivel = 2;
        } // end if
        if ( nivel == 0 || nivel == 1 ) {
            /// Si la cadena contiene END bajamos el nivel
            if ( !cad.startsWith ( "END" ) ) {
                auxxml ++;
                if ( auxxml > 20 ) auxxml = 1;
            } // end if
            for ( int i = 0; i < auxxml; i++ ) {
                outxml << "    ";
                out << "    ";
            } // end for

            QString cad1 = cad;
            if ( cad.startsWith ( "END" ) ) {
                cad1 = "</" + cad1.remove ( 0, 4 ) + " time=\"" + QString::number ( t.elapsed() ) + "\" result=\"" + param + "\" >";
            } else if ( cad.contains ( " " ) ) {
                cad1 = "    <COMENT value=\"" + cad + " " + param + "\"></COMENT>";
            } else {
                cad1 = "<" + cad1 + " time=\"" + QString::number ( t.elapsed() ) + "\" param=\"" + param + "\" >";
            } // end if

            outxml << cad1  << "\n" << flush;
            out << cad << " " << param << "\n" << flush;
	    
            if ( cad.startsWith ( "END" ) ) {
                auxxml --;
                if ( auxxml < 0 ) auxxml = 20;
            } // end if
        } // end if
        for ( int i = 0; i < indice; i++ ) {
            if ( cad == mensajesanulados[i] ) {
                return;
            } // end if
        } // end for
        for ( int i = 0; i < indiceclases; i++ ) {
            if ( cad.left ( cad.indexOf ( "::" ) ) == clasesanuladas[i] ) {
                return;
            } // end if
        } // end for

        if ( nivel == 2 || ( supnivel == 2 && nivel == 0 ) || nivel == 3 ) {
            out << cad << " " << param << "\n" << flush;
            int err = QMessageBox::information ( NULL,
                                                 _ ( "Informacion de depuracion" ),
                                                 cad + " " + param,
                                                 _ ( "&Continuar" ),
                                                 _ ( "&Omitir" ),
                                                 _ ( "Omitir &clase" ),
                                                 0, 1 );
            if ( err == 1 ) {
                mensajesanulados[indice++] = cad;
            } // end if
            if ( err == 2 ) {
                clasesanuladas[indiceclases++] = cad.left ( cad.indexOf ( "::" ) );
            } // end if
        } // end if

        file.flush();
    }
}
#endif
#endif

void blMsgInfo ( QString cad, QWidget *parent )
{
    BL_FUNC_DEBUG
    QMessageBox msgBox;
    msgBox.information ( parent,
                         _ ( "Informacion del programa" ),
                         cad, _ ( "&Continuar" ),
                         QString::null, 0 );
}


void blMsgWarning ( QString cad, QWidget *parent )
{
    BL_FUNC_DEBUG
    QMessageBox msgBox;
    msgBox.warning ( parent,
                     _ ( "Aviso del programa" ),
                     cad, _ ( "&Continuar" ),
                     QString::null, 0 );
}


void blMsgError ( QString cad, QWidget *parent )
{
    BL_FUNC_DEBUG
    QMessageBox msgBox;
    msgBox.critical ( parent,
                      _ ( "Error del programa" ),
                      cad, _ ( "&Continuar" ),
                      QString::null, 0 );
}


QString blNumberToText ( QString numero, QString moneda, QString singular )
{
    BL_FUNC_DEBUG
    /// Si es 0 el n&uacute;mero, no tiene caso procesar toda la informaci&oacute;n.
    if ( numero == "0" || numero == "00" ) {
        return "cero " + moneda;
    } // end if

    /// En caso que sea un peso, pues igual que el 0 aparte que no muestre el plural "pesos".
    if ( numero == "1" ) {
        return "un " + singular;
    } // end if

    QMap<QString , QString > numeros;
    //numeros["unidad"][0][0] = "cero";
    numeros["unidad10"] = "un";
    numeros["unidad20"] = "dos";
    numeros["unidad30"] = "tres";
    numeros["unidad40"] = "cuatro";
    numeros["unidad50"] = "cinco";
    numeros["unidad60"] = "seis";
    numeros["unidad70"] = "siete";
    numeros["unidad80"] = "ocho";
    numeros["unidad90"] = "nueve";

    numeros["decenas10"] = "diez";
    numeros["decenas20"] = "veinte";
    numeros["decenas30"] = "treinta";
    numeros["decenas40"] = "cuarenta";
    numeros["decenas50"] = "cincuenta";
    numeros["decenas60"] = "sesenta";
    numeros["decenas70"] = "setenta";
    numeros["decenas80"] = "ochenta";
    numeros["decenas90"] = "noventa";
    numeros["decenas110"] = "dieci";
    numeros["decenas111"] = "once";
    numeros["decenas112"] = "doce";
    numeros["decenas113"] = "trece";
    numeros["decenas114"] = "catorce";
    numeros["decenas115"] = "quince";
    numeros["decenas21"] = "veinti";
    numeros["decenas31"] = "treinta y ";
    numeros["decenas41"] = "cuarenta y ";
    numeros["decenas51"] = "cincuenta y ";
    numeros["decenas61"] = "sesenta y ";
    numeros["decenas71"] = "setenta y ";
    numeros["decenas81"] = "ochenta y ";
    numeros["decenas91"] = "noventa y ";

    numeros["centenas10"] = "cien";
    numeros["centenas20"] = "doscientos ";
    numeros["centenas30"] = "trecientos ";
    numeros["centenas40"] = "cuatrocientos ";
    numeros["centenas50"] = "quinientos ";
    numeros["centenas60"] = "seiscientos ";
    numeros["centenas70"] = "setecientos ";
    numeros["centenas80"] = "ochocientos ";
    numeros["centenas90"] = "novecientos ";
    numeros["centenas11"] = "ciento ";

    QMap<QString, QString> postfijos;
    postfijos["1-0"] = "";
    postfijos["10-0"] = "";
    postfijos["100-0"] = "";
    postfijos["1000-0"] = " mil ";
    postfijos["10000-0"] = " mil ";
    postfijos["100000-0"] = " mil ";
    postfijos["1000000-0"] = " millon ";
    postfijos["10000000-0"] = " millon ";
    postfijos["100000000-0"] = " millon ";
    postfijos["1000000-1"] = " millones ";
    postfijos["10000000-1"] = " millones ";
    postfijos["100000000-1"] = " millones ";

    numero = numero.replace ( ",", "." );
    QString decimal_break = ".";
    //echo "test run on ".$numero."<br>";
    QString entero = numero.split ( decimal_break ).at ( 0 );
    QString decimal = numero.split ( decimal_break ).at ( 1 );

    if ( decimal == "" ) {
        decimal = "00";
    } // end if
    if ( decimal.size() < 2 ) {
        decimal = decimal + "0";
    } // end if
    if ( decimal.size() > 2 ) {
        decimal = decimal.right ( 2 );
    } // end if
    //echo "entero ".$entero."<br> decimal ".$decimal."<br>";

    QString entero_breakdown = entero;

    QString breakdown_key = "1000000000000";
    QString num_string = "";
    QMap<QString, QString> breakdown;
    QString chundreds = "";
    QString tens = "";
    QString ctens = "";
    QString cctens = "";
    QString ones = "";
    QString cpostfijos = "";
    while ( breakdown_key.toDouble() > 0.5 ) {
        breakdown["entero" + breakdown_key + "number"] = /*floor(*/ QString::number ( entero_breakdown.toLongLong() / breakdown_key.toLongLong() );

        if ( breakdown["entero" + breakdown_key+"number"].toLongLong() > 0 ) {
            //echo " further process <br>";
            breakdown["entero" + breakdown_key+"100"] = /*floor(*/ QString::number ( breakdown["entero" + breakdown_key + "number"].toLongLong() / 100 );
            breakdown["entero" + breakdown_key+"10"] = /*floor( */ QString::number ( ( breakdown["entero" + breakdown_key + "number"].toLongLong() % 100 ) / 10 );
            breakdown["entero" + breakdown_key+"1"] = /*floor(*/   QString::number ( breakdown["entero" + breakdown_key + "number"].toLongLong() % 10 );

            QString hundreds = breakdown["entero" + breakdown_key + "100"];
            // if not a closed value at hundredths
            if ( ( breakdown["entero" + breakdown_key + "10"].toLongLong() + breakdown["entero"+breakdown_key+"1"].toLongLong() ) > 0 ) {
                chundreds = "1";
            } else {
                chundreds = "0";
            } // end if

            if ( numeros.contains ( "centenas" + hundreds + chundreds ) ) {
                num_string += numeros["centenas" + hundreds + chundreds];
            } else {
                if ( numeros.contains ( "centenas" + hundreds + "0" ) ) {
                    num_string += numeros["centenas" + hundreds + "0"];
                }
            } // end if

            if ( ( breakdown["entero" + breakdown_key + "1"].toLongLong() ) > 0 ) {
                ctens = "1";
                tens = breakdown["entero" + breakdown_key + "10"];
                if ( breakdown["entero" + breakdown_key + "10"].toLongLong() == 1 ) {
                    if ( breakdown["entero" + breakdown_key + "1"].toLongLong() < 6 ) {
                        cctens = breakdown["entero" + breakdown_key + "1"];
                        num_string += numeros["decenas" + tens + ctens + cctens];
                    } else {
                        num_string += numeros["decenas" + tens + ctens + "0"];
                    } // end if
                } else {
                    //echo " decenas ".numeros["decenas"][$tens][$ctens]."<br>";
                    if ( numeros.contains ( "decenas" + tens + ctens ) ) {
                        num_string += numeros["decenas" + tens + ctens];
                    }
                } // end if
            } else {
                ctens = "0";
                tens = breakdown["entero" + breakdown_key + "10"];
                if ( numeros.contains ( "decenas" + tens + ctens ) ) {
                    num_string += numeros["decenas" + tens + ctens];
                } // end if
            } // end if

            if ( cctens == "" ) {
                ones = breakdown["entero" + breakdown_key + "1"];
                if ( numeros.contains ( "unidad" + ones + "0" ) ) {
                    num_string += numeros["unidad" + ones + "0"];
                } // end if
            } // end if

            cpostfijos = "-1";
            if ( breakdown["entero" + breakdown_key + "number"].toLongLong() > 1 ) {
                cpostfijos = "1";
            } // end if

            if ( postfijos.contains ( breakdown_key + "-" + cpostfijos ) ) {
                num_string += postfijos[breakdown_key + "-" + cpostfijos];
            } else {
                num_string += postfijos[breakdown_key + "-0"];
            } // end if
        } // end if
        cctens = "";
        entero_breakdown = QString::number ( entero_breakdown.toInt() % breakdown_key.toLongLong() );
        breakdown_key = QString::number ( breakdown_key.toLongLong() / 1000 );

    } // end while
    if ( decimal != "0" && decimal != "00" ) {
        return num_string + " " + moneda + " con " + blNumberToText ( decimal + ".00", " centimos", " centimo" );
    } else {
        return num_string + " " + moneda;
    } // end if
}


void blCenterOnScreen ( QWidget *ventana )
{
    BL_FUNC_DEBUG
    QRect rect;
    QDesktopWidget *escritorio = new QDesktopWidget();
    rect = escritorio->availableGeometry();
    ventana->move ( rect.center() - ventana->rect().center() );
    delete escritorio;
}


QString blStringToUsAscii ( const QString &orig )
{
    BL_FUNC_DEBUG
    /// Recorre todo el QString y sustituye los caracteres NO ASCII y
    /// los caracteres que no van muy bien en un XML.

    QString cadena = orig;
    QString cadenatmp = "";


    /// Cambia tambien otros caracteres no adecuados.
    cadena = cadena.replace ( "a", "a" );
    cadena = cadena.replace ( "e", "e" );
    cadena = cadena.replace ( "i", "i" );
    cadena = cadena.replace ( "o", "o" );
    cadena = cadena.replace ( "u", "u" );

    cadena = cadena.replace ( "à", "a" );
    cadena = cadena.replace ( "è", "e" );
    cadena = cadena.replace ( "ì", "i" );
    cadena = cadena.replace ( "ò", "o" );
    cadena = cadena.replace ( "ù", "u" );

    cadena = cadena.replace ( "ä", "a" );
    cadena = cadena.replace ( "ë", "e" );
    cadena = cadena.replace ( "ï", "i" );
    cadena = cadena.replace ( "ö", "o" );
    cadena = cadena.replace ( "ü", "u" );

    cadena = cadena.replace ( "â", "a" );
    cadena = cadena.replace ( "ê", "e" );
    cadena = cadena.replace ( "î", "i" );
    cadena = cadena.replace ( "ô", "o" );
    cadena = cadena.replace ( "û", "u" );

    cadena = cadena.replace ( "ny", "n" );
    cadena = cadena.replace ( "ç", "c" );

    cadena = cadena.replace ( "&", "." );

    QChar *data = cadena.data();
    int i;
    for ( i = 0; i < cadena.length(); i++ ) {
        if ( data->unicode() <= 127 ) {
            cadenatmp = cadenatmp + QString ( *data );
        } // end if
        ++data;
    } // end if
    return cadenatmp;
}

/// Trata una cadena para ser escrita dentro de un fichero en Python sin que esta
/// rompa la sintaxis relacionada con el indentado del lenguaje
QString blStringToPython ( QString string, bool eraseQuotes )
{
    BL_FUNC_DEBUG
    string = string.replace ( "\n", " " );
    string = string.replace ( "\t", " " );

    if (eraseQuotes) {
      /// Se eliminan las comillas simples o dobles.
      string = string.replace ( "\'", " " );
      string = string.replace ( "\"", " " );
    } else {
      string = string.replace ( "\'", "\\\'" );
      string = string.replace ( "\"", "\\\"" );
    } // end if
    return string;
}


/// La tabla de conversion de codigos para el CIF
QChar g_spainCIFCode[] = {QChar ( 'A' ), QChar ( 'B' ), QChar ( 'C' ), QChar ( 'D' ), QChar ( 'E' ), QChar ( 'F' ), QChar ( 'G' ), QChar ( 'H' ), QChar ( 'J' ), QChar ( 'P' ), QChar ( 'Q' ), QChar ( 'R' ), QChar ( 'S' ), QChar ( 'U' ), QChar ( 'V' ), QChar ( 'N' ), QChar ( 'W' ) };

char g_validateSpainCIFCode[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

/// La tabla de conversion de codigos para el NIF
char g_spainNIFCode[] = {'T', 'R', 'W', 'A', 'G', 'M', 'Y', 'F', 'P', 'D', 'X', 'B', 'N', 'J', 'Z', 'S', 'Q', 'V', 'H', 'L', 'C', 'K', 'E', 'T'};


/// Suma los digitos de un numero  y devuelve el resultado.
/// por ejemplo blSumAllDigits(56) devolveria 5 + 6 = 11
int blSumAllDigits ( int val )
{
    BL_FUNC_DEBUG
    QString cad = QString::number ( val );
    int res = 0;
    for ( int i = 0; i < cad.size(); i++ ) {
        res += cad[i].digitValue();
    } // end for
    return res;
}


/// Dependiendo de lo que se haya introducido distingue si es un CIF o un NIF y valida
bool blValidateSpainCIFNIFCode ( QString nifcif, QChar &digit )
{
    BL_FUNC_DEBUG
    if ( nifcif[0].isDigit() )
        return blValidateSpainNIFCode ( nifcif, digit );
    return blValidateSpainCIFCode ( nifcif, digit );
}


/// Valida un nif de 9 digitos. Si el nif pasado es menor de 5 digitos
bool blValidateSpainNIFCode ( QString nif1, QChar &digit )
{
    BL_FUNC_DEBUG
    /// Quitamos los caracteres raros.
    QString nif = nif1.replace ( "-", "" );
    nif = nif.replace ( ".", "" );
    nif = nif.replace ( "_", "" );
    nif = nif1.left ( 8 );

    /// Si el CIF tiene menos de 4 caracteres validamos. Ya que igual queremos permitir CIF's Inventados.
    if ( nif1.size() < 5 ) {
        return true;
    } // end if

    int modulo = nif.toInt() % 23;
    digit = QChar ( g_spainNIFCode[modulo] );
    if ( nif1[8] == QChar ( g_spainNIFCode[modulo] ) )
        return true;
    return false;

}


//
bool blValidateSpainCIFCode ( QString cif1, QChar &digit )
{
    BL_FUNC_DEBUG
    // ESPANYA EFECTIVIDAD A PARTIR DEL 1 JULIO 2008:
    // Orden EHA/451/2008 de 20 de febrero.
    // BOE Numero: 49 de martes, 26 febrero de 2008. Pagina: 11374.
    // ------------------------------------------------------------
    // A. Sociedades anonimas.
    // B. Sociedades de responsabilidad limitada.
    // C. Sociedades colectivas.
    // D. Sociedades comanditarias.
    // E. Comunidades de bienes y herencias yacentes.
    // F. Sociedades cooperativas.
    // G. Asociaciones.
    // H. Comunidades de propietarios en regimen de propiedad horizontal.
    // J. Sociedades civiles, con o sin personalidad juridica.
    // P. Corporaciones Locales.
    // Q. Organismos publicos.
    // R. Congregaciones e instituciones religiosas.
    // S. Organos de la Administracion del Estado y de las Comunidades Autonomas.
    // U. Uniones Temporales de Empresas.
    // V. Otros tipos no definidos en el resto de claves.
    //
    // N. Entidad extranjera. Con o sin personalidad juridica.
    // W. Establecimiento permanente de entidad no residente en territorio espanyol.

    QString cif = cif1.replace ( "-", "" );
    cif = cif.replace ( ".", "" );
    cif = cif.replace ( "_", "" );
    /// Si el CIF tiene menos de 4 caracteres validamos. Ya que igual queremos permitir CIF's Inventados.
    if ( cif.size() < 5 ) {
        return true;
    } // end if
    int valpar = cif[2].digitValue() + cif[4].digitValue() + cif[6].digitValue();
    int valimpar = blSumAllDigits ( cif[1].digitValue() * 2 ) + blSumAllDigits ( cif[3].digitValue() * 2 ) + blSumAllDigits ( cif[5].digitValue() * 2 ) + blSumAllDigits ( cif[7].digitValue() * 2 );
    int sumapar = valpar + valimpar;
    QString textsumapar = QString::number ( sumapar );
    int c = textsumapar.right ( 1 ).toInt();
    int d = 10 - c;
    if ( cif[0] == 'N' || cif[0] == 'R' || cif[0] == 'K' || cif[0] == 'P' || cif[0] == 'Q' || cif[0] == 'S' || cif[0] == 'W' ) {
        digit = g_spainCIFCode[d-1];
        if ( cif[8] == g_spainCIFCode[d-1] ) {
            return true;
        } else {
            return false;
        } // end if
    } // end if
    if ( cif[0] == 'A' || cif[0] == 'B' || cif[0] == 'C' || cif[0] == 'D' || cif[0] == 'E' || cif[0] == 'F' || cif[0] == 'G' || cif[0] == 'H' || cif[0] == 'I' || cif[0] == 'J' || cif[0] == 'U' || cif[0] == 'V' ) {
        digit = QString::number ( d % 10 ) [0];
        if ( cif[8].digitValue() == d % 10 ) {
            return true;
        } else {
            return false;
        } // end if
    } //end if
    if ( cif[8] == QChar ( g_validateSpainCIFCode[d-1] ) || cif[8].digitValue() == d % d ) {
        return true;
    } //end if
    return false;
}


void blRawPrint(const QString &archivo, bool diruser, const QString &defprinter)
{
      BL_FUNC_DEBUG
      QString printer = "";
      if (defprinter.isEmpty()) {
	  printer = g_confpr->value(CONF_CUPS_DEFAULT_PRINTER);
      } else {
	  printer = defprinter;
      } // end if

      QString dir = "";
      if (diruser) {
	      dir = g_confpr->value(CONF_DIR_USER);
      } // end if
      
      #ifndef Q_OS_WIN32
	      QString comando = "lp -d" + printer + " " + dir + archivo;
	      system ( comando.toLatin1().data() );
      #else
	      /* TODO: Eliminar CONF_SPOOL porque ya se imprime en 'raw' directamente desde BulmaLib.
	       *
	      QString comando = "\"" + g_confpr->value(CONF_SPOOL)  + "\" \"" + dir + archivo + "\" " + printer;
	      comando.replace("/","\\");
	      comando = "\"" + comando + "\"";
	      system ( comando.toLatin1().data() );
	      */

	      /// Lee el contenido del archivo y lo manda a la impresora.
	      QFile file(dir + archivo);
	      if (file.open(QIODevice::ReadOnly)) {
	      	QByteArray blob = file.readAll();
        	BlRawDataToPrinter(printer, blob);
    
	      } else {
		// No se ha podido leer el archivo.
	      } // end if

      #endif

}


int blWebBrowser(const QString &uri, const QString &defbrowser) {
    BL_FUNC_DEBUG
    QString browser = "";
    if (browser.isEmpty()) {
        browser = g_confpr->value(CONF_NAVEGADOR);
    } // end if
    
    #ifndef Q_OS_WIN32
        QString commas = " ";
        QString inbackground = " &";
    #else
        QString commas= "\" ";
        QString inbackground = "";
    #endif
    
    if (browser.isEmpty()) {
        QDesktopServices::openUrl(QUrl(uri, QUrl::TolerantMode));
        return 0;
    }
    
    else {
        QString webcommand = commas + browser + commas + QUrl(uri, QUrl::TolerantMode).toString() + inbackground;
        int result = system ( webcommand.toLatin1().data() );
        return result;
    } // end if

}


QString blGetEnv( const char *varName )
{
    BL_FUNC_DEBUG
    QString result {""};

#ifdef Q_OS_WIN32
    /// En MS-Windows hay problemas al pasar a la codificacion UTF-8.
    DWORD length {GetEnvironmentVariableW(BlQStringToWCHAR(varName), NULL, 0)};

    wchar_t *variable {new wchar_t[length]};

    if (GetEnvironmentVariableW(BlQStringToWCHAR(varName), variable, length ))
    {
        /// Found and there are no errors.
        result = QString::fromWCharArray(variable);
    } // end if

#else
    result = QString( qgetenv(varName).constData() );
#endif

    return result;
}


#ifdef Q_OS_WIN32
/// Converts QString to WCHAR (MS-Windows only function).
wchar_t* BlQStringToWCHAR (QString inString) {
    BL_FUNC_DEBUG

	if (inString.isEmpty())
		return NULL;

	std::wstring str = inString.toStdWString ();

	wchar_t *outString = new wchar_t[inString.size () + 1];
    	unsigned int length = inString.toWCharArray (outString);

	outString[length] = '\0';

	return outString;
}


bool BlRawDataToPrinter(QString printerName, QByteArray dataToPrint)
{
    BL_FUNC_DEBUG

	HANDLE printerHandle;
	DOC_INFO_1W documentInfo;
	LPBYTE rawBlob;
	DWORD printerJob;
        DWORD bytesWritten;
	DWORD bytesToWrite = dataToPrint.size();

	/// Converts data to DWORD format.
	rawBlob = reinterpret_cast<BYTE*>(dataToPrint.data());
	wchar_t *selectedPrinter = BlQStringToWCHAR(printerName);

	/// Printer handle.
	if ( OpenPrinterW( selectedPrinter, &printerHandle, NULL ) ) {

	    /// Info about document to print.
	    documentInfo.pDocName = BlQStringToWCHAR("Document in raw format");
	    documentInfo.pDatatype = BlQStringToWCHAR("RAW");
	    documentInfo.pOutputFile = NULL;

	    /// Start document.
	    if ( (printerJob = StartDocPrinterW( printerHandle, 1, (BYTE*)&documentInfo )) != 0 ) {

	      /// New page.
	      if ( StartPagePrinter( printerHandle ) ) {

		/// Send data to printer.
		if ( WritePrinter( printerHandle, rawBlob, bytesToWrite, &bytesWritten ) ) {

		    /// End page.
		    if ( EndPagePrinter( printerHandle ) ) {

		      /// End document.
		      if ( EndDocPrinter( printerHandle ) ) {

			  /// Close printer.
			  ClosePrinter( printerHandle );

			  /// Last check.
			  if ( bytesWritten == bytesToWrite ) {
				  return true;
			  } // end if

		      } else {
			  ClosePrinter( printerHandle );
		      } // end if

		    } else {
			EndDocPrinter( printerHandle );
			ClosePrinter( printerHandle );
		    } // end if

		} else {
		    EndPagePrinter( printerHandle );
		    EndDocPrinter( printerHandle );
		    ClosePrinter( printerHandle );
		} // end if

	      } else {
		  EndDocPrinter( printerHandle );
		  ClosePrinter( printerHandle );
	      } // end if

	    } else {
		ClosePrinter( printerHandle );
	    } // end if
	    
	} // end if

	return false;
}

#endif



bool blCopyFile( const QString &oldName, const QString &newName )
{
    ///Funcion para copiar archivos, pudiendo mover entre particiones y con direcciones tolerantes multiplataforma.
    BL_FUNC_DEBUG

    // Si es el mismo archivo, no lo copia.
    if(oldName.compare(newName) == 0)
    {
        return true;
    } // end if

    /// Transforma las direcciones en tolerantes, para  que funcione en totas las plataformas.
    QString oldFile = QDir::toNativeSeparators(oldName);
    QString newFile = QDir::toNativeSeparators(newName);

    /// Intenta copiar usando QFile::copy() y si falla, lo copia mediante sistema
    if (!QFile::copy(oldFile, newFile))
    {
        QString command = "";

        #ifdef Q_OS_WIN32
            command = "copy \"" + oldFile + "\" \"" + newFile + "\"";
            command.replace("/", "\\");
            command = command + " /y";
        #else
            command = "cp \"" + oldFile + "\" \"" + newFile + "\"";
        #endif

        BlDebug::blDebug(command);

        int result = system ( command.toLatin1().data() );

        if (result == -1) return false;

    } // end if

    // Se ha copiado correctamente el archivo
    return true;
}


bool blRemove(const QString &filetoremove)
{
    /// Function to remove files independent.
    BL_FUNC_DEBUG

    QString removeFile = QDir::toNativeSeparators(filetoremove);
    QDir rootDir;
    
    // Intenta cerrar el archivo y eliminarlo
    if(!QFile::remove(removeFile) )
    {
        //Si falla, recurre a QDir::remove()
        if(!rootDir.remove (removeFile))
        {
	QString command = "";

	#ifdef Q_OS_WIN32
	    command = "del \"" + removeFile + "\"";
		command.replace("/", "\\");
	#else
	    command = "rm " + removeFile;
	#endif

        int result = system ( command.toLatin1().data() );
        if (result == -1) {
            return false;
            } // end if

        } // end if
    } // end if

    // Se ha eliminado correctamente el archivo
    return true;
    
}


/// Funci&oacute;n para borrar un directorio y todo su contenido
bool blRemoveDirectory( const QString &directorytoremove )
{
    BL_FUNC_DEBUG

    bool res = true;

    QString removeDirectory = QDir::toNativeSeparators(directorytoremove);
    QDir dir ( removeDirectory );

    /// The directory does not exists or it's a file
    if ( !dir.exists() )
        return true;

    /// Remove each file in the folder, and for the folders it calls this function again
    foreach ( QFileInfo fi,
              dir.entryInfoList ( QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst ) ) {
        if ( fi.isDir() )
            res = blRemoveDirectory ( fi.absoluteFilePath () );
        else
            res = blRemove ( fi.absoluteFilePath() );

        if ( !res )
            return false;
    } // end foreach

    /// When the directory is empty, we can delete it
    res = dir.rmdir ( removeDirectory );

    return res;
}


bool blMoveFile( const QString &oldName, const QString &newName )
{
    ///Funcion para mover archivos, pudiendo mover entre particiones y con direcciones tolerantes multiplataforma.
    BL_FUNC_DEBUG
    QDir rootDir;

    // Transforma las direcciones en tolerantes, para  que funcione en totas las plataformas.
    QString oldFile = QDir::toNativeSeparators(oldName);
    QString newFile = QDir::toNativeSeparators(newName);

    // Intenta usar Qdir::rename()
    if(!rootDir.rename(oldName, newName) )
    {
        //Si falla, recurre a blCopy y luego lo elimina

        // Copiado fallado, no elimina el original
        if(!blCopyFile(oldName, newName))
        return false;

        // Copiado exitoso, elimina el original
        blRemove(oldName);
    } // end if

    // Se ha movido correctamente el archivo, o se ha copiado y eliminado el original
    return true;
}


/// FUNCIONES DE MANEJO DEL CORREO ELECTRONICO
/// ==========================================

///
/**
\ param recipient
\ param subject
\ param body
\ param attached
\ return
**/
int Thunderbird ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached )
{
    BL_FUNC_DEBUG
    QString dir_email = g_confpr->value( CONF_EMAIL_CLIENT );
    
    QString runcommand = QString(CAD_COMILLAS + dir_email + CAD_COMILLAS);
    runcommand += " -compose ";
    runcommand += "to='" + recipient + "',";
    runcommand += "bcc='" + bcc + "',";
    runcommand += "subject='" + subject + "',";
    body.replace(",","&#44;"); // convertim les comes amb el seu valor html, del contrari thunderbird ens talla el missatge.
    runcommand += "body='" + body + "',";
    if (attached != "") {
        runcommand += "attachment='file://" + attached + "'";
    }
            
    system(QString( runcommand + background).toLatin1());
    
    return 0;
}

///
/**
\ param recipient
\ param subject
\ param body
\ param attached
\ return
**/
int Kmail ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached )
{
    BL_FUNC_DEBUG
    QString dir_email = g_confpr->value( CONF_EMAIL_CLIENT );
    
    QString runcommand = QString(CAD_COMILLAS + dir_email + CAD_COMILLAS);
    if (subject != "")
      runcommand += " -s \"" + subject + "\"";
    if (bcc != "") 
      runcommand += " -b " + bcc;
    if (body != "")
      runcommand += " --body \"" + body + "\"";
    if (attached != "") 
      runcommand += " --attach " + attached;
  
    runcommand += " " + recipient;
            
    system(QString( runcommand + background).toLatin1());
    
    return 0;
}
///
/**
\ param recipient
\ param subject
\ param body
\ param attached
\ return
**/
int Evolution ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached )
{
    BL_FUNC_DEBUG
    QString dir_email = g_confpr->value( CONF_EMAIL_CLIENT );
    
    
    QString urlmail = " \"mailto:" + recipient;
    urlmail += "?subject=" + subject;
    urlmail += "&bcc=" + bcc;
    urlmail += "&body=" + body;
    if (attached != "") {
        urlmail += "&attach=" + attached + "\"";
    }
    QString runcommand = QString(CAD_COMILLAS + dir_email + QUrl(urlmail, QUrl::TolerantMode).toString() + CAD_COMILLAS);
    system(QString( runcommand + background).toLatin1());
    return 0;

}

///
/**
\ param recipient
\ param subject
\ param body
\ param attached
\ return
**/
#ifdef Q_OS_WIN32
int Outlook ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached )
{
    BL_FUNC_DEBUG
    QString dir_email = g_confpr->value( CONF_EMAIL_CLIENT );
    QString barra2;
    
    QString urlmail = " /c ipm.note";
    if (attached != "") {
        urlmail += " /a " + attached;
    }
    urlmail += " /m " + recipient;
    QByteArray barray = QUrl(subject, QUrl::TolerantMode).toEncoded();
    QByteArray barray2 = QUrl(body, QUrl::TolerantMode).toEncoded();
    QString stringed(barray);
    QString stringed2(barray);
    urlmail += "?subject=" + stringed;
    urlmail += "&body=" + stringed2;
    
    QString runcommand = QString(CAD_COMILLAS + dir_email + CAD_COMILLAS + CAD_COMILLAS + urlmail + CAD_COMILLAS);
    
    system(QString( runcommand + background).toLatin1());
    return 0;

}
#endif

///
/**
\ param recipient
\ param subject
\ param body
\ param attached
\ return
**/
int blSendEmail ( QString &recipient, QString &bcc, QString &subject, QString &body, QString &attached )
{
    BL_FUNC_DEBUG
    
    QString dir_email = g_confpr->value( CONF_EMAIL_CLIENT );
      
    QFileInfo fileinfo(dir_email);
    QString program_name = fileinfo.fileName();
    #ifdef Q_OS_WIN32
        program_name.replace(".exe", "");
    #endif
    
    if (program_name == "thunderbird") {
        Thunderbird (  recipient, bcc, subject, body, attached );
    } else if (program_name == "kmail") {
        Kmail (  recipient, bcc, subject, body, attached );
    } else if (program_name == "evolution") {
        Evolution (  recipient, bcc, subject, body, attached );
    }
    #ifdef Q_OS_WIN32
     else if (program_name == "Outlook") {
        Outlook (  recipient, bcc, subject, body, attached );
    } // end if
    #endif
    
    return 0;
    
}



bool isPluginActive(const QString &theplugin)
{
    BL_FUNC_DEBUG

    QStringList loaded;
    bool value = false;

    /// Checkeamos si el plugin deseado esta o no cargado
    for (int i = 0; i < g_plugins->pluginsLoaded().size(); ++i)
    {
        /// Convertimos el nombre en un QFileInfo para poder saber el nombre real, ignorando si es un .dll o un .so (deteccion multiplataforma)
        QFileInfo stringplugin(g_plugins->pluginsLoaded().at(i)->fileName());
        loaded << stringplugin.baseName();
        loaded.removeDuplicates();
    }

    /// Si existe el plugin, devuelve verdadero
    if ( loaded.contains(theplugin) )
    {
        value = true;
    }
    return value;
}


bool blStartDetachedProcess(const QString path, const QString command, const QList<QString> checkExistingFiles, const bool forceNewConsole, QProcessEnvironment *env)
{
  BL_FUNC_DEBUG

  bool canRun = true;
  int errorCode = 0;

  for (int i = 0; i < checkExistingFiles.size(); ++i)
  {
    QString fileName = checkExistingFiles.at(i);
    QFileInfo file(fileName);
    if (!file.exists())
    {
      BlDebug::blDebug ( "blStartDetachedProcess", 0, QString("File not found: ") + fileName );
      errorCode = 404;
      canRun = false;
      break;
    } // end if
  } // end for

  if (canRun)
  {
    /// Run command
#ifdef Q_OS_WIN32
    LPVOID env_block;
    /// Get system environment
    if (CreateEnvironmentBlock(&env_block, NULL, false))
    {
      STARTUPINFOW info = {0};
      PROCESS_INFORMATION processInfo;
    
      int createNewConsole = 0;
      if (forceNewConsole) createNewConsole = CREATE_NEW_CONSOLE;

      wchar_t wPath[path.size() + 1];
      path.toWCharArray(wPath);
      wPath[path.length()] = '\0';

      wchar_t wCommand[command.size() + 1];
      command.toWCharArray(wCommand);
      wCommand[command.length()] = '\0';

      if (CreateProcessW(wPath, wCommand, NULL, NULL,
                      true, createNewConsole | CREATE_UNICODE_ENVIRONMENT, env_block, NULL, &info, &processInfo)) {
        CloseHandle(processInfo.hProcess); // Cleanup since you don't need this
        CloseHandle(processInfo.hThread); // Cleanup since you don't need this
        DestroyEnvironmentBlock(env_block);
      } else {
        /// Error 
        errorCode = GetLastError();
        canRun = false;
      } // end if
    } else {
      /// Error
      errorCode = 405;
      canRun = false;
    } // end if

#else
    QProcess process;

    if (env)
    {
        process.setProcessEnvironment(*env);
    }
    else
    {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        process.setProcessEnvironment(env);
    } // end if

    canRun = process.startDetached(command);
    if (!canRun) errorCode = 406;
#endif
  } // end if

  /// TODO check that process was really started
  if (errorCode != 0)
  {
    blMsgError(_("No se ha podido ejecutar el comando. Error #") + QString::number(errorCode));
  } // end if

  return canRun;
}
