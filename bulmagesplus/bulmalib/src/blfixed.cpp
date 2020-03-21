/***************************************************************************
 *   Author: Philip Erdelsky's <pje@efgh.com>                              *
 *   http://efgh.com/software/fixed.htm                                    *
 *   Licence: Public Domain                                                *
 *   December 2, 1998                                                      *
 *   Modified by: Tomeu Borras Riera.                                      *
 *   Modified by: Fco. Javier M. C. (2008)                                 *
 ***************************************************************************/

#include <QtGlobal>
#include <QString>
#include <QChar>
#include <QLocale>

#include "stdio.h"

#include "blfixed.h"


///
/**
\param x
\param p
**/
BlFixed::BlFixed ( int x, int p )
{
    BL_FUNC_DEBUG

    m_data = QDecNumber(x);
    m_precision = p;
/*
    value = x;
    m_precision = p;
*/
}

BlFixed::BlFixed(int x)
{
    m_data = QDecNumber(x);
    m_precision = 0;
}

///
/**
\param a
**/
BlFixed::BlFixed ( QString a )
{
    BL_FUNC_DEBUG
    fromBlFixed ( a.toLatin1() );
}


///
/**
\param a
**/
BlFixed::BlFixed ( const char *a )
{
    BL_FUNC_DEBUG
    fromBlFixed ( a );
    
}


///
/**
**/
BlFixed::BlFixed()
{
    BL_FUNC_DEBUG
    
    m_data = QDecNumber(0);
}


BlFixed::scale BlFixed::SCALE;


BlFixed::scale::scale(void)
{
  int i;
  BlFixed_numerator n;
  n = 1;
  for (i = 0; i <= MAX_FIXED_PRECISION; i++)
  {
    x[i] = n;
    n = 10 * n;
  }
}


BlFixed operator + ( BlFixed x, BlFixed y )
{
    x.m_data = x.m_data + y.m_data;
    return x;
}



BlFixed operator + ( BlFixed x, int y )
{
    x.m_data = x.m_data + QDecNumber(y);
    return x;
}


BlFixed operator + ( int x, BlFixed y )
{
    y.m_data = QDecNumber(x) + y.m_data;
    return y;
}




BlFixed BlFixed::operator = ( BlFixed x )
{
    m_data = x.m_data;
    m_precision = x.m_precision;
    return *this;
}


BlFixed BlFixed::operator = ( int x )
{
    m_data = QDecNumber(x);
    m_precision = 0;
    return *this;
}


BlFixed operator / ( BlFixed x, BlFixed y )
{
    x.m_data = x.m_data / y.m_data;

    return x;
/*
    int pres = x.precision() > y.precision() ? x.precision() : y.precision();
    x.setPrecision(6);
    y.setPrecision(6);
    x.value = (x.value * BlFixed::SCALE.x[y.m_precision]) / y.value;
    x.setPrecision(pres);
    return x;
*/
}


BlFixed operator / ( BlFixed x, int y )
{
    x.m_data = x.m_data / QDecNumber(y);
    return x;
}


BlFixed operator / ( int x, BlFixed y )
{
    y.m_data = QDecNumber(x) / y.m_data;
    return y;
}


BlFixed operator / ( BlFixed x, double y )
{
    x.m_data = x.m_data / QDecNumber(y);
    return x;
}


BlFixed operator / ( double x, BlFixed y )
{
    y.m_data = QDecNumber(x) / y.m_data;
    return y;
}


///
/**
\param separadorDecimal: '0' usa el decimalPoint de las locales
\param precision
\param separadorMillares: '0' usa el groupSeparator de las locales
\return
**/
QString BlFixed::toQString ( QChar separadorDecimal, int precision, QChar separadorMillares )
{
    BL_FUNC_DEBUG
 
    QString result("");


    QLocale locale;

	/// Si no se pasa separador decimal cogemos el de las locales
	if (separadorDecimal == '0')
	{
		separadorDecimal = locale.decimalPoint ();
	} // end if

    QByteArray ba;
    if (precision < 0)
    {
        ba = m_data.rescale(-m_precision).toString();
    } else {
        ba = m_data.rescale(-precision).toString();
    } // end if
 
    result = QString(ba);
    result.replace(".", separadorDecimal);

 
    return result;
    
    
    BlDebug::blDebug ( "BlFixed::toQString", 0, QString::number(precision) );

    /// Si no hay una precision establecida cogemos por defecto la precision del numero a mostrar
    if (precision < 0)  {
        precision = m_precision;
        BlDebug::blDebug("la precision ha cambiado a " + QString::number(m_precision));
    } // end if


/*
	/// Si no se pasa separador de millares cogemos el de las locales
	if  ( separadorMillares == '0' ) { //ARON
		separadorMillares = locale.groupSeparator (); //ARON
	} // end if
*/

    setPrecision ( precision );
    int options = COMMAS;
    BlFixed_numerator x = value;
    bool negative;
    if ( x < 0 ) {
        x = -x;
        /// prevent buffer overflow if result is still negative.
        if ( x < 0 )
            x = x - 1;
        negative = true;
    } else
        negative = false;
    BlFixed_numerator n = 0;
    BlFixed_numerator units = 0;
    QString buffer = "";

    do {
/*
        if ( ( n - precision  - 1 ) % 3 == 0 && n - 1 > precision ) //ARON
          buffer = separadorMillares + buffer; //ARON
*/
        if ( n == precision ) {
            if ( n > 0 || options & DECIMAL )
              n ++;
            if ( precision > 0 ) 
              buffer = separadorDecimal + buffer;
            units = n;
        } // end if
        BlFixed_numerator y;
        y = ( BlFixed_numerator ) x / 10;
        n++;
        buffer = QString::number(x-y * 10) + buffer;
        x = y;
    } while ( n <= precision || x != 0 );
    if ( negative )
	  buffer = "-" + buffer;
	 n++;
    if ( options & ALIGN ) {
        while ( n - units < MAX_FIXED_LENGTH - 2 ) {
	      buffer = " " + buffer;
		  n++;
        } // end while
    } // end if

    
    return buffer;
}


