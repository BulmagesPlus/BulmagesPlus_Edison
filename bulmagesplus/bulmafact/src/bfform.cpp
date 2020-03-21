/***************************************************************************
 *   Copyright (C) 2007 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   Copyright (C) 2019 by Fco. Javier M. C.                               *
 *   fcojavmc@todo-redes.com                                               *
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

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QDebug>
#include <QStringList>

#include "bfform.h"
#include "blplugins.h"
#include "bltemplateselector.h"
#include "bfbuscarregimenfiscal.h"
#include "bfbuscaralmacen.h"
#include "bfbuscararticulo.h"
#include "bfbuscarbanco.h"
#include "bfbuscarfamilia.h"
#include "bfbuscarformapago.h"
#include "bfbuscarreferencia.h"
#include "bfbuscarseriefactura.h"
#include "bfbuscartipoarticulo.h"
#include "bfbuscartipoiva.h"
#include "bfbuscartrabajador.h"


class BlFixed;

///
/**
\param comp
\param parent
\param f
\param modo
**/
BfForm::BfForm ( BfCompany *company, QWidget *parent, Qt::WindowFlags f, edmode modo )
        : BlForm ( company, parent, f, modo )
{
    BL_FUNC_DEBUG

    m_listalineas = NULL;
    m_listadescuentos = NULL;
    //m_totalIvaAdicional = BlFixed("0.0");
    m_ivaAdicional = QHash <QString, QList<QPair<BlFixed, BlFixed> > >();

}

///
/**
\return
**/
BfForm::~BfForm()
{
    BL_FUNC_DEBUG
    
    mainCompany() ->removeWindow ( this );
    
    
}

///
/**
\return
**/
BfCompany * BfForm::mainCompany()
{
    BL_FUNC_DEBUG
    
    
    return ( BfCompany * ) BlForm::mainCompany();
}


/** Calcula los totales de los documentos teniendo en cuenta los descuentos e impuestos.
    Invoca al metodo de pintaTotales para que se pinten dichos valores en la pantalla.
*/
/**
\return
**/
void BfForm::calculaypintatotales()
{
    BL_FUNC_DEBUG

    BlFixed totalBaseImponible("0");
    BlFixed totalIVA("0");
    BlFixed totalRE("0");
    BlFixed totalIRPF("0");
    BlFixed totalDescuentos("0");
    BlFixed totalTotal("0");

    Base listaBaseImponible;
    Base listaRE;
    Base listaIVA;

    /// Total Base Imponible
    {
        int baseImponibleLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName()); /// Tabla LINEAS
        int baseImponibleTotalDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES

        /// Calcula BASE IMPONIBLE
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed baseImponibleLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100"));
            baseImponibleLinea.setScale(baseImponibleLineaDecimales);

            BlFixed descuentoLinea = cant * pvp * descuento / BlFixed("100");
            descuentoLinea.setScale(baseImponibleLineaDecimales);
            totalDescuentos = totalDescuentos + descuentoLinea;

            /// Guarda por tipo IVA
            if (listaBaseImponible.contains(linea->dbValue("iva" + m_listalineas->tableName())))
            {
                listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] + baseImponibleLinea;
            } else {
                listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = baseImponibleLinea;
            } // end if

        } // end for

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
            {
                BlFixed nuevaBaseImponible = i.value() * (BlFixed("1") - proporcion / BlFixed("100"));
                listaBaseImponible[i.key()] = nuevaBaseImponible;
                totalDescuentos = totalDescuentos + (nuevaBaseImponible * proporcion / BlFixed("100"));
            } // end for
        } // end for

        /// Suma Bases Imponibles para saber el total.
        for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
        {
            BlFixed baseImponible = i.value();
            baseImponible.setScale(baseImponibleTotalDecimales);
            totalBaseImponible = totalBaseImponible + baseImponible;
        } // end for

        totalBaseImponible.setScale(baseImponibleTotalDecimales);
    }

    /// Total IVA
    {
        int impuestosLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "iva" + m_listalineas->tableName()); /// Tabla LINEAS
        int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

        /// Calcula IVA
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed iva (linea->dbValue("iva" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed impuestosLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100")) * (iva / BlFixed("100"));
            impuestosLinea.setScale(impuestosLineaDecimales);

            /// Guarda por tipo IVA
            if (listaIVA.contains(linea->dbValue("iva" + m_listalineas->tableName())))
            {
                listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] = listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] + impuestosLinea;
            } else {
                listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] = impuestosLinea;
            } // end if
        } // end for

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            for (Base::Iterator i = listaIVA.begin(); i != listaIVA.end(); ++i)
            {
                BlFixed nuevoIVA = i.value() * (BlFixed("1") - proporcion / BlFixed("100"));
                listaIVA[i.key()] = nuevoIVA;
            } // end for
        } // end for

        /// Suma IVAs para saber el total.
        for (Base::Iterator i = listaIVA.begin(); i != listaIVA.end(); ++i)
        {
            BlFixed iva = i.value();
            iva.setScale(impuestosTotalDecimales);
            totalIVA = totalIVA + iva;
        } // end for

        totalIVA.setScale(impuestosTotalDecimales);
    }

    /// Total Recargo de Equivalencia
    {
        int impuestosLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "reqeq" + m_listalineas->tableName()); /// Tabla LINEAS
        int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

        /// Calcula Recargo de Equivalencia
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed reqeq (linea->dbValue("reqeq" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed impuestosLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100")) * (reqeq / BlFixed("100"));
            impuestosLinea.setScale(impuestosLineaDecimales);

            /// Guarda por tipo RE
            if (listaRE.contains(linea->dbValue("reqeq" + m_listalineas->tableName())))
            {
                listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] = listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] + impuestosLinea;
            } else {
                listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] = impuestosLinea;
            } // end if
        } // end for

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            for (Base::Iterator i = listaRE.begin(); i != listaRE.end(); ++i)
            {
                BlFixed nuevoRE = i.value() * (BlFixed("1") - proporcion / BlFixed("100"));
                listaRE[i.key()] = nuevoRE;
            } // end for
        } // end for

        /// Suma REs para saber el total.
        for (Base::Iterator i = listaRE.begin(); i != listaRE.end(); ++i)
        {
            BlFixed re = i.value();
            re.setScale(impuestosTotalDecimales);
            totalRE = totalRE + re;
        } // end for

        totalRE.setScale(impuestosTotalDecimales);
    }

    /// Total IRPF
    {
        BlDbRecordSet *rs;
        int totalTotalDecimales = mainCompany()->numericScale(tableName(), "total" + tableName()); /// Tabla TOTALES

        /// Usa IRPF general por defecto.
        BlFixed tipoIRPF("0");
        rs = mainCompany()->loadQuery("SELECT valor::numeric AS irpf FROM configuracion WHERE LOWER(nombre) = 'irpf'");
        if (rs)
        {
            if (!rs->eof())
            {
                tipoIRPF = BlFixed(rs->value("irpf"));
            } // end if
            delete rs;
        } // end if
        /// Usar IRPF por documento si existe y no es NULL.
        QString columnaIRPF = QString("irpf") + tableName();
        if (exists(columnaIRPF))
        {
            /// Usa IRPF del documento
            if (dbValue(columnaIRPF) != QString(""))
            {
                tipoIRPF = BlFixed(dbValue(columnaIRPF));
            } // end if
        } // end if

        /// Usa IRPF general por defecto.
        QString modoIRPF("sobrebase");
        rs = mainCompany()->loadQuery("SELECT valor::varchar AS irpf_modo FROM configuracion WHERE LOWER(nombre) = 'irpf_modo'");
        if (rs)
        {
            if (!rs->eof())
            {
                modoIRPF = rs->value("irpf_modo");
            } // end if
            delete rs;
        } // end if
        /// Modo IRPF general o por documento.
        QString columnaModoIRPF = QString("irpf_modo") + tableName();
        if (exists(columnaModoIRPF))
        {
            /// Usa IRPF_MODO del documento
            if (dbValue(columnaModoIRPF) != QString(""))
            {
                modoIRPF = dbValue(columnaModoIRPF);
            } // end if
        } // end if

        /// Calcula IRPF
        if (modoIRPF == QString("sobrebase"))
        {
            totalIRPF = totalBaseImponible * (tipoIRPF / BlFixed("100"));
        }
        else if (modoIRPF == QString("sobretotal"))
        {
            totalIRPF = (totalBaseImponible + totalIVA + totalRE) * (tipoIRPF / BlFixed("100"));
        } // end if

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            totalIRPF = totalIRPF * (BlFixed("1") - proporcion / BlFixed("100"));
        } // end for

        totalIRPF.setScale(totalTotalDecimales);
    }

    /// Total descuentos
    {
        int totalDescuentosDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES
        totalDescuentos.setScale(totalDescuentosDecimales);
    }

    /// Total total
    {
        int totalTotalDecimales = mainCompany()->numericScale(tableName(), "total" + tableName()); /// Tabla TOTALES
        totalTotal = totalBaseImponible - totalIRPF + totalIVA + totalRE;
        totalTotal.setScale(totalTotalDecimales);
    }

    qDebug() << m_listalineas->tableName();
    qDebug() << tableName();

    qDebug() << totalBaseImponible.toQString();
    qDebug() << totalIVA.toQString();
    qDebug() << totalRE.toQString();
    qDebug() << totalIRPF.toQString();
    qDebug() << totalDescuentos.toQString();
    qDebug() << totalTotal.toQString();


    pintatotales(totalIVA, totalBaseImponible, totalTotal, totalDescuentos, totalIRPF, totalRE);
}

/// Busca strings del tipo [xxxx] entro del texto pasado y los sustituye
/// Por valores existentes en la base de datos.
/**
\param buff El texto entero sobre el que se hace el reemplazo de sentencias.
**/
int BfForm::parseTags ( QString &buff, int tipoEscape )
{
    BL_FUNC_DEBUG
    
    int pos =  0;

    /// Buscamos algo de lineas de detalle
    QRegExp rx ( "<!--\\s*LINEAS\\s*DETALLE\\s*-->(.*)<!--\\s*END\\s*LINEAS\\s*DETALLE\\s*-->" );
    rx.setMinimal ( true );
    
    while ( ( pos = rx.indexIn ( buff, pos ) ) != -1 ) {
        QString ldetalle = trataLineasDetalle ( rx.cap ( 1 ), tipoEscape );
        buff.replace ( pos, rx.matchedLength(), ldetalle );
        pos = 0;
    } // end while

    /// Buscamos Si hay descuentos en condicional
    pos = 0;
    QRegExp rx3 ( "<!--\\s*IF\\s*DESCUENTOS\\s*-->(.*)<!--\\s*END\\s*IF\\s*DESCUENTOS\\s*-->" );
    rx3.setMinimal ( true );
    
    while ( ( pos = rx3.indexIn ( buff, pos ) ) != -1 ) {
        if ( m_listadescuentos->rowCount() - 1 <= 0 ) {
            buff.replace ( pos, rx3.matchedLength(), "" );
            pos = 0;
        } else {
            pos += rx3.matchedLength();
        } // end if
    } // end while

    /// Buscamos lineas de descuento
    pos = 0;
    QRegExp rx2 ( "<!--\\s*LINEAS\\s*DESCUENTO\\s*-->(.*)<!--\\s*END\\s*LINEAS\\s*DESCUENTO\\s*-->" );
    rx2.setMinimal ( true );
    
    while ( ( pos = rx2.indexIn ( buff, pos ) ) != -1 ) {
        QString ldetalle = trataLineasDescuento ( rx2.cap ( 1 ), tipoEscape );
        buff.replace ( pos, rx2.matchedLength(), ldetalle );
        pos = 0;
    } // end while

    /// Buscamos lineas de totales de Bases Imponibles e IVAS.
    pos = 0;
    QRegExp rx5 ( "<!--\\s*TOTALES\\s*-->(.*)<!--\\s*END\\s*TOTALES\\s*-->" );
    rx5.setMinimal ( true );
    
    while ( ( pos = rx5.indexIn ( buff, pos ) ) != -1 ) {
        QString ldetalle = trataTotales ( rx5.cap ( 1 ), 1 );
        buff.replace ( pos, rx5.matchedLength(), ldetalle );
        pos = 0;
    } // end while

    pos = 0;
    QRegExp rx6 ( "<!--\\s*TOTREQ\\s*-->(.*)<!--\\s*END\\s*TOTREQ\\s*-->" );
    rx6.setMinimal ( true );
    
    while ( ( pos = rx6.indexIn ( buff, pos ) ) != -1 ) {
        QString ldetalle = trataTotales ( rx6.cap ( 1 ), 2 );
        buff.replace ( pos, rx6.matchedLength(), ldetalle );
        pos = 0;
    } // end while

    int ret = BlForm::parseTags ( buff, tipoEscape );

    parseTagsBf ( buff, tipoEscape );
    
    
    
    return ret;
}