bool operator == ( BlFixed x, BlFixed y )
{
    return x.m_data == y.m_data;
/*
    x.equalize_precision ( y );
    return x.value == y.value;
*/
}


bool operator == ( BlFixed x, int y )
{
    return x.m_data == QDecNumber(y);
}



bool operator == ( int x, BlFixed y )
{
    return QDecNumber(x) == y.m_data;
}


///
/**
\param x
**/
void BlFixed::equalize_precision ( BlFixed &x )
{
    BL_FUNC_DEBUG
    while ( m_precision < x.m_precision )   {
        value *= 10;
        m_precision ++;
    } // end while
    while ( x.m_precision < m_precision )  {
        x.value *= 10 ;
        x.m_precision ++;
    } // end while
    
}


///
/**
\param prec
**/
void BlFixed::setPrecision ( int precision )
{
    BL_FUNC_DEBUG
    
    /// Elimina decimales redondeando.
    QByteArray nuevoValor;
    if (precision < 0)
    {
        nuevoValor = m_data.rescale(-m_precision).toString();
    } else {
        nuevoValor = m_data.rescale(-precision).toString();
        m_precision = precision;
    } // end if

    m_data = QDecNumber(nuevoValor.constData());


    return;
/*    
    while ( m_precision < prec ) {
        value *= 10;
        m_precision ++;
    } // end while
    while ( prec < m_precision ) {
        BlFixed_numerator aux;
        aux = value;
        value = ( BlFixed_numerator ) ( value / 10 );
        if ( ( aux % 10 ) >= 5 ) {
            value++;
        } // end if
        m_precision--;
    } // end while
*/
}


///
/**
\param prec
**/
void BlFixed::setScale(int scale)
{
    BL_FUNC_DEBUG

    /// Elimina decimales redondeando.
    QByteArray nuevoValor;
    if (scale < 0)
    {
        nuevoValor = m_data.rescale(-m_scale).toString();
    } else {
        nuevoValor = m_data.rescale(-scale).toString();
        m_scale = scale;
        /// TODO
        m_precision = scale;
    } // end if

    m_data = QDecNumber(nuevoValor.constData());
}



///
/**
\param s
**/
void BlFixed::fromBlFixed ( const char *s )
{
    BL_FUNC_DEBUG

    m_precision = 0;
    
    QString data(s);
    if (data.isEmpty())
    {
      m_data = QDecNumber(0);
    } else {
      data.replace(",", ".");
      m_data = QDecNumber(data.toLatin1().constData());

      /// Cuenta el número de decimales.
      int dotPos = data.indexOf(".");
      if (dotPos >= 0)
      {
        m_precision = data.mid(dotPos + 1).length();
      } // end if
    } // end if

qDebug() << QString(m_data.toString());

    

/*
    QLocale locale;
    value = 0;
    int c;
    while ( ( c = *s++ ) == ' ' || c == '\t' )
        ;
    bool negative;
    if ( c == '-' ) {
        negative = true;
        c = *s++;
    } else {
        negative = false;
    } // end if
    bool decimal = false;
    while ( m_precision < MAX_FIXED_PRECISION ) {
        if ( '0' <= c && c <= '9' ) {
            value = value * 10 + ( c - '0' );
            if ( decimal )
                m_precision++;
        } else if ( c == '.' || c == ',' ) {
            if ( decimal )
                break; 
            decimal = true;
        } else if ( c != ','  )
            break; 
        c = *s++;
    } // end while
    if ( negative )
        value = - value;
    if ( value == 0 )
        m_precision = 1;

*/
}


bool operator < ( BlFixed x, BlFixed y )
{
    return x.m_data < y.m_data;
/*
    x.equalize_precision ( y );
    return x.value < y.value;
*/
}


bool operator < ( BlFixed x, int y )
{
    return x.m_data < QDecNumber(y);
}


bool operator < ( int x, BlFixed y )
{
    return QDecNumber(x) < y.m_data;
}


BlFixed operator - ( BlFixed x )
{
    x.m_data = x.m_data.invert();
    return x;
}


BlFixed operator * ( BlFixed x, int y )
{
    x.m_data = x.m_data * QDecNumber(y);
    return x;
}


BlFixed operator * ( BlFixed x, BlFixed y )
{
    x.m_data = x.m_data * y.m_data;

    return x;
/*
    int pres = x.precision() > y.precision() ? x.precision() : y.precision();
    x.value = x.value * y.value;
    x.m_precision = x.m_precision + y.m_precision;
    x.setPrecision(pres);
    return x;
*/
}


BlFixed BlFixed::operator [] ( int p ) const
{
    BL_FUNC_DEBUG
    BlFixed x ( 0, p );
    
    return x;
}


BlFixed operator - ( BlFixed x, BlFixed y )
{
    x.m_data = x.m_data - y.m_data;
    return x;

/*
    x.equalize_precision ( y );
    x.value = x.value - y.value;
    return x;
*/
}


BlFixed operator - ( BlFixed x, int y )
{
    x.m_data = x.m_data - QDecNumber(y);
    return x;
}


BlFixed operator - ( int x, BlFixed y )
{
    y.m_data = QDecNumber(x) - y.m_data;
    return y;
}