void BfForm::parseTagsBf ( QString &buff, int tipoEscape )
{
    BL_FUNC_DEBUG

    QString fitxersortidatxt = "";
    BlDbRecordSet *cur = NULL;
    
    try {

        if ( exists ( "id" + tableName() ) )
            buff.replace ( "[id" + tableName() + "]", blStringEscape ( dbValue ( "id" + tableName() ), tipoEscape ) );
        if ( exists ( "num" + tableName() ) )
            buff.replace ( "[num" + tableName() + "]", blStringEscape ( dbValue ( "num" + tableName() ), tipoEscape ) );
        if ( exists ( "f" + tableName() ) )
            buff.replace ( "[f" + tableName() + "]", blStringEscape ( dbValue ( "f" + tableName() ), tipoEscape ) );
        if ( exists ( "venc" + tableName() ) )
            buff.replace ( "[venc" + tableName() + "]", blStringEscape ( dbValue ( "venc" + tableName() ), tipoEscape ) );
        if ( exists ( "contact" + tableName() ) )
            buff.replace ( "[contact" + tableName() + "]", blStringEscape ( dbValue ( "contact" + tableName() ), tipoEscape ) );
        if ( exists ( "tel" + tableName() ) )
            buff.replace ( "[tel" + tableName() + "]", blStringEscape ( dbValue ( "tel" + tableName() ), tipoEscape ) );
        if ( exists ( "coment" + tableName() ) )
            buff.replace ( "[coment" + tableName() + "]", blStringEscape ( dbValue ( "coment" + tableName() ), tipoEscape ) );
        if ( exists ( "desc" + tableName() ) )
            buff.replace ( "[desc" + tableName() + "]", blStringEscape ( dbValue ( "desc" + tableName() ), tipoEscape ) );
        if ( exists ( "ref" + tableName() ) )
            buff.replace ( "[ref" + tableName() + "]", blStringEscape ( dbValue ( "ref" + tableName() ), tipoEscape ) );
        if ( exists ( "codigoserie_" + tableName() ) )
            buff.replace ( "[codigoserie_" + tableName() + "]", blStringEscape ( dbValue ( "codigoserie_" + tableName() ), tipoEscape ) );
        if ( exists ( "fecha" + tableName() ) )
            buff.replace ( "[fecha" + tableName() + "]", blStringEscape ( dbValue ( "fecha" + tableName() ), tipoEscape ) );

        /// Impresion de la tabla de contenidos.
        QString l;

        /// Contador que sirve para poner lineas de mas en caso de que sea preciso.
        BlDbSubFormRecord *linea;

        /// Si no hay lista de lineas salimos.
        if (!m_listalineas) return;

        /// Expresion regular para saber los parametros de STORY.
        QRegExp rx("\\[story(.*)\\]");
        rx.setMinimal(true);
        rx.setCaseSensitivity(Qt::CaseInsensitive);

        /// Sustituye todas las ocurrencias de STORY.
        int pos = 0;

        while (( pos = rx.indexIn ( buff, pos )) != -1 )
        {
            fitxersortidatxt = "";

            /// Extrae los parametros de STORY.
            QStringList parametros_story_original = rx.cap(1).split(",");
            QStringList parametros_story = rx.cap(1).toLower().split(",");

            /// Si STORY tiene parametros se crean las columnas en el orden indicado.
            /// Si no tiene parametros se escriben unas columnas predeterminadas.
            /// parametros_story siempre tiene, al menos, 1 elemento aunque no se hayan
            /// especificado parametros. Por eso tenemos que hacer unas pocas comprobaciones mas.
            if ((parametros_story.count() >= 1) && (parametros_story[0].trimmed() != ""))
            {

                /// Recorre las lineas del documento
                for ( int i = 0; i < ( m_listalineas->rowCount() - 1 ); ++i ) {

                    linea = m_listalineas->lineaat ( i );
                    BlFixed base = BlFixed ( linea->dbValue ( "cant" + m_listalineas->tableName() ).toLatin1().constData() ) * BlFixed ( linea->dbValue ( "pvp" + m_listalineas->tableName() ).toLatin1().constData() );
                    /// Recorre todos los parametros

                      QString bufferLinea{""};
                      QString serializaValoresLinea{""};
                      bufferLinea += "<tr>\n";

                      for (int j{0}; j < parametros_story.count(); ++j)
                      {
                        if (parametros_story[j].trimmed() == "codigocompletoarticulo")
                        {
                            ///Impresion del codigo de los articulos
                            QString valor = linea->dbValue ( "codigocompletoarticulo" );
                            serializaValoresLinea += valor;
                            bufferLinea += "    <td>" + blStringEscape ( valor, tipoEscape ) + "</td>\n";
                        } else if (parametros_story[j].trimmed() == "extraline" ) {
                            bufferLinea += "    </tr>\n";
                            /// guarda los datos hasta ahora si toca o se descartan si todos los campos están vacios.
                            serializaValoresLinea = serializaValoresLinea.trimmed();
                            if (!serializaValoresLinea.isEmpty())
                            {
                              fitxersortidatxt += bufferLinea;
                              serializaValoresLinea = "";
                            } // end if
                            
                            /// Nuevo buffer de línea.
                            bufferLinea = "    <tr>\n";
                        } else if (parametros_story[j].trimmed() == "empty" ) {
                            ///Campo vacío
                            bufferLinea += "    <td></td>\n";
                        } else if (parametros_story[j].trimmed() == "desc" + tableName()) {
                            ///Impresion del descripcion de los articulos
                            QString valor = linea->dbValue ( "desc" + m_listalineas->tableName() );
                            serializaValoresLinea += valor;
                            bufferLinea += "    <td><para style=\"paragrafo\">" + blStringEscape ( valor, tipoEscape ).replace ( QChar ( '\n' ), "<br/>" ) + "</para></td>\n";
                        } else if (parametros_story[j].trimmed() == "cant" + tableName()) {
                            ///Impresion de las candidad de los articulos
                            QString valor = linea->dbValue ( "cant" + m_listalineas->tableName() );
                            serializaValoresLinea += valor;
                            bufferLinea += "    <td>" + valor + "</td>\n";
                        } else if (parametros_story[j].trimmed() == "pvp" + tableName()) {
                            ///Impresion del precio de los articulos
                            QString valor = linea->dbValue ( "pvp" + m_listalineas->tableName() );
                            serializaValoresLinea += valor;
                            bufferLinea += "    <td>" + l.sprintf ( "%s", blStringEscape ( valor, tipoEscape ).toLatin1().constData() ) + "</td>\n";
                        } else if (parametros_story[j].trimmed() == "descuento" + tableName()) {
                            ///Impresion de los descuentos de los articulos
                            QString valor = linea->dbValue ( "descuento" + m_listalineas->tableName() );
                            serializaValoresLinea += valor;
                            bufferLinea += "    <td>" + l.sprintf ( "%s", blStringEscape ( valor, tipoEscape ).toLatin1().constData() ) + " %</td>\n";
                        } else if (parametros_story[j].trimmed() == "subtotal" + tableName()) {
                            ///Impresion del subtotal de los articulos
                            BlFixed subtotal = base * (BlFixed("1") - BlFixed(linea->dbValue("descuento" + m_listalineas->tableName())) / BlFixed("100"));
                            int pvpPrecision = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName());
                            subtotal.setScale(pvpPrecision);

                            QString valor = subtotal.toQString();
                            serializaValoresLinea += valor;

                            bufferLinea += "    <td>" + l.sprintf ( "%s", valor.toLatin1().constData() ) + "</td>\n";
                        } else {
                            bool isCurrency{false};
                            bool zeroIsEmpty{false};
                            /// Tiene título asociado:
                            QString titulo = "";
                            QString dato = parametros_story_original[j];
                            QString campo = dato;
                            int pos = dato.indexOf("->");
                            if (pos >= 0)
                            {
                              titulo = dato.mid(0, pos);
                              titulo = titulo.trimmed() + " ";
                              campo = dato.mid(pos + 2);

                              if (campo.contains("@is_currency"))
                              {
                                isCurrency = true;
                                campo = campo.remove("@is_currency");
                              } // end if

                              if (campo.contains("@zero_is_empty"))
                              {
                                zeroIsEmpty = true;
                                campo = campo.remove("@zero_is_empty");
                              } // end if
                            } // end if

                            campo = campo.trimmed();
                            campo = campo.toLower();

                            QString valor = linea->dbValue ( campo );
                            if (zeroIsEmpty)
                            {
                              if (BlFixed(valor) == BlFixed("0"))
                              {
                                valor = QString("");
                              } // end if
                            } // end if
                            serializaValoresLinea += valor;

                            if (isCurrency && !valor.isEmpty())
                            {
                                BlFixed data(valor);
                                valor = data.toQString(',', 2) + QString(" €");
                            } // end if

                            /// Para otros campos se busca directamente.
                            bufferLinea += "    <td>" + l.sprintf ( "%s", blStringEscape ( titulo + valor, tipoEscape ).toLatin1().constData() ) + "</td>\n";
                        } // end if

                      } //end for

                      bufferLinea += "</tr>\n";

                      /// guarda los datos hasta ahora si toca o se descartan si todos los campos están vacios.
                      serializaValoresLinea = serializaValoresLinea.trimmed();
                      if (!serializaValoresLinea.isEmpty())
                      {
                        fitxersortidatxt += bufferLinea;
                      } // end if

                } // end for

                buff.replace ( pos, rx.matchedLength(), fitxersortidatxt );
                pos += rx.matchedLength();

            } else {

                /// Impresion de las lineas
                for ( int i = 0; i < ( m_listalineas->rowCount() - 1 ); ++i)
                {
                    linea = m_listalineas->lineaat ( i );
                    BlFixed base = BlFixed ( linea->dbValue ( "cant" + m_listalineas->tableName() ).toLatin1().constData() ) * BlFixed ( linea->dbValue ( "pvp" + m_listalineas->tableName() ).toLatin1().constData() );
                    ///Impresion de los contenidos
                    fitxersortidatxt += "<tr>";
                    fitxersortidatxt += "    <td>" + blStringEscape ( linea->dbValue ( "codigocompletoarticulo" ), tipoEscape ) + "</td>\n";
                    fitxersortidatxt += "    <td><para style=\"paragrafo\">" + blStringEscape ( linea->dbValue ( "desc" + m_listalineas->tableName() ), tipoEscape ).replace ( QChar ( '\n' ), "<br/>" ) + "</para></td>\n";
                    fitxersortidatxt += "    <td>" + linea->dbValue ( "cant" + m_listalineas->tableName() ) + "</td>\n";
                    fitxersortidatxt += "    <td>" + l.sprintf ( "%s", blStringEscape ( linea->dbValue ( "pvp" + m_listalineas->tableName() ), tipoEscape ).toLatin1().constData() ) + "</td>\n";
                    fitxersortidatxt += "    <td>" + l.sprintf ( "%s", blStringEscape ( linea->dbValue ( "descuento" + m_listalineas->tableName() ), tipoEscape ).toLatin1().constData() ) + " %</td>\n";

                    BlFixed subtotal = base * (BlFixed("1") - BlFixed(linea->dbValue("descuento" + m_listalineas->tableName())) / BlFixed("100"));
                    int pvpPrecision = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName());
                    subtotal.setScale(pvpPrecision);

                    fitxersortidatxt += "    <td>" + l.sprintf ( "%s", ( subtotal ).toQString().toLatin1().constData() ) + "</td>\n";
                    fitxersortidatxt += "</tr>";
                } // end for

                buff.replace ( "[story]", fitxersortidatxt );
            } // end if

        } // end while


    BlFixed totalSinDescuentosGenerales("0");
    BlFixed totalBaseImponible("0");
    BlFixed totalIVA("0");
    BlFixed totalRE("0");
    BlFixed totalIRPF("0");
    BlFixed totalDescuentos("0");
    BlFixed totalTotal("0");

    Base listaBaseImponible;
    Base listaRE;
    Base listaIVA;
  
    QString tr1 = ""; /// Rellena el primer tr de titulares.
    QString tr2 = ""; /// Rellena el segundo tr de cantidades.

    /// Total Base Imponible
    {
        BlFixed total("0");
        int baseImponibleLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName()); /// Tabla LINEAS
        int baseImponibleTotalDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES

        /// Calcula BASE IMPONIBLE
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed baseImponibleLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100"));
            baseImponibleLinea.setScale(baseImponibleLineaDecimales);

            BlFixed descuentoLinea = cant * pvp * descuento / BlFixed("100");
            descuentoLinea.setScale(baseImponibleLineaDecimales);

            total = total + baseImponibleLinea;
            totalDescuentos = totalDescuentos + descuentoLinea;

            /// Aplica DESCUENTOS GLOBALES
            for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
            {
                BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
                BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

                baseImponibleLinea = baseImponibleLinea * (BlFixed("1") - proporcion / BlFixed("100"));
                totalDescuentos = totalDescuentos + (baseImponibleLinea * proporcion / BlFixed("100"));
            } // end for

            tr1 += "    <td>" + blStringEscape ( _ ( "Base Imponible" ) + " " + blStringEscape ( linea->dbValue("iva" + m_listalineas->tableName()) ), tipoEscape ) + " %</td>\n";
            tr2 += "    <td>" + l.sprintf ( " %s ", baseImponibleLinea.toQString().toLatin1().constData() ) + "</td>\n";

            if (baseImponibleLinea != BlFixed("0"))
            {
                baseImponibleLinea.setScale(baseImponibleLineaDecimales);

                /// Guarda por tipo IVA
                if (listaBaseImponible.contains(linea->dbValue("iva" + m_listalineas->tableName())))
                {
                    listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] + baseImponibleLinea;
                } else {
                    listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = baseImponibleLinea;
                } // end if

            } // end if
        } // end for

        /// Suma Bases Imponibles para saber el total.
        for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
        {
            BlFixed baseImponible = i.value();
            baseImponible.setScale(baseImponibleTotalDecimales);
            totalBaseImponible = totalBaseImponible + baseImponible;
        } // end for

        totalBaseImponible.setScale(baseImponibleTotalDecimales);

        totalSinDescuentosGenerales = total;
        totalSinDescuentosGenerales.setScale(baseImponibleTotalDecimales);
    }

    /// Total IVA
    {
        int impuestosLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "iva" + m_listalineas->tableName()); /// Tabla LINEAS
        int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

        /// Calcula IVA
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed iva (linea->dbValue("iva" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed impuestosLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100")) * (iva / BlFixed("100"));
            impuestosLinea.setScale(impuestosLineaDecimales);

            /// Aplica DESCUENTOS GLOBALES
            for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
            {
                BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
                BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

                impuestosLinea = impuestosLinea * (BlFixed("1") - proporcion / BlFixed("100"));
            } // end for

            tr1 += "    <td>" + blStringEscape ( _ ( "I.V.A." ), tipoEscape ) + " " + blStringEscape ( linea->dbValue("iva" + m_listalineas->tableName()), tipoEscape ) + " %</td>\n";
            tr2 += "    <td>" + l.sprintf ( " %s ", blStringEscape ( impuestosLinea.toQString(), tipoEscape ).toLatin1().constData() ) + "</td>\n";

            if (impuestosLinea != BlFixed("0"))
            {
                impuestosLinea.setScale(impuestosLineaDecimales);

                /// Guarda por tipo IVA
                if (listaIVA.contains(linea->dbValue("iva" + m_listalineas->tableName())))
                {
                    listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] = listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] + impuestosLinea;
                } else {
                    listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] = impuestosLinea;
                } // end if
            } // end if
        } // end for

        /// Suma IVAs para saber el total.
        for (Base::Iterator i = listaIVA.begin(); i != listaIVA.end(); ++i)
        {
            BlFixed iva = i.value();
            iva.setScale(impuestosTotalDecimales);
            totalIVA = totalIVA + iva;
        } // end for

        totalIVA.setScale(impuestosTotalDecimales);
    }

    /// Total Recargo de Equivalencia
    {
        int impuestosLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "reqeq" + m_listalineas->tableName()); /// Tabla LINEAS
        int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

        /// Calcula Recargo de Equivalencia
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed reqeq (linea->dbValue("reqeq" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed impuestosLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100")) * (reqeq / BlFixed("100"));
            impuestosLinea.setScale(impuestosLineaDecimales);

            /// Aplica DESCUENTOS GLOBALES
            for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
            {
                BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
                BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

                impuestosLinea = impuestosLinea * (BlFixed("1") - proporcion / BlFixed("100"));
            } // end for

            tr1 += "    <td>" + _ ( "R.E." ) + " " + blStringEscape ( linea->dbValue("reqeq" + m_listalineas->tableName()), tipoEscape ) + " %</td>\n";
            tr2 += "    <td>" + l.sprintf ( " %s ", impuestosLinea.toQString().toLatin1().constData() ) + "</td>\n";

            if (impuestosLinea != BlFixed("0"))
            {
                impuestosLinea.setScale(impuestosLineaDecimales);

                /// Guarda por tipo RE
                if (listaRE.contains(linea->dbValue("reqeq" + m_listalineas->tableName())))
                {
                    listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] = listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] + impuestosLinea;
                } else {
                    listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] = impuestosLinea;
                } // end if
            } // end if
        } // end for

        /// Suma REs para saber el total.
        for (Base::Iterator i = listaRE.begin(); i != listaRE.end(); ++i)
        {
            BlFixed re = i.value();
            re.setScale(impuestosTotalDecimales);
            totalRE = totalRE + re;
        } // end for

        totalRE.setScale(impuestosTotalDecimales);
    }


    BlFixed tipoIRPF("0");
    /// Total IRPF
    {
        BlDbRecordSet *rs;
        int totalTotalDecimales = mainCompany()->numericScale(tableName(), "total" + tableName()); /// Tabla TOTALES

        /// Usa IRPF general por defecto.
        //BlFixed tipoIRPF("0");
        rs = mainCompany()->loadQuery("SELECT valor::numeric AS irpf FROM configuracion WHERE LOWER(nombre) = 'irpf'");
        if (rs)
        {
            if (!rs->eof())
            {
                tipoIRPF = BlFixed(rs->value("irpf"));
            } // end if
            delete rs;
        } // end if
        /// Usar IRPF por documento si existe y no es NULL.
        QString columnaIRPF = QString("irpf") + tableName();
        if (exists(columnaIRPF))
        {
            /// Usa IRPF del documento
            if (dbValue(columnaIRPF) != QString(""))
            {
                tipoIRPF = BlFixed(dbValue(columnaIRPF));
            } // end if
        } // end if

        /// Usa IRPF general por defecto.
        QString modoIRPF("sobrebase");
        rs = mainCompany()->loadQuery("SELECT valor::varchar AS irpf_modo FROM configuracion WHERE LOWER(nombre) = 'irpf_modo'");
        if (rs)
        {
            if (!rs->eof())
            {
                modoIRPF = rs->value("irpf_modo");
            } // end if
            delete rs;
        } // end if
        /// Modo IRPF general o por documento.
        QString columnaModoIRPF = QString("irpf_modo") + tableName();
        if (exists(columnaModoIRPF))
        {
            /// Usa IRPF_MODO del documento
            if (dbValue(columnaModoIRPF) != QString(""))
            {
                modoIRPF = dbValue(columnaModoIRPF);
            } // end if
        } // end if


        /// Calcula IRPF
        if (modoIRPF == QString("sobrebase"))
        {
            totalIRPF = totalBaseImponible * (tipoIRPF / BlFixed("100"));
        }
        else if (modoIRPF == QString("sobretotal"))
        {
            totalIRPF = (totalBaseImponible + totalIVA + totalRE) * (tipoIRPF / BlFixed("100"));
        } // end if

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            totalIRPF = totalIRPF * (BlFixed("1") - proporcion / BlFixed("100"));
        } // end for

        totalIRPF.setScale(totalTotalDecimales);

        if ( tipoIRPF != BlFixed("0") )
        {
            tr1 += "    <td>" + blStringEscape ( _ ( "I.R.P.F." ) + "  (- " +  tipoIRPF.toQString() + ") %", tipoEscape ) + "</td>\n";
            tr2 += "    <td>" + blStringEscape ( l.sprintf ( " %s ", totalIRPF.toQString().toLatin1().constData() ), tipoEscape ) + "</td>\n";
        } // end if

        tipoIRPF.setScale(totalTotalDecimales);
    }

    /// Total descuentos
    {
        int totalDescuentosDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES
        totalDescuentos.setScale(totalDescuentosDecimales);
    }

    /// Total total
    {
        int totalTotalDecimales = mainCompany()->numericScale(tableName(), "total" + tableName()); /// Tabla TOTALES
        totalTotal = totalBaseImponible - totalIRPF + totalIVA + totalRE;
        totalTotal.setScale(totalTotalDecimales);
    }


        /// Impresion de los descuentos.
        fitxersortidatxt = "";
        BlDbSubFormRecord *linea1;

        if (m_listadescuentos)
        {
            if ( m_listadescuentos->rowCount() - 1 )
            {
                fitxersortidatxt += "<blockTable style=\"tabladescuento\">\n";
                fitxersortidatxt += "<tr>\n";
                fitxersortidatxt += "    <td>" + blStringEscape ( _ ( "Descuento" ), tipoEscape ) + "</td>\n";
                fitxersortidatxt += "    <td>" + blStringEscape ( _ ( "Porcentaje" ), tipoEscape ) + "</td>\n";
                fitxersortidatxt += "    <td>" + blStringEscape ( _ ( "Total Desc." ), tipoEscape ) + "</td>\n";
                fitxersortidatxt += "</tr>\n";

                BlFixed basei = totalBaseImponible;
                for ( int i = 0; i < ( m_listadescuentos->rowCount() - 1 ); ++i )
                {
                    linea1 = m_listadescuentos->lineaat ( i );
                    BlFixed importeDescuento = BlFixed(linea1->dbValue("proporcion" + m_listadescuentos->tableName())) * basei / BlFixed("100");
                    basei = basei - importeDescuento;
                    
                    fitxersortidatxt += "<tr>\n";
                    fitxersortidatxt += "    <td>" + blStringEscape ( linea1->dbValue ( "concept" + m_listadescuentos->tableName() ), tipoEscape ) + "</td>\n";
                    fitxersortidatxt += "    <td>" + blStringEscape ( l.sprintf ( "%s", linea1->dbValue ( "proporcion" + m_listadescuentos->tableName() ).toLatin1().constData() ), tipoEscape ) + " %</td>\n";
                    fitxersortidatxt += "    <td>" + blStringEscape ( l.sprintf ( "-%s", importeDescuento.toQString().toLatin1().constData() ), tipoEscape ) + "</td>\n";
                    fitxersortidatxt += "</tr>";
                } // end for
                
                fitxersortidatxt += "</blockTable>\n";
                
            } // end if
            
        } // end if
        
        buff.replace ( "[descuentos]", fitxersortidatxt );

        tr1 += "    <td>" + blStringEscape ( _ ( "Total" ), tipoEscape ) + "</td>\n";
        tr2 += "    <td>" + blStringEscape ( l.sprintf ( " %s ", totalTotal.toQString().toLatin1().constData() ), tipoEscape ) + "</td>\n";

        /// Impresion de las bases imponibles.
        fitxersortidatxt = "";
        fitxersortidatxt += "<blockTable style=\"tabladescuento\" repeatRows=\"1\" >\n";
        fitxersortidatxt += "<tr>" + tr1 + "</tr><tr>" + tr2 + "</tr></blockTable>\n";

        buff.replace ( "[totales]", fitxersortidatxt );
        buff.replace ( "[irpf]", totalIRPF.toQString() );
        buff.replace ( "[tirpf]", tipoIRPF.toQString() );
        buff.replace ( "[totalre]", totalRE.toQString() );
        buff.replace ( "[teoricbimp]", totalSinDescuentosGenerales.toQString() );
        buff.replace ( "[totalbimp]", totalBaseImponible.toQString() );
        buff.replace ( "[totaldesc]", totalDescuentos.toQString() );
        buff.replace ( "[totaliva]", totalIVA.toQString() );
        buff.replace ( "[total]", totalTotal.toQString() );

        /// En la version para windows hay problemas con las imagenes,
        /// por eso de momento lo dejamos asi.
#ifndef WINDOWS
        //   buff.replace("[detallearticulos]", detalleArticulos());
#endif

// a ver. No se porque pongo esto aqui.
// Como que he sacado codigo de generateRML(arch) para ponerlo aqui (porque me parece que es parseTags
// quien deberia tratar todos los tags y porque asi aprovecho el proceso de escribir el fichero en el
// encoding correcto que ya esta hecho en Ficha) pues he copiado tambien esto, pero yo hubiera dicho
// que C++ ya haria el delete solito y si
// no es asi, porque en generateRML había dos catchs y solo uno hacia delete?
    } catch ( ... ) {
    
        if ( cur ) delete cur;
        throw ( -1 );
        
    }

    
}

/// Trata las lineas de detalle encontradas dentro de los tags <!--LINEAS DETALLE-->
/**
\param det Texto de entrada para ser tratado por iteracion.
\return
**/
QString BfForm::trataLineasDetalle ( const QString &det, int tipoEscape )
{
    BL_FUNC_DEBUG
    
    QString result = "";

    /// Contador que sirve para poner lineas de mas en caso de que sea preciso.
    BlDbSubFormRecord *linea;

    int baseImponibleLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName()); /// Tabla LINEAS
    
    /// Impresion de las lineas
    for ( int i = 0; i < ( m_listalineas->rowCount() - 1 ); ++i ) {
    
        QString salidatemp = det;
        linea = m_listalineas->lineaat ( i );
        BlFixed base = BlFixed (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData()) * BlFixed(linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
        QString l;

        /// Los saltos de carro se deben tratar de modo especial ya que RML no los contempla bien.
        QString desc = linea->dbValue ( "desc" + m_listalineas->tableName() );
        QStringList descp = desc.split ( "\n" );
        QString desc1 = "";
        
        for ( int i = 0; i < descp.size(); ++i ) {
            if ( descp[i] != "" ) {
                if ( descp[i].startsWith ( " " ) ) {
                    desc1 += "<pre>" + blStringEscape ( descp[i], tipoEscape ) + "</pre>\n";
                } else {
                    desc1 += "<para>" + blStringEscape ( descp[i], tipoEscape ) + "</para>\n";
                } // end if
            } else {
                desc1 += "<spacer length=\"0.1cm\"/>\n";
            } // end if
        } // end for

        salidatemp.replace ( "[desc" + m_listalineas->tableName() + "]", desc1 );
        salidatemp.replace ( "[cant" + m_listalineas->tableName() + "]",  linea->dbValue ( "cant" + m_listalineas->tableName() ) );

        salidatemp.replace ( "[pvp" + m_listalineas->tableName() + "]",  blStringEscape ( linea->dbValue ( "pvp" + m_listalineas->tableName() ), tipoEscape ).toLatin1().constData() );
        salidatemp.replace ( "[descuento" + m_listalineas->tableName() + "]" ,  blStringEscape ( linea->dbValue ( "descuento" + m_listalineas->tableName() ), tipoEscape ).toLatin1().constData() );

        BlFixed ftotal = BlFixed ( linea->dbValue ( "descuento" + m_listalineas->tableName() ) );
        BlFixed fftotal = base - (BlFixed("1") - ftotal / BlFixed("100"));
        fftotal.setScale(baseImponibleLineaDecimales);

        salidatemp.replace ( "[total" + m_listalineas->tableName() + "]" ,  blStringEscape ( fftotal.toQString(), tipoEscape ) );

        /// Buscamos cadenas perdidas adicionales que puedan quedar por poner.
        QRegExp rx ( "\\[(\\w*)\\]" );
        int pos =  0;
        
        while ( ( pos = rx.indexIn ( salidatemp, pos ) ) != -1 ) {
            if ( linea->exists ( rx.cap ( 1 ) ) ) {
                salidatemp.replace ( pos, rx.matchedLength(), blStringEscape ( linea->dbValue ( rx.cap ( 1 ) ), tipoEscape ) );
                pos = 0;
            } else {
                pos += rx.matchedLength();
            }
        } // end while

        result += salidatemp;
        
    } // end for

    return result;
}


/// Trata las lineas de descuento encontradas dentro de los tags <!--LINEAS DESCUENTO-->
/**
\param det Texto de entrada para ser tratado por iteracion.
\return
**/
QString BfForm::trataLineasDescuento ( const QString &det, int tipoEscape )
{
    BL_FUNC_DEBUG

    QString result = "";

    BlFixed totalBaseImponible("0");
    BlFixed totalDescuentos("0");
    Base listaBaseImponible;

    /// Total Base Imponible
    {
        int baseImponibleLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName()); /// Tabla LINEAS
        int baseImponibleTotalDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES

        /// Calcula BASE IMPONIBLE
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed baseImponibleLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100"));
            baseImponibleLinea.setScale(baseImponibleLineaDecimales);

            BlFixed descuentoLinea = cant * pvp * descuento / BlFixed("100");
            descuentoLinea.setScale(baseImponibleLineaDecimales);

            totalDescuentos = totalDescuentos + descuentoLinea;

            if (baseImponibleLinea != BlFixed("0"))
            {
                baseImponibleLinea.setScale(baseImponibleLineaDecimales);

                /// Guarda por tipo IVA
                if (listaBaseImponible.contains(linea->dbValue("iva" + m_listalineas->tableName())))
                {
                    listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] + baseImponibleLinea;
                } else {
                    listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = baseImponibleLinea;
                } // end if

            } // end if
        } // end for

        /// Suma Bases Imponibles para saber el total.
        for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
        {
            BlFixed baseImponible = i.value();
            baseImponible.setScale(baseImponibleTotalDecimales);
            totalBaseImponible = totalBaseImponible + baseImponible;
        } // end for

        totalBaseImponible.setScale(baseImponibleTotalDecimales);

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < (m_listadescuentos->rowCount() - 1); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            BlFixed importeDescuento = (totalBaseImponible * proporcion / BlFixed("100"));
            totalBaseImponible = totalBaseImponible - importeDescuento;
            totalDescuentos = totalDescuentos + importeDescuento;

            importeDescuento.setScale(baseImponibleLineaDecimales);

            QString salidatemp = det;
            QString l;
            salidatemp.replace ( "[concept" + m_listadescuentos->tableName() + "]", blStringEscape ( linea->dbValue ( "concept" + m_listadescuentos->tableName() ), tipoEscape ) );
            salidatemp.replace ( "[proporciondesc" + m_listadescuentos->tableName() + "]", blStringEscape ( l.sprintf ( "%s", linea->dbValue ( "proporcion" + m_listadescuentos->tableName() ).toLatin1().constData() ), tipoEscape ) );
            salidatemp.replace ( "[totaldesc" + m_listadescuentos->tableName() + "]", l.sprintf ( "-%s", importeDescuento.toQString().toLatin1().constData() ) );

            /// Buscamos cadenas perdidas adicionales que puedan quedar por poner.
            QRegExp rx ( "\\[(\\w*)\\]" );
            int pos =  0;

            while ( ( pos = rx.indexIn ( salidatemp, pos ) ) != -1 ) {
                if ( linea->exists ( rx.cap ( 1 ) ) ) {
                    salidatemp.replace ( pos, rx.matchedLength(), blStringEscape ( linea->dbValue ( rx.cap ( 1 ) ), tipoEscape ) );
                    pos = 0;
                } else {
                    pos += rx.matchedLength();
                }
            } // end while

            result += salidatemp;

        } // end for
    }

    return result;
}


/** Calcula los totales de factura, descuentos e impuestos.
*/
/**
\return
**/
QString BfForm::trataTotales ( const QString &det, int bimporeq )
{
    BL_FUNC_DEBUG

    QString result = "";
    QString salidaTemp = "";

    BlFixed totalSinDescuentosGenerales("0");
    BlFixed totalBaseImponible("0");
    BlFixed totalIVA("0");
    BlFixed totalRE("0");
    BlFixed totalIRPF("0");
    BlFixed totalDescuentos("0");
    BlFixed totalTotal("0");

    Base listaBaseImponible;
    Base listaRE;
    Base listaIVA;

    /// Total Base Imponible
    {
        int baseImponibleLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "pvp" + m_listalineas->tableName()); /// Tabla LINEAS
        int baseImponibleTotalDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES

        /// Calcula BASE IMPONIBLE
        for (int i = 0; i < m_listalineas->rowCount() - 1; ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed baseImponibleLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100"));
            baseImponibleLinea.setScale(baseImponibleLineaDecimales);

            BlFixed descuentoLinea = cant * pvp * descuento / BlFixed("100");
            descuentoLinea.setScale(baseImponibleLineaDecimales);
            totalDescuentos = totalDescuentos + descuentoLinea;

            /// Guarda por tipo IVA
            if (listaBaseImponible.contains(linea->dbValue("iva" + m_listalineas->tableName())))
            {
                listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] + baseImponibleLinea;
            } else {
                listaBaseImponible[linea->dbValue("iva" + m_listalineas->tableName())] = baseImponibleLinea;
            } // end if

        } // end for

        /// Suma Bases Imponibles para saber el total sin descuentos generales.
        for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
        {
            BlFixed baseImponible = i.value();
            baseImponible.setScale(baseImponibleTotalDecimales);
            totalSinDescuentosGenerales = totalSinDescuentosGenerales + baseImponible;
        } // end for

        totalSinDescuentosGenerales.setScale(baseImponibleTotalDecimales);

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
            {
                BlFixed nuevaBaseImponible = i.value() * (BlFixed("1") - proporcion / BlFixed("100"));
                listaBaseImponible[i.key()] = nuevaBaseImponible;
                totalDescuentos = totalDescuentos + (nuevaBaseImponible * proporcion / BlFixed("100"));
            } // end for
        } // end for

        /// Suma Bases Imponibles para saber el total.
        for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
        {
            BlFixed baseImponible = i.value();
            baseImponible.setScale(baseImponibleTotalDecimales);
            totalBaseImponible = totalBaseImponible + baseImponible;
        } // end for

        totalBaseImponible.setScale(baseImponibleTotalDecimales);
    }

    /// Total IVA
    {
        int impuestosLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "iva" + m_listalineas->tableName()); /// Tabla LINEAS
        int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

        /// Calcula IVA
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed iva (linea->dbValue("iva" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed impuestosLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100")) * (iva / BlFixed("100"));
            impuestosLinea.setScale(impuestosLineaDecimales);

            /// Guarda por tipo IVA
            if (listaIVA.contains(linea->dbValue("iva" + m_listalineas->tableName())))
            {
                listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] = listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] + impuestosLinea;
            } else {
                listaIVA[linea->dbValue("iva" + m_listalineas->tableName())] = impuestosLinea;
            } // end if
        } // end for

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            for (Base::Iterator i = listaIVA.begin(); i != listaIVA.end(); ++i)
            {
                BlFixed nuevoIVA = i.value() * (BlFixed("1") - proporcion / BlFixed("100"));
                listaIVA[i.key()] = nuevoIVA;
            } // end for
        } // end for

        /// Suma IVAs para saber el total.
        for (Base::Iterator i = listaIVA.begin(); i != listaIVA.end(); ++i)
        {
            BlFixed iva = i.value();
            iva.setScale(impuestosTotalDecimales);
            totalIVA = totalIVA + iva;
        } // end for

        totalIVA.setScale(impuestosTotalDecimales);
    }

    /// Total Recargo de Equivalencia
    {
        int impuestosLineaDecimales = mainCompany()->numericScale(m_listalineas->tableName(), "reqeq" + m_listalineas->tableName()); /// Tabla LINEAS
        int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

        /// Calcula Recargo de Equivalencia
        for (int i = 0; i < m_listalineas->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listalineas->lineaat(i);
            BlFixed cant (linea->dbValue("cant" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed pvp (linea->dbValue("pvp" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed descuento (linea->dbValue("descuento" + m_listalineas->tableName()).toLatin1().constData());
            BlFixed reqeq (linea->dbValue("reqeq" + m_listalineas->tableName()).toLatin1().constData());

            BlFixed impuestosLinea = cant * pvp * (BlFixed("1") - descuento / BlFixed("100")) * (reqeq / BlFixed("100"));
            impuestosLinea.setScale(impuestosLineaDecimales);

            /// Guarda por tipo RE
            if (listaRE.contains(linea->dbValue("reqeq" + m_listalineas->tableName())))
            {
                listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] = listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] + impuestosLinea;
            } else {
                listaRE[linea->dbValue("reqeq" + m_listalineas->tableName())] = impuestosLinea;
            } // end if
        } // end for

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            for (Base::Iterator i = listaRE.begin(); i != listaRE.end(); ++i)
            {
              BlFixed nuevoRE = i.value() * (BlFixed("1") - proporcion / BlFixed("100"));
                listaRE[i.key()] = nuevoRE;
            } // end for
        } // end for

        /// Suma REs para saber el total.
        for (Base::Iterator i = listaRE.begin(); i != listaRE.end(); ++i)
        {
            BlFixed re = i.value();
            re.setScale(impuestosTotalDecimales);
            totalRE = totalRE + re;
        } // end for

        totalRE.setScale(impuestosTotalDecimales);
    }

    BlFixed tipoIRPF("0");
    /// Total IRPF
    {
        BlDbRecordSet *rs;
        int totalTotalDecimales = mainCompany()->numericScale(tableName(), "total" + tableName()); /// Tabla TOTALES

        /// Usa IRPF general por defecto.
        rs = mainCompany()->loadQuery("SELECT valor::numeric AS irpf FROM configuracion WHERE LOWER(nombre) = 'irpf'");
        if (rs)
        {
            if (!rs->eof())
            {
                tipoIRPF = BlFixed(rs->value("irpf"));
            } // end if
            delete rs;
        } // end if
        /// Usar IRPF por documento si existe y no es NULL.
        QString columnaIRPF = QString("irpf") + tableName();
        if (exists(columnaIRPF))
        {
            /// Usa IRPF del documento
            if (dbValue(columnaIRPF) != QString(""))
            {
                tipoIRPF = BlFixed(dbValue(columnaIRPF));
            } // end if
        } // end if

        /// Usa IRPF general por defecto.
        QString modoIRPF("sobrebase");
        rs = mainCompany()->loadQuery("SELECT valor::varchar AS irpf_modo FROM configuracion WHERE LOWER(nombre) = 'irpf_modo'");
        if (rs)
        {
            if (!rs->eof())
            {
                modoIRPF = rs->value("irpf_modo");
            } // end if
            delete rs;
        } // end if
        /// Modo IRPF general o por documento.
        QString columnaModoIRPF = QString("irpf_modo") + tableName();
        if (exists(columnaModoIRPF))
        {
            /// Usa IRPF_MODO del documento
            if (dbValue(columnaModoIRPF) != QString(""))
            {
                modoIRPF = dbValue(columnaModoIRPF);
            } // end if
        } // end if

        /// Calcula IRPF
        if (modoIRPF == QString("sobrebase"))
        {
            totalIRPF = totalBaseImponible * (tipoIRPF / BlFixed("100"));
        }
        else if (modoIRPF == QString("sobretotal"))
        {
            totalIRPF = (totalBaseImponible + totalIVA + totalRE) * (tipoIRPF / BlFixed("100"));
        } // end if

        /// Aplica DESCUENTOS GLOBALES
        for (int i = 0; i < m_listadescuentos->rowCount(); ++i)
        {
            BlDbSubFormRecord *linea = m_listadescuentos->lineaat(i);
            BlFixed proporcion(linea->dbValue("proporcion" + m_listadescuentos->tableName()).toLatin1().constData());

            totalIRPF = totalIRPF * (BlFixed("1") - proporcion / BlFixed("100"));
        } // end for

        totalIRPF.setScale(totalTotalDecimales);
    }

    /// Total descuentos
    {
        int totalDescuentosDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES
        totalDescuentos.setScale(totalDescuentosDecimales);
    }

    /// Total total
    {
        int totalTotalDecimales = mainCompany()->numericScale(tableName(), "total" + tableName()); /// Tabla TOTALES
        totalTotal = totalBaseImponible - totalIRPF + totalIVA + totalRE;
        totalTotal.setScale(totalTotalDecimales);
    }

    BlFixed cero("0");
    cero.setScale(2);

    switch (bimporeq)
    {
        case 1:
            /// Sustituye Bases Imponibles de cada tipo de IVA.

            /// Comprueba que haya elementos. Si no sustituye con ceros.
            if (listaBaseImponible.isEmpty())
            {
                salidaTemp = det;

                salidaTemp.replace ( "[bimp]", cero.toQString());
                salidaTemp.replace ( "[tbimp]", cero.toQString());
                salidaTemp.replace ( "[iva]", cero.toQString());
                salidaTemp.replace ( "[tiva]", cero.toQString());
                salidaTemp.replace ( "[irpf]", cero.toQString());
                salidaTemp.replace ( "[tirpf]", cero.toQString());
                salidaTemp.replace ( "[totalre]", cero.toQString());
                salidaTemp.replace ( "[teoricbimp]", cero.toQString());
                salidaTemp.replace ( "[totalbimp]", cero.toQString());
                salidaTemp.replace ( "[totaldesc]", cero.toQString());
                salidaTemp.replace ( "[totaliva]", cero.toQString());
                salidaTemp.replace ( "[total]", cero.toQString());

                result += salidaTemp;

            } else {

                int baseImponibleTotalDecimales = mainCompany()->numericScale(tableName(), "bimp" + tableName()); /// Tabla TOTALES
                int impuestosTotalDecimales = mainCompany()->numericScale(tableName(), "imp" + tableName()); /// Tabla TOTALES

                for (Base::Iterator i = listaBaseImponible.begin(); i != listaBaseImponible.end(); ++i)
                {
                    salidaTemp = det;

                    BlFixed baseImponible = i.value();
                    baseImponible.setScale(baseImponibleTotalDecimales);

                    BlFixed iva = listaIVA[i.key()];
                    iva.setScale(impuestosTotalDecimales);

                    /// %IVA
                    BlFixed piva(i.key());
                    /// TODO. scale IVA?

                    /// %IVA de la Base Imponible.
                    salidaTemp.replace("[tbimp]", piva.toQString());
                    /// Importe Base Imponible.
                    salidaTemp.replace("[bimp]", baseImponible.toQString());
                    /// %IVA.
                    salidaTemp.replace("[iva]", iva.toQString());
                    /// Importe IVA para ese %IVA.
                    salidaTemp.replace("[tiva]", piva.toQString());
                    /// Total IRPF.
                    salidaTemp.replace("[irpf]", totalIRPF.toQString());
                    /// Tipo IRPF.
                    salidaTemp.replace("[tirpf]", tipoIRPF.toQString());
                    /// Total antes de descuentos generales.
                    salidaTemp.replace("[teoricbimp]", totalSinDescuentosGenerales.toQString());
                    /// Total RE.
                    salidaTemp.replace("[totalre]", totalRE.toQString());
                    /// Total Base Imponible.
                    salidaTemp.replace("[totalbimp]", totalBaseImponible.toQString());
                    /// Total Descuentos.
                    salidaTemp.replace("[totaldesc]", totalDescuentos.toQString());
                    /// Total importe IVA.
                    salidaTemp.replace("[totaliva]", totalIVA.toQString());
                    /// Total.
                    salidaTemp.replace("[total]", totalTotal.toQString());

                    result += salidaTemp;
                } // end for

            } // end if

            break;

        case 2:
            /// Sustituye Recargos de equivalencia

            if (listaRE.isEmpty())
            {
                salidaTemp = det;
                salidaTemp.replace("[re]", cero.toQString());
                salidaTemp.replace("[tre]", cero.toQString());

                result += salidaTemp;
            } else {
            
                for (Base::Iterator i = listaRE.begin(); i != listaRE.end(); ++i)
                {
                    salidaTemp = det;

                    /// Importe RE para ese %RE.
                    salidaTemp.replace("[re]", i.value().toQString());
                    /// Tipo %RE.
                    salidaTemp.replace("[tre]", i.key());

                    result += salidaTemp;
                } // end for

            } // end if

            break;

    } // end switch

    return result;
}


int BfForm::generateRML ( void )
{
    BL_FUNC_DEBUG
    int err = BlForm::generateRML();
    
    return err;
    
}


QString BfForm::templateName ( void )
{
    BL_FUNC_DEBUG
    
    
    return tableName();
}

///
/**
\param arch archivo a generar
\return 0 si ha ocurrido un error, != 0 si todo ha ido bien.
**/
int BfForm::generateRML ( const QString &arch )
{
    BL_FUNC_DEBUG
    
    try {

        if ( dbValue ( fieldId() ).isEmpty() ) {
            throw 100;
        } // end if

        /// Disparamos los plugins
        int res = g_plugins->run ( "BfForm_generateRML", this );
        if ( res != 0 ) {
            return 1;
        } // end if

        BlForm::generateRML ( arch );

        
        return 1;

    } catch ( int e ) {
        
        throw e;

    } catch ( ... ) {
        
        throw ( -1 );
    } // end try
}

///
/**
\return
**/
void BfForm::imprimir()
{
    BL_FUNC_DEBUG

    try {
        /// Disparamos los plugins
        int res = g_plugins->run ( "BfForm_imprimir", this );
        if ( res != 0 ) {
            return;
        } // end if

        {
          /// ¿Existen múltiples plantillas donde elegir?
          /// Si es así: mostrar selector de plantilla.
          /// Si solo hay 1 plantilla (por defecto) usarla directamente.
          QStringList nameFilter;
          nameFilter << (tableName() + QString(".rml"));
          nameFilter << (tableName() + QString("_*.rml"));
          QDir directory(g_confpr->value(CONF_DIR_OPENREPORTS));
          QStringList values = directory.entryList(nameFilter, QDir::Files | QDir::Readable);

          if (values.size() == 0)
          {
              /// No hay plantilla.
              blMsgError(_("No se ha encontrado plantilla de impresion."));
          } else if (values.size() == 1) {
              /// Quita la extensión .rml del nombre.
              QString templateName{values.at(0)};
              templateName = templateName.mid(0, templateName.size() - 4);
              BlForm::imprimir(templateName);
          } else {
              BlTemplateSelector *templateSelector = new BlTemplateSelector(this);
              templateSelector->setValues(values);
              int result = templateSelector->exec();

              QString templateName{templateSelector->value()};

              /// Quita la extensión .rml del nombre.
              templateName = templateName.mid(0, templateName.size() - 4);

              delete templateSelector;

              if ((result == QDialog::Accepted) && (!templateName.isEmpty()))
              {
                  BlForm::imprimir(templateName);
              } // end if
          } // end if
        }

    } catch ( int e ) {
        if ( e == 100 ) {
            /// El documento no se ha guardado y no se dispone en la base de datos de estos datos.
            blMsgInfo ( _ ( "Tiene que guardar el documento antes de poder procesarlo." ) );
        } // end if
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error inesperado en la impresion" ) );
    } // end try
}

///
/**
\param form
**/
void BfForm::setListaLineas ( BfSubForm *form )
{
    BL_FUNC_DEBUG
    
    m_listalineas = form;
    
    
}

///
/**
\param form
**/
void BfForm::setListaDescuentos ( BfSubForm *form )
{
    BL_FUNC_DEBUG
    
    m_listadescuentos = form;
    
    
}

///
/**
\param form
**/
/*
void BfForm::setTotalIvaAdicional ( QString value )
{
    BL_FUNC_DEBUG
    
    m_totalIvaAdicional = BlFixed(value);
    
    
}
*/

///
/**
\param form
**/
/*
void BfForm::setTotalIvaAdicional ( BlFixed value )
{
    BL_FUNC_DEBUG
    
    m_totalIvaAdicional = value;
    
    
}
*/

///
/**
\param form
**/
/*
BlFixed BfForm::getTotalIvaAdicional ( )
{
    BL_FUNC_DEBUG
    
    return m_totalIvaAdicional;
    
}
*/
///
/**
\return
**/
BfSubForm* BfForm::getlistalineas()
{
    BL_FUNC_DEBUG
    
    
    return m_listalineas;
}

///
/**
\return
**/
BfSubForm* BfForm::getlistadescuentos()
{
    BL_FUNC_DEBUG
    
    
    return m_listadescuentos;
}


void BfForm::imprimeIvaAdicional()
{
fprintf(stderr, "Z\n");


	foreach (QString origen, m_ivaAdicional.keys()) {
            fprintf(stderr, "Z3\n");

            QList<QPair<BlFixed, BlFixed> > valores = m_ivaAdicional[origen];
            fprintf(stderr, "Z4\n");
            for (int i = 0; i < valores.size(); ++i) {
                fprintf(stderr, "Z5\n");
                QPair<BlFixed, BlFixed> par = valores.at(i);
                QString salida = origen + " :: " + par.first.toQString() + " :: " + par.second.toQString() + "\n";
                fprintf(stderr, salida.toLatin1());
            } // end for
            fprintf(stderr, "Z6\n");
        } // end foreach
        fprintf(stderr, "Z7\n");
}

void BfForm::testIVA()
{
	/// Imprime contenido de IVA adicional
	imprimeIvaAdicional();
	
	fprintf(stderr , "\n-------------------\n");
/*	
	/// Crea un origen y mete valores
	setIvaAdicional("pluginivaadicional", BlFixed("18.00"), BlFixed("3.02") );
        setIvaAdicional("pluginivaadicional", BlFixed("8.00"), BlFixed("0.12") );
	
	
	/// Imprime contenido
	
	foreach (QString origen, m_ivaAdicional.keys()) {
	
		QHash<BlFixed *, BlFixed *> valores = m_ivaAdicional[origen];

		QHash<BlFixed *, BlFixed *>::const_iterator i = valores.constBegin();
		while (i != valores.constEnd()) {
                        QString salida = origen + " :: " + i.key()->toQString() + " :: " + i.value()->toQString() + "\n";
                        fprintf(stderr, salida.toLatin1());
		} // end while
	} // end foreach
	
	fprintf(stderr , "\n-------------------\n");
	
	/// Crea otro origen y mete valores
	
	setIvaAdicional("otroplugin", BlFixed("21.00"), BlFixed("0.02") );
        setIvaAdicional("otroplugin", BlFixed("12.00"), BlFixed("1.12") );
	
	
	/// Imprime contenido
	
	foreach (QString origen, m_ivaAdicional.keys()) {
	
		QHash<BlFixed *, BlFixed *> valores = m_ivaAdicional[origen];

		QHash<BlFixed *, BlFixed *>::const_iterator i = valores.constBegin();
		while (i != valores.constEnd()) {
                        QString salida = origen + " :: " + i.key()->toQString() + " :: " + i.value()->toQString() + "\n";
                        fprintf(stderr, salida.toLatin1());
		} // end while
	} // end foreach
	
	fprintf(stderr , "\n-------------------\n");


	/// Borra un tipo de IVA
	
	deleteIvaAdicional("pluginivaadicional", BlFixed("18.00"));
	deleteIvaAdicional("otroplugin", BlFixed("12.00"));


	/// Imprime contenido
	
		
	foreach (QString origen, m_ivaAdicional.keys()) {
	
		QHash<BlFixed *, BlFixed *> valores = m_ivaAdicional[origen];

		QHash<BlFixed *, BlFixed *>::const_iterator i = valores.constBegin();
		while (i != valores.constEnd()) {
                        QString salida = origen + " :: " + i.key()->toQString() + " :: " + i.value()->toQString() + "\n";
                        fprintf(stderr, salida.toLatin1());
		} // end while
	} // end foreach
	
	fprintf(stderr , "\n-------------------\n");
*/
}


void BfForm::setIvaAdicional(QString origen, BlFixed porcentajeIVA, BlFixed ivaAdicional)
{
	QList<QPair<BlFixed, BlFixed> > valores = m_ivaAdicional[origen];
        valores.append(qMakePair(porcentajeIVA, ivaAdicional));
}


BlFixed BfForm::ivaAdicional(BlFixed porcentajeIVA)
{
	BlFixed resultado = BlFixed("0.0");
        /// Busca en el hash todos los porcentajes iguales y los suma.

        foreach (QString origen, m_ivaAdicional.keys()) {
            QList<QPair<BlFixed, BlFixed> > valores = m_ivaAdicional[origen];
            QList<QPair<BlFixed, BlFixed> >::const_iterator i = valores.constBegin();
            for (int i = 0; i < valores.size(); ++i) {
                QPair<BlFixed, BlFixed> par = valores.at(i);
                if (par.first == porcentajeIVA) {
                    resultado = resultado + par.second;
                }
            } // end for
        } // end foreach
	
	
	return resultado;
}



void BfForm::deleteIvaAdicional(QString origen, BlFixed porcentajeIVA)
{
    /// Busca en el hash de origen el porcentaje de IVA y lo borra.

    QList<QPair<BlFixed, BlFixed> > valores = m_ivaAdicional[origen];

    for (int i = 0; i < valores.size(); ++i) {
        QPair<BlFixed, BlFixed> par = valores.at(i);
        if (par.first == porcentajeIVA) {
            valores.removeOne(par);
        }
    } // end for
    
}


void BfForm::recogeValores()
{
    BL_FUNC_DEBUG

    /// 1) Primero captura datos según BulmaLib.
    BlForm::recogeValores();

    /// 2) Luego captura datos según BulmaFact. BulmaLib no conoce nada sobre los Widgets de BulmaFact
    /// y los datos capturados en el punto 1 no son fiables.
    BlDbField *campo;
    /// Recorremos todos los campos definidos.
    for (int i = 0; i < m_lista.size(); ++i)
    {
        campo = m_lista.at(i);

        /// Buscamos un BfBuscarRegimenFiscal con nombre coincidente.
        BfBuscarRegimenFiscal *d0 = findChild<BfBuscarRegimenFiscal *> ( "mui_" + campo->fieldName() );
        if (d0)
        {
            campo->set (d0->fieldValue());
           continue;
       } // end if

        /// Buscamos un BfBuscarAlmacen con nombre coincidente.
        BfBuscarAlmacen *d1 = findChild<BfBuscarAlmacen *> ( "mui_" + campo->fieldName() );
        if (d1)
        {
            campo->set (d1->id());
       qDebug() << "almacen " << d1->id();
           continue;
       } // end if

        /// Buscamos un BfBuscarArticulo con nombre coincidente.
        BfBuscarArticulo *d2 = findChild<BfBuscarArticulo *> ( "mui_" + campo->fieldName() );
        if (d2)
        {
            campo->set (d2->idarticulo());
           continue;
       } // end if

        /// Buscamos un BfBuscarBanco con nombre coincidente.
        BfBuscarBanco *d3 = findChild<BfBuscarBanco *> ( "mui_" + campo->fieldName() );
        if (d3)
        {
            campo->set (d3->id());
           continue;
       } // end if

        /// Buscamos un BfBuscarFamilia con nombre coincidente.
        BfBuscarFamilia *d4 = findChild<BfBuscarFamilia *> ( "mui_" + campo->fieldName() );
        if (d4)
        {
            campo->set (d4->idfamilia());
           continue;
       } // end if

        /// Buscamos un BfBuscarFormaPago con nombre coincidente.
        BfBuscarFormaPago *d5 = findChild<BfBuscarFormaPago *> ( "mui_" + campo->fieldName() );
        if (d5)
        {
            campo->set (d5->id());
           continue;
       } // end if

        /// Buscamos un BfBuscarReferencia con nombre coincidente.
        BfBuscarReferencia *d6 = findChild<BfBuscarReferencia *> ( "mui_" + campo->fieldName() );
        if (d6)
        {
            campo->set (d6->fieldValue());
           continue;
       } // end if

        /// Buscamos un BfBuscarSerieFactura con nombre coincidente.
        BfBuscarSerieFactura *d7 = findChild<BfBuscarSerieFactura *> ( "mui_" + campo->fieldName() );
        if (d7)
        {
            campo->set (d7->id());
           continue;
       } // end if

        /// Buscamos un BfBuscarTipoArticulo con nombre coincidente.
        BfBuscarTipoArticulo *d8 = findChild<BfBuscarTipoArticulo *> ( "mui_" + campo->fieldName() );
        if (d8)
        {
            campo->set (d8->idtipo_articulo());
           continue;
       } // end if

        /// Buscamos un BfBuscarTipoIVA con nombre coincidente.
        BfBuscarTipoIVA *d9 = findChild<BfBuscarTipoIVA *> ( "mui_" + campo->fieldName() );
        if (d9)
        {
            campo->set (d9->id());
           continue;
       } // end if

        /// Buscamos un BfBuscarTrabajador con nombre coincidente.
        BfBuscarTrabajador *d10 = findChild<BfBuscarTrabajador *> ( "mui_" + campo->fieldName() );
        if (d10)
        {
            campo->set (d10->id());
           continue;
       } // end if

    } // end for
    
}

