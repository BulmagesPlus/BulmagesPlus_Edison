/***************************************************************************
 *   Copyright (C) 2003 by Santiago Capel Torres.                          *
 *   Copyright (C) 2003 by Tomeu Borrás Riera.                             *
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
 ***************************************************************************/

#include <stdio.h>

#include <QDateTime>
#include <QTextStream>
#include <QObject>
#include <QString>
#include <QMessageBox>
#include <QXmlDefaultHandler>

#include "blapplication.h"
#include "blimportexport.h"
#include "local_blI18n.h"

#define EURO                166.386
#define S_EURO             "166.386"

#define LEN_CODIGO_CUENTA    12
#define LEN_TITULO           40
#define LEN_NIF              15
#define LEN_DOMICILIO        35
#define LEN_POBLACION        25
#define LEN_PROVINCIA        20
#define LEN_CODPOSTAL         5
#define LEN_DIVISA            1
#define LEN_CTA_CODDIVISA     5
#define LEN_CTA_DOCUMENTO     1
#define LEN_AJUSTAME          1
#define LEN_TIPOIVA           1

#define LEN_ASIEN             6
#define LEN_FECHA             8
#define LEN_SUBCTA           12
#define LEN_CONTRA           12
#define LEN_PTADEBE          16
#define LEN_CONCEPTO         25
#define LEN_PTAHABER         16
#define LEN_FACTURA           8
#define LEN_BASEIMPO         16
#define LEN_IVA               5
#define LEN_RECEQUIV          5
#define LEN_DOCUMENTO        10
#define LEN_DEPARTA           3
#define LEN_CLAVE             6
#define LEN_ESTADO            1
#define LEN_NCASADO           6
#define LEN_TCASADO           1
#define LEN_TRANS             6
#define LEN_CAMBIO           16
#define LEN_DEBEME           16
#define LEN_HABERME          16
#define LEN_AUXILIAR          1
#define LEN_SERIE             1
#define LEN_SUCURSAL          4
#define LEN_CODDIVISA         5
#define LEN_IMPAUXME         16
#define LEN_MONEDAUSO         1
#define LEN_EURODEBE         16
#define LEN_EUROHABER        16
#define LEN_BASEEURO         16
#define LEN_NOCONV            1
#define LEN_NUMEROINV        10





/// ---------------------------------------------

/// Clase para leer archivos de XML y hacer la importacion de datos.
class StructureParser : public QXmlDefaultHandler
{
private:
    BlPostgreSqlClient *dbConnection;
    QString cadintermedia; /// Esta variable va almacenando los valores que van saliendo en la clase.
    /// Variables usadas para almacenar los datos de un asiento.
    QString idasiento;
    QString ordenasiento;
    QString fechaasiento;
    /// Variables usadas para almacenar los datos de un apunte.
    QString idapunte;
    QString idborrador;
    QString fechaapunte;
    QString codigocuentaapunte;
    QString debeapunte;
    QString haberapunte;
    QString conceptocontableapunte;
    int m_ordenapunte;
    /// Variables usadas para almacenar los datos de una cuenta.
    QString idcuenta;
    QString descripcioncuenta;
    QString codigocuenta;
    QString codigopadre;
    QString m_bloqueadaCuenta;
    QString m_nodebeCuenta;
    QString m_nohaberCuenta;
    QString m_tipoCuenta;
    /// Variables usadas para almacenar los datos del registro de IVA.
    QString m_idRegistroIva;
    QString m_rIvaContrapartida;
    QString m_rIvaBaseImp;
    QString m_rIvaIva;
    QString m_rIvaFFactura;
    QString m_rIvaFactura;
    QString m_rIvaCIF;
    QString m_rIvaIdFPago;
    QString m_rIvRecRegIva;
    /// Variables usadas para almacenar los datos de la tabla de IVA's.
    QString m_idTipoIva;
    QString m_baseIva;
    QString m_nombreTipoIva;
    /// El tagpadre indica en que posicion estamos. Si estamos en un asiento, un apunte, una cuenta, etc etc etc.
    QString tagpadre;

public:
    StructureParser ( BlPostgreSqlClient *, unsigned int tip = IMPORT_TODO );
    ~StructureParser();
    bool startDocument();
    bool startElement ( const QString&, const QString&, const QString&, const QXmlAttributes& );
    bool endElement ( const QString&, const QString&, const QString& );
    bool characters ( const QString& );

private:
    QString indent;
    unsigned int m_tipo;
};


/// Clase para leer archivos de XML y hacer la importacion de datos.
/// Usamos este tipo para almacenar todos los valores que va recogiendo la clase.
typedef QMap<QString, QString> tvalores;


class ImportBulmaFact : public QXmlDefaultHandler
{
private:
    BlPostgreSqlClient *dbConnection;
    QString cadintermedia; /// Esta variable va almacenando los valores que van saliendo en la clase.
    /// Variables usadas para almacenar los datos de un asiento.
    tvalores valores;
    /// El tagpadre indica en que posicion estamos. Si estamos en un asiento, un apunte, una cuenta, etc etc etc.
    QString tagpadre;
    BlImportExport *pgimport;
    /// Estas estructuras sirven para guardar datos intermedios.
    QList<tvalores> listalpresupuesto;
    QList<tvalores> listadpresupuesto;
    QList<tvalores> listalfactura;
    QList<tvalores> listadfactura;
    QList<tvalores> listalalbaran;
    QList<tvalores> listadalbaran;
    QList<tvalores> listalpedidocliente;
    QList<tvalores> listadpedidocliente;

public:
    ImportBulmaFact ( BlImportExport *, BlPostgreSqlClient *, unsigned long long int tip = IMPORT_TODO );
    ~ImportBulmaFact();
    bool startDocument();
    bool startElement ( const QString&, const QString&, const QString&, const QXmlAttributes& );
    bool endElement ( const QString&, const QString&, const QString& );
    bool characters ( const QString& );

private:
    QString indent;
    unsigned int m_tipo;
    void printcontents();
    int trataCliente();
    int trataProveedor();
    int trataFormaPago();
    int trataAlmacen();
    int trataArticulo();
    int trataFactura();
    int trataLFactura();
    int trataDFactura();
    int trataPresupuesto();
    int trataLPresupuesto();
    int trataDPresupuesto();
    int trataFamilia();
    int trataAlbaran();
    int trataLAlbaran();
    int trataDAlbaran();
    int trataPedidoCliente();
    int trataLPedidoCliente();
    int trataDPedidoCliente();
};


/// ---------------------------------------------


///
/**
**/
void BlImportExport::alerta ( int, int )
{
    BL_FUNC_DEBUG
    
}


///
/**
**/
void BlImportExport::mensajeria ( QString )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "Funcion no implementada en esta clase\n", 2 );
}


///
/**
\param f
**/
void BlImportExport::setFInicial ( QString f )
{
    BL_FUNC_DEBUG
    m_fInicial = f;
    
}


///
/**
\param f
**/
void BlImportExport::setFFinal ( QString f )
{
    BL_FUNC_DEBUG
    m_fFinal = f;
    
}


///
/**
**/
void BlImportExport::setModoTest()
{
    BL_FUNC_DEBUG
    m_modoTest = true;
    
}


///
/**
**/
void BlImportExport::setModoNormal()
{
    BL_FUNC_DEBUG
    m_modoTest = false;
    
}


///
/**
\return
**/
bool BlImportExport::modoTest()
{
    BL_FUNC_DEBUG
    
    return ( m_modoTest == true );
}

///
/**
**/
BlImportExport::~BlImportExport()
{
    BL_FUNC_DEBUG
    
}

///
/**
\param con
**/
BlImportExport::BlImportExport ( BlPostgreSqlClient *con )
{
    BL_FUNC_DEBUG
    dbConnection = con;
    m_fInicial = "";
    m_fFinal = "";
    setModoNormal();
    
}

/// Esta funcion se encarga de pasar los datos de BulmaGes a Contaplus.
/**
\param subcuentas
\param asientos
\return
**/
int BlImportExport::bulmages2Contaplus ( QFile &subcuentas, QFile &asientos )
{
    BL_FUNC_DEBUG
    QString codigo, descripcion;
    QString strblancomax;
    QTextStream stream ( &subcuentas );
    QTextStream streamas ( &asientos );
    /// Se supone que ho hay campos mayores de 100 caracteres para que el algoritmo funcione.
    strblancomax.fill ( ' ', 100 );
    /// Solo se van a exportar las cuentas utilizadas, Ya que contaplus no hace
    /// ordenacion en arbol.
    QString query = "SELECT * FROM cuenta WHERE idcuenta IN (SELECT DISTINCT idcuenta FROM apunte)";
    dbConnection->begin();
    BlDbRecordSet *curcta = dbConnection->loadQuery ( query, "elquery" );
    dbConnection->commit();
    while ( !curcta->eof() ) {
        QString linea = "";
        linea += ( curcta->value( "codigo" ) + strblancomax ).left ( LEN_CODIGO_CUENTA );
        linea += ( curcta->value( "descripcion" ) + strblancomax ).left ( LEN_TITULO );
        linea += ( curcta->value( "cifent_cuenta" ) + strblancomax ).left ( LEN_NIF );
        linea += ( curcta->value( "dirent_cuenta" ) + strblancomax ).left ( LEN_DOMICILIO );
        linea += ( strblancomax ).left ( LEN_POBLACION );
        linea += ( strblancomax ).left ( LEN_CODPOSTAL );
        linea += ( strblancomax ).left ( LEN_DIVISA );
        linea += ( strblancomax ).left ( LEN_CTA_CODDIVISA );
        linea += ( strblancomax ).left ( LEN_CTA_DOCUMENTO );
        linea += ( strblancomax ).left ( LEN_AJUSTAME );
        linea += ( strblancomax ).left ( LEN_TIPOIVA );
        linea += "\n";
        stream << linea;
        curcta->nextRecord();
    } // end while
    delete curcta;
    query = "SELECT * FROM asiento, apunte, cuenta WHERE asiento.idasiento = apunte.idasiento AND cuenta.idcuenta = apunte.idcuenta ";
    if ( m_fInicial != "" )
        query += " AND asiento.fecha >= '" + m_fInicial + "'";
    if ( m_fFinal != "" )
        query += " AND asiento.fecha <= '" + m_fFinal + "'";
    query += " ORDER BY asiento.idasiento ";
    dbConnection->begin();
    BlDbRecordSet *curas = dbConnection->loadQuery ( query, "masquery" );
    dbConnection->commit();
    int i = 0;
    int numreg = curas->numregistros() + 1;
    while ( !curas->eof() ) {
        alerta ( i++, numreg );
        QString linea = "";
        linea += ( strblancomax + curas->value( "ordenasiento" ) ).right ( LEN_ASIEN );
        QString fecha = curas->value( "fecha" );
        fecha = fecha.mid ( 6, 4 ) + fecha.mid ( 3, 2 ) + fecha.mid ( 0, 2 );
        linea += ( fecha + strblancomax ).left ( LEN_FECHA );
        linea += ( curas->value( "codigo" ) + strblancomax ).left ( LEN_SUBCTA );
        linea += ( strblancomax ).left ( LEN_CONTRA );
        QString cadaux;
        cadaux.sprintf ( "%2.2f", curas->value( "debe" ).toFloat() );
        linea += ( strblancomax + cadaux ).right ( LEN_PTADEBE );
        linea += ( curas->value( "conceptocontable" ) + strblancomax ).left ( LEN_CONCEPTO );
        cadaux.sprintf ( "%2.2f", curas->value( "haber" ).toFloat() );
        linea += ( strblancomax + cadaux ).right ( LEN_PTAHABER );
        linea += ( strblancomax ).left ( LEN_FACTURA );
        linea += ( strblancomax ).left ( LEN_BASEIMPO );
        linea += ( strblancomax ).left ( LEN_IVA );
        linea += ( strblancomax ).left ( LEN_RECEQUIV );
        linea += ( strblancomax ).left ( LEN_DOCUMENTO );
        linea += ( strblancomax ).left ( LEN_DEPARTA );
        linea += ( strblancomax ).left ( LEN_CLAVE );
        linea += ( strblancomax ).left ( LEN_ESTADO );
        linea += ( strblancomax ).left ( LEN_NCASADO );
        linea += ( strblancomax ).left ( LEN_TCASADO );
        linea += ( strblancomax ).left ( LEN_TRANS );
        linea += ( strblancomax ).left ( LEN_CAMBIO );
        linea += ( strblancomax + "0.00" ).right ( LEN_DEBEME );
        linea += ( strblancomax + "0.00" ).right ( LEN_HABERME );
        linea += ( strblancomax ).left ( LEN_AUXILIAR );
        linea += ( strblancomax ).left ( LEN_SERIE );
        linea += ( strblancomax ).left ( LEN_SUCURSAL );
        linea += ( strblancomax ).left ( LEN_CODDIVISA );
        linea += ( strblancomax ).left ( LEN_IMPAUXME );
        linea += ( "2" + strblancomax ).left ( LEN_MONEDAUSO );
        /// Para evitar redondeos usamos el valor devuelto en forma de texto por la
        /// base de datos que ya opera ella en punto fijo.
        cadaux.sprintf ( "%2.2f", curas->value( "debe" ).toFloat() );
        cadaux = curas->value( "debe" );
        linea += ( strblancomax + cadaux ).right ( LEN_EURODEBE );
        cadaux.sprintf ( "%2.2f", curas->value( "haber" ).toFloat() );
        cadaux = curas->value( "haber" );
        linea += ( strblancomax + cadaux ).right ( LEN_EUROHABER );
        linea += ( strblancomax + "0.00" ).right ( LEN_BASEEURO );
        linea += ( "F" + strblancomax ).left ( LEN_NOCONV );
        linea += ( strblancomax ).left ( LEN_NUMEROINV );
        linea += "\n";
        mensajeria ( _ ( "Exportando :" ) + curas->value( "codigo" ) + "--" + fecha + "\n" );
        streamas << linea;
        curas->nextRecord();
    } // end while
    delete curas;
    alerta ( 100, 100 );
    
    return 0;
} // end if


/// Pasa archivos de Contaplus a BulmaGes
/**
\param subcuentas
\param asientos
\return
**/
int BlImportExport::contaplus2Bulmages ( QFile &subcuentas, QFile &asientos )
{
    BL_FUNC_DEBUG
    QString idasiento;
    QString lopd_str, cuenta_str;
    QString debe, haber;
    QString query;
    QDate fecha1;
    QDate fechain ( 1, 1, 1 );
    QDate fechafi ( 2999, 12, 31 );
    int orden = 0;
    if ( m_fInicial != "" )
        fechain.setDate ( m_fInicial.mid ( 6, 4 ).toInt(), m_fInicial.mid ( 3, 2 ).toInt(), m_fInicial.mid ( 0, 2 ).toInt() );
    if ( m_fFinal != "" )
        fechafi.setDate ( m_fFinal.mid ( 6, 4 ).toInt(), m_fFinal.mid ( 3, 2 ).toInt(), m_fFinal.mid ( 0, 2 ).toInt() );
    /// Subcuentas.
    QTextStream stream ( &subcuentas );
    while ( !subcuentas.atEnd() ) {
        alerta ( subcuentas.pos() + asientos.pos(), subcuentas.size() + asientos.size() );
        QString line = stream.readLine();
        if ( line.length() < 2 )
            break;
        int pos = 0;
        QString cod = line.mid ( pos, LEN_CODIGO_CUENTA ).trimmed();
        pos += LEN_CODIGO_CUENTA;
        QString titulo = line.mid ( pos, LEN_TITULO ).trimmed();
        pos += LEN_TITULO;
        QString nif = line.mid ( pos, LEN_NIF ).trimmed();
        pos += LEN_NIF;
        QString domicilio = line.mid ( pos, LEN_DOMICILIO ).trimmed();
        pos += LEN_DOMICILIO;
        QString poblacion = line.mid ( pos, LEN_POBLACION ).trimmed();
        pos += LEN_POBLACION;
        QString provincia = line.mid ( pos, LEN_PROVINCIA ).trimmed();
        pos += LEN_PROVINCIA;
        QString codpostal = line.mid ( pos, LEN_CODPOSTAL ).trimmed();
        pos += LEN_CODPOSTAL;
        QString divisa = line.mid ( pos, LEN_DIVISA ).trimmed();
        pos += LEN_DIVISA;
        QString cta_coddivisa = line.mid ( pos, LEN_CTA_CODDIVISA ).trimmed();
        pos += LEN_CTA_CODDIVISA;
        QString cta_documento = line.mid ( pos, LEN_CTA_DOCUMENTO ).trimmed();
        pos += LEN_CTA_DOCUMENTO;
        QString ajustame = line.mid ( pos, LEN_AJUSTAME ).trimmed();
        pos += LEN_AJUSTAME;
        QString tipoiva = line.mid ( pos, LEN_TIPOIVA ).trimmed();
        pos += LEN_TIPOIVA;
        /// Antes de hacer una insercion comprobamos que la cuenta no exista ya en el sistema.
        QString query = "SELECT * FROM cuenta WHERE codigo = '" + cod + "'";
        dbConnection->begin();
        BlDbRecordSet *cursaux = dbConnection->loadQuery ( query, "hol" );
        dbConnection->commit();
        if ( cursaux->eof() ) {
            if ( !cod.isEmpty() ) {
                QString padre = searchParent ( cod );
                QString idgrupo = cod.left ( 1 );
                query = "INSERT INTO cuenta (imputacion, activo, tipocuenta, codigo, descripcion, cifent_cuenta, padre, nombreent_cuenta, dirent_cuenta, telent_cuenta, coment_cuenta, bancoent_cuenta, emailent_cuenta, webent_cuenta) VALUES  (TRUE, TRUE, 1,'" + cod + "', '" + titulo + "', '" + nif + "', " + padre + ", 'importada de ContaPlus','" + domicilio + poblacion + provincia + codpostal + "','','','','','')";
                dbConnection->begin();
                int error = dbConnection->runQuery ( query );
                if ( error ) {
                    dbConnection->rollback();
                    return 1;
                } // end if
                dbConnection->commit();
                mensajeria ( _ ( "<LI>Se ha insertado la cuenta " ) + cod + "</LI>\n" );
            } // end if
        } else {
            mensajeria ( _ ( "<LI>Ya hay una cuenta con el codigo " ) + cod + "</LI>\n" );
        } // end if
        delete cursaux;
    } // end while

    QTextStream stream2 ( &asientos );
    QString lastasiento = "0";
    int napunte = 0;
    while ( !asientos.atEnd() ) {
        alerta ( subcuentas.pos() + asientos.pos(), subcuentas.size() + asientos.size() );
        QString line = stream2.readLine();
        if ( line.length() < 2 )
            break;
        int pos = 0;
        QString asiento = line.mid ( pos, LEN_ASIEN ).trimmed();
        pos += LEN_ASIEN;
        QString fecha = line.mid ( pos, LEN_FECHA ).trimmed();
        fecha1.setDate ( fecha.mid ( 0, 4 ).toInt(), fecha.mid ( 4, 2 ).toInt(), fecha.mid ( 6, 2 ).toInt() );
        fecha = fecha.mid ( 0, 4 ) + "-" + fecha.mid ( 4, 2 ) + "-" + fecha.mid ( 6, 2 );
        pos += LEN_FECHA;
        QString subcta = line.mid ( pos, LEN_SUBCTA ).trimmed();
        pos += LEN_SUBCTA;
        QString contra = line.mid ( pos, LEN_CONTRA ).trimmed();
        pos += LEN_CONTRA;
        QString ptadebe = line.mid ( pos, LEN_PTADEBE ).trimmed();
        pos += LEN_PTADEBE;
        QString concepto = line.mid ( pos, LEN_CONCEPTO ).trimmed();
        pos += LEN_CONCEPTO;
        QString ptahaber = line.mid ( pos, LEN_PTAHABER ).trimmed();
        pos += LEN_PTAHABER;
        QString factura = line.mid ( pos, LEN_FACTURA ).trimmed();
        pos += LEN_FACTURA;
        QString baseimpo = line.mid ( pos, LEN_BASEIMPO ).trimmed();
        pos += LEN_BASEIMPO;
        QString iva = line.mid ( pos, LEN_IVA ).trimmed();
        pos += LEN_IVA;
        QString recequiv = line.mid ( pos, LEN_RECEQUIV ).trimmed();
        pos += LEN_RECEQUIV;
        QString documento = line.mid ( pos, LEN_DOCUMENTO ).trimmed();
        pos += LEN_DOCUMENTO;
        QString departa = line.mid ( pos, LEN_DEPARTA ).trimmed();
        pos += LEN_DEPARTA;
        QString clave = line.mid ( pos, LEN_CLAVE ).trimmed();
        pos += LEN_CLAVE;
        QString estado = line.mid ( pos, LEN_ESTADO ).trimmed();
        pos += LEN_ESTADO;
        QString ncasado = line.mid ( pos, LEN_NCASADO ).trimmed();
        pos += LEN_NCASADO;
        QString tcasado = line.mid ( pos, LEN_TCASADO ).trimmed();
        pos += LEN_TCASADO;
        QString trans = line.mid ( pos, LEN_TRANS ).trimmed();
        pos += LEN_TRANS;
        QString cambio = line.mid ( pos, LEN_CAMBIO ).trimmed();
        pos += LEN_CAMBIO;
        QString debeme = line.mid ( pos, LEN_DEBEME ).trimmed();
        pos += LEN_DEBEME;
        QString haberme = line.mid ( pos, LEN_HABERME ).trimmed();
        pos += LEN_HABERME;
        QString auxiliar = line.mid ( pos, LEN_AUXILIAR ).trimmed();
        pos += LEN_AUXILIAR;
        QString serie = line.mid ( pos, LEN_SERIE ).trimmed();
        pos += LEN_SERIE;
        QString sucursal = line.mid ( pos, LEN_SUCURSAL ).trimmed();
        pos += LEN_SUCURSAL;
        QString coddivisa = line.mid ( pos, LEN_CODDIVISA ).trimmed();
        pos += LEN_CODDIVISA;
        QString impauxme = line.mid ( pos, LEN_IMPAUXME ).trimmed();
        pos += LEN_IMPAUXME;
        QString monedauso = line.mid ( pos, LEN_MONEDAUSO ).trimmed();
        pos += LEN_MONEDAUSO;
        QString eurodebe = line.mid ( pos, LEN_EURODEBE ).trimmed();
        pos += LEN_EURODEBE;
        QString eurohaber = line.mid ( pos, LEN_EUROHABER ).trimmed();
        pos += LEN_EUROHABER;
        QString baseeuro = line.mid ( pos, LEN_BASEEURO ).trimmed();
        pos += LEN_BASEEURO;
        QString noconv = line.mid ( pos, LEN_NOCONV ).trimmed();
        pos += LEN_NOCONV;
        QString numeroinv = line.mid ( pos, LEN_NUMEROINV ).trimmed();
        pos += LEN_NUMEROINV;

        if ( asiento != lastasiento ) {
            if ( lastasiento != "0" ) {
                query = "SELECT cierraasiento(" + idasiento + ")";
                if ( !modoTest() ) {
                    dbConnection->begin();
                    BlDbRecordSet * cur = dbConnection->loadQuery ( query, "hola" );
                    dbConnection->commit();
                    delete cur;
                } // end if
            } // end if
            if ( fecha1 >= fechain && fecha1 <= fechafi ) {
                query = "INSERT INTO asiento (fecha, comentariosasiento, clase) VALUES ('" + fecha + "','Importado de Contaplus', 1 )";
                dbConnection->begin();
                if ( !modoTest() ) {
                    int error = dbConnection->runQuery ( query );
                    if ( error ) {
                        dbConnection->rollback();
                        return 1;
                    } // end if
                } // end if
                query = "SELECT max(idasiento) as idasiento FROM asiento";
                BlDbRecordSet *cur = dbConnection->loadQuery ( query );
                idasiento = cur->value( "idasiento" );
                dbConnection->commit();
                delete cur;
                napunte = 0;
                lastasiento = asiento;
                orden = 0;
                mensajeria ( _ ( "<LI>Insercion de asiento" ) + idasiento + "</LI>\n" );
            } // end if
        } // end if
        napunte++;
        if ( monedauso == "1" ) { /// Ptas.
            /// Aqui esta el peor error cometido, usar punto flotante
            debe = ptahaber + "/" + S_EURO;
            haber = ptadebe + "/" + S_EURO;
        } else {
            debe = eurodebe;
            haber = eurohaber;
        } // end if
        query = "SELECT * FROM cuenta WHERE codigo = '" + subcta + "'";
        dbConnection->begin();
        BlDbRecordSet *cur = dbConnection->loadQuery ( query, "elquery" );
        dbConnection->commit();
        if ( !cur->eof() ) {
            if ( fecha1 >= fechain && fecha1 <= fechafi ) {
                if ( !modoTest() ) {
                    query = "INSERT INTO borrador (idasiento, idcuenta, fecha, conceptocontable, debe, haber, orden) VALUES (" + idasiento + ", id_cuenta('" + subcta + "'), '" + fecha + "','" + concepto + "'," + debe + "," + haber + "," + QString::number ( orden++ ) + " )";
                    dbConnection->begin();
                    int error = dbConnection->runQuery ( query );
                    if ( error ) {
                        dbConnection->rollback();
                        return 1;
                    } // end if
                    dbConnection->commit();
                } // end if
                mensajeria ( _ ( "<LI>Insercion de apunte" ) + subcta + "," + concepto + "</LI>\n" );
            } else {
                mensajeria ( _ ( "<LI>Apunte fuera de fecha</LI>\n" ) );
            } // end if
        } // end if
    } // end while
    if ( lastasiento != "0" ) {
        if ( !modoTest() ) {
            query = "SELECT cierraasiento(" + idasiento + ")";
            BlDbRecordSet * cur = dbConnection->loadQuery ( query );
            delete cur;
        } // end if
    } // end if
    mensajeria ( _ ( "<LI>Terminado</LI>\n" ) );
    alerta ( subcuentas.size() + asientos.size(), subcuentas.size() + asientos.size() );
    
    return 0;
}


/// Esta funcion busca en la base de datos la cuenta padre de la cuenta seleccionada.
/// Si no existe la cuenta padre devuelve NULL.

/**
\param cod
\return
**/
QString BlImportExport::searchParent ( QString cod )
{
    BL_FUNC_DEBUG
    QString padre = "NULL"; /// Almacena el padre de la cuenta.
    QString query;
    int i = 2;
    int fin = 0;
    while ( !fin ) {
        query = "SELECT * FROM cuenta WHERE codigo = '" + cod.left ( i ) + "'";
        dbConnection->begin();
        BlDbRecordSet *cur = dbConnection->loadQuery ( query, "unquery" );
        dbConnection->commit();
        if ( !cur->eof() ) {
            padre = cur->value( "idcuenta" );
        } else {
            fin = 1;
        } // end if
        delete cur;
        i++;
    } // end while
    
    return padre;
}


/// Esta funcion pasa de bulmafact a XML.
/**
\param xmlfile
\param tipo
\return
**/
int BlImportExport::bulmafact2XML ( QFile &xmlfile, unsigned long long int tipo )
{
    BL_FUNC_DEBUG
    QTextStream stream ( &xmlfile );
    stream << "<?xml version=\"1.0\" encoding = \"iso-8859-1\"?>\n"
    "<!DOCTYPE FUGIT>\n"
    "<FUGIT version='0.3.1' origen=''"
    " date='" << QDate().toString ( Qt::ISODate ) << "'>\n";

    if ( tipo & IMPORT_FORMAS_PAGO ) {
        QString query = "SELECT * FROM forma_pago";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<FORMA_PAGO>\n";
            stream << "\t<IDFORMA_PAGO>" << blXMLEncode ( curc->value( "idforma_pago" ) ) << "</IDFORMA_PAGO>\n";
            stream << "\t<DESCFORMA_PAGO>" << blXMLEncode ( curc->value( "descforma_pago" ) ) << "</DESCFORMA_PAGO>\n";
            stream << "\t<DIAS1TFORMA_PAGO>" << blXMLEncode ( curc->value( "dias1tforma_pago" ) ) << "</DIAS1TFORMA_PAGO>\n";
            stream << "\t<DESCUENTOFORMA_PAGO>" << blXMLEncode ( curc->value( "descuentoforma_pago" ) ) << "</DESCUENTOFORMA_PAGO>\n";
            stream << "</FORMA_PAGO>\n";
            mensajeria ( "<LI>" + _("Exportando nueva forma de pago") + "</LI>\n" );
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    if ( tipo & IMPORT_ALMACENES ) {
        QString query = "SELECT * FROM almacen";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<ALMACEN>\n";
            stream << "\t<IDALMACEN>" << blXMLEncode ( curc->value( "idalmacen" ) ) << "</IDALMACEN>\n";
            stream << "\t<CODIGOALMACEN>" << blXMLEncode ( curc->value( "codigoalmacen" ) ) << "</CODIGOALMACEN>\n";
            stream << "\t<NOMALMACEN>" << blXMLEncode ( curc->value( "nomalmacen" ) ) << "</NOMALMACEN>\n";
            stream << "\t<DIRALMACEN>" << blXMLEncode ( curc->value( "diralmacen" ) ) << "</DIRALMACEN>\n";
            stream << "\t<POBLALMACEN>" << blXMLEncode ( curc->value( "poblalmacen" ) ) << "</POBLALMACEN>\n";
            stream << "\t<CPALMACEN>" << blXMLEncode ( curc->value( "cpalmacen" ) ) << "</CPALMACEN>\n";
            stream << "\t<TELALMACEN>" << blXMLEncode ( curc->value( "telalmacen" ) ) << "</TELALMACEN>\n";
            stream << "\t<FAXALMACEN>" << blXMLEncode ( curc->value( "faxalmacen" ) ) << "</FAXALMACEN>\n";
            stream << "\t<EMAILALMACEN>" << blXMLEncode ( curc->value( "emailalmacen" ) ) << "</EMAILALMACEN>\n";
            stream << "\t<INACTIVOALMACEN>" << blXMLEncode ( curc->value( "inactivoalmacen" ) ) << "</INACTIVOALMACEN>\n";
            stream << "</ALMACEN>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    if ( tipo & IMPORT_TRABAJADORES ) {
        QString query = "SELECT * FROM trabajador";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<TRABAJADOR>\n";
            stream << "\t<IDTRABAJADOR>" << blXMLEncode ( curc->value( "idtrabajador" ) ) << "</IDTRABAJADOR>\n";
            stream << "\t<NOMTRABAJADOR>" << blXMLEncode ( curc->value( "nomtrabajador" ) ) << "</NOMTRABAJADOR>\n";
            stream << "\t<APELLIDOSTRABAJADOR>" << blXMLEncode ( curc->value( "apellidostrabajador" ) ) << "</APELLIDOSTRABAJADOR>\n";
            stream << "\t<DIRTRABAJADOR>" << blXMLEncode ( curc->value( "dirtrabajador" ) ) << "</DIRTRABAJADOR>\n";
            stream << "\t<NSSTRABAJADOR>" << blXMLEncode ( curc->value( "nsstrabajador" ) ) << "</NSSTRABAJADOR>\n";
            stream << "\t<TELTRABAJADOR>" << blXMLEncode ( curc->value( "teltrabajador" ) ) << "</TELTRABAJADOR>\n";
            stream << "\t<MOVILTRABAJADOR>" << blXMLEncode ( curc->value( "moviltrabajador" ) ) << "</MOVILTRABAJADOR>\n";
            stream << "\t<EMAILTRABAJADOR>" << blXMLEncode ( curc->value( "emailtrabajador" ) ) << "</EMAILTRABAJADOR>\n";
            stream << "\t<FOTOTRABAJADOR>" << blXMLEncode ( curc->value( "fototrabajador" ) ) << "</FOTOTRABAJADOR>\n";
            stream << "\t<ACTIVOTRABAJADOR>" << blXMLEncode ( curc->value( "activotrabajador" ) ) << "</ACTIVOTRABAJADOR>\n";
            stream << "</TRABAJADOR>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    if ( tipo & IMPORT_CLIENTES ) {
        QString query = "SELECT * FROM cliente ORDER BY cifcliente";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<CLIENTE>\n";
            stream << "\t<NOMCLIENTE>" << blXMLEncode ( curc->value( "nomcliente" ) ) << "</NOMCLIENTE>\n";
            stream << "\t<NOMALTCLIENTE>" << blXMLEncode ( curc->value( "nomaltcliente" ) ) << "</NOMALTCLIENTE>\n";
            stream << "\t<CIFCLIENTE>" << blXMLEncode ( curc->value( "cifcliente" ) ) << "</CIFCLIENTE>\n";
            stream << "\t<CODCLIENTE>" << blXMLEncode ( curc->value( "codcliente" ) ) << "</CODCLIENTE>\n";
            stream << "\t<BANCOCLIENTE>" << blXMLEncode ( curc->value( "bancocliente" ) ) << "</BANCOCLIENTE>\n";
            stream << "\t<DIRCLIENTE>" << blXMLEncode ( curc->value( "dircliente" ) ) << "</DIRCLIENTE>\n";
            stream << "\t<POBLCLIENTE>" << blXMLEncode ( curc->value( "poblcliente" ) ) << "</POBLCLIENTE>\n";
            stream << "\t<CPCLIENTE>" << blXMLEncode ( curc->value( "cpcliente" ) ) << "</CPCLIENTE>\n";
            stream << "\t<TELCLIENTE>" << blXMLEncode ( curc->value( "telcliente" ) ) << "</TELCLIENTE>\n";
            stream << "\t<TELTRABCLIENTE>" << blXMLEncode ( curc->value( "teltrabcliente" ) ) << "</TELTRABCLIENTE>\n";
            stream << "\t<MOVILCLIENTE>" << blXMLEncode ( curc->value( "movilcliente" ) ) << "</MOVILCLIENTE>\n";
            stream << "\t<FAXCLIENTE>" << blXMLEncode ( curc->value( "faxcliente" ) ) << "</FAXCLIENTE>\n";
            stream << "\t<MAILCLIENTE>" << blXMLEncode ( curc->value( "mailcliente" ) ) << "</MAILCLIENTE>\n";
            stream << "\t<URLCLIENTE>" << blXMLEncode ( curc->value( "urlcliente" ) ) << "</URLCLIENTE>\n";
            stream << "\t<CORPCLIENTE>" << blXMLEncode ( curc->value( "corpcliente" ) ) << "</CORPCLIENTE>\n";
            stream << "\t<FALTACLIENTE>" << blXMLEncode ( curc->value( "faltacliente" ) ) << "</FALTACLIENTE>\n";
            stream << "\t<FBAJACLIENTE>" << blXMLEncode ( curc->value( "fbajacliente" ) ) << "</FBAJACLIENTE>\n";
            stream << "\t<REGIMENFISCALCLIENTE>" << blXMLEncode ( curc->value( "regimenfiscalcliente" ) ) << "</REGIMENFISCALCLIENTE>\n";
            stream << "\t<COMENTCLIENTE>" << blXMLEncode ( curc->value( "comentcliente" ) ) << "</COMENTCLIENTE>\n";
            stream << "\t<ECOMMERCEDATACLIENTE>" << blXMLEncode ( curc->value( "ecommercedatacliente" ) ) << "</ECOMMERCEDATACLIENTE>\n";
            stream << "\t<INACTIVOCLIENTE>" << blXMLEncode ( curc->value( "inactivocliente" ) ) << "</INACTIVOCLIENTE>\n";
            stream << "\t<RECARGOEQCLIENTE>" << blXMLEncode ( curc->value( "recargoeqcliente" ) ) << "</RECARGOEQCLIENTE>\n";
            stream << "\t<IDFORMA_PAGO>" << blXMLEncode ( curc->value( "idforma_pago" ) ) << "</IDFORMA_PAGO>\n";
            stream << "\t<IDPROVINCIA>" << blXMLEncode ( curc->value( "idprovincia" ) ) << "</IDPROVINCIA>\n";
            stream << "</CLIENTE>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    if ( tipo & IMPORT_PROVEEDORES ) {
        QString query = "SELECT * FROM proveedor ORDER BY cifproveedor";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<PROVEEDOR>\n";
            stream << "\t<IDPROVEEDOR>" << blXMLEncode ( curc->value( "idproveedor" ) ) << "</IDPROVEEDOR>\n";
            stream << "\t<NOMPROVEEDOR>" << blXMLEncode ( curc->value( "nomproveedor" ) ) << "</NOMPROVEEDOR>\n";
            stream << "\t<NOMALTPROVEEDOR>" << blXMLEncode ( curc->value( "nomaltproveedor" ) ) << "</NOMALTPROVEEDOR>\n";
            stream << "\t<CIFPROVEEDOR>" << blXMLEncode ( curc->value( "cifproveedor" ) ) << "</CIFPROVEEDOR>\n";
            stream << "\t<CODICLIPROVEEDOR>" << blXMLEncode ( curc->value( "codicliproveedor" ) ) << "</CODICLIPROVEEDOR>\n";
            stream << "\t<CBANCPROVEEDOR>" << blXMLEncode ( curc->value( "cbancproveedor" ) ) << "</CBANCPROVEEDOR>\n";
            stream << "\t<COMENTPROVEEDOR>" << blXMLEncode ( curc->value( "comentproveedor" ) ) << "</COMENTPROVEEDOR>\n";
            stream << "\t<DIRPROVEEDOR>" << blXMLEncode ( curc->value( "dirproveedor" ) ) << "</DIRPROVEEDOR>\n";
            stream << "\t<POBLPROVEEDOR>" << blXMLEncode ( curc->value( "poblproveedor" ) ) << "</POBLPROVEEDOR>\n";
            stream << "\t<CPPROVEEDOR>" << blXMLEncode ( curc->value( "cpproveedor" ) ) << "</CPPROVEEDOR>\n";
            stream << "\t<TELPROVEEDOR>" << blXMLEncode ( curc->value( "telproveedor" ) ) << "</TELPROVEEDOR>\n";
            stream << "\t<FAXPROVEEDOR>" << blXMLEncode ( curc->value( "faxproveedor" ) ) << "</FAXPROVEEDOR>\n";
            stream << "\t<EMAILPROVEEDOR>" << blXMLEncode ( curc->value( "emailproveedor" ) ) << "</EMAILPROVEEDOR>\n";
            stream << "\t<URLPROVEEDOR>" << blXMLEncode ( curc->value( "urlproveedor" ) ) << "</URLPROVEEDOR>\n";
            stream << "\t<CLAVEPROVEEDOR>" << blXMLEncode ( curc->value( "clavewebproveedor" ) ) << "</CLAVEPROVEEDOR>\n";
            stream << "\t<INACTIVOPROVEEDOR>" << blXMLEncode ( curc->value( "inactivoproveedor" ) ) << "</INACTIVOPROVEEDOR>\n";
            stream << "\t<PROVPROVEEDOR>" << blXMLEncode ( curc->value( "idprovincia" ) ) << "</PROVPROVEEDOR>\n";
            stream << "</PROVEEDOR>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    if ( tipo & IMPORT_FAMILIAS ) {
        QString query = "SELECT * FROM familia ORDER BY codigocompletofamilia ";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<FAMILIA>\n";
            stream << "\t<IDFAMILIA>" << blXMLEncode ( curc->value( "idfamilia" ) ) << "</IDFAMILIA>\n";
            stream << "\t<CODIGOFAMILIA>" << blXMLEncode ( curc->value( "codigofamilia" ) ) << "</CODIGOFAMILIA>\n";
            stream << "\t<NOMBREFAMILIA>" << blXMLEncode ( curc->value( "nombrefamilia" ) ) << "</NOMBREFAMILIA>\n";
            stream << "\t<DESCFAMILIA>" << blXMLEncode ( curc->value( "descfamilia" ) ) << "</DESCFAMILIA>\n";
            stream << "\t<PADREFAMILIA>" << blXMLEncode ( curc->value( "padrefamilia" ) ) << "</PADREFAMILIA>\n";
            stream << "\t<CODIGOCOMPLETOFAMILIA>" << blXMLEncode ( curc->value( "codigocompletofamilia" ) ) << "</CODIGOCOMPLETOFAMILIA>\n";
            stream << "</FAMILIA>\n";
            curc->nextRecord();
        }// end while
        delete curc;
    } // end if

    if ( tipo & IMPORT_ARTICULOS ) {
        QString query = "SELECT * FROM articulo LEFT JOIN familia ON familia.idfamilia = articulo.idfamilia ";
        query += " LEFT JOIN tipo_articulo ON articulo.idtipo_articulo = tipo_articulo.idtipo_articulo ";
        query += " LEFT JOIN tipo_iva ON articulo.idtipo_iva = tipo_iva.idtipo_iva ";
        query += " ORDER BY codigocompletoarticulo ";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<ARTICULO>\n";
            stream << "\t<IDARTICULO>" << blXMLEncode ( curc->value( "idarticulo" ) ) << "</IDARTICULO>\n";
            stream << "\t<CODARTICULO>" << blXMLEncode ( curc->value( "codarticulo" ) ) << "</CODARTICULO>\n";
            stream << "\t<NOMARTICULO>" << blXMLEncode ( curc->value( "nomarticulo" ) ) << "</NOMARTICULO>\n";
            stream << "\t<ABREVARTICULO>" << blXMLEncode ( curc->value( "abrevarticulo" ) ) << "</ABREVARTICULO>\n";
            stream << "\t<OBSERARTICULO>" << blXMLEncode ( curc->value( "obserarticulo" ) ) << "</OBSERARTICULO>\n";
            stream << "\t<PRESENTABLEARTICULO>" << blXMLEncode ( curc->value( "presentablearticulo" ) ) << "</PRESENTABLEARTICULO>\n";
            stream << "\t<CONTROLSTOCKARTICULO>" << blXMLEncode ( curc->value( "controlstockarticulo" ) ) << "</CONTROLSTOCKARTICULO>\n";
            stream << "\t<IDTIPO_ARTICULO>" << blXMLEncode ( curc->value( "idtipo_articulo" ) ) << "</IDTIPO_ARTICULO>\n";
            stream << "\t<IDTIPO_IVA>" << blXMLEncode ( curc->value( "idtipo_iva" ) ) << "</IDTIPO_IVA>\n";
            stream << "\t<CODIGOCOMPLETOARTICULO>" << blXMLEncode ( curc->value( "codigocompletoarticulo" ) ) << "</CODIGOCOMPLETOARTICULO>\n";
            stream << "\t<IDFAMILIA>" << blXMLEncode ( curc->value( "idfamilia" ) ) << "</IDFAMILIA>\n";
            stream << "\t<STOCKARTICULO>" << blXMLEncode ( curc->value( "stockarticulo" ) ) << "</STOCKARTICULO>\n";
            stream << "\t<INACTIVOARTICULO>" << blXMLEncode ( curc->value( "inactivoarticulo" ) ) << "</INACTIVOARTICULO>\n";
            stream << "\t<PVPARTICULO>" << blXMLEncode ( curc->value( "pvparticulo" ) ) << "</PVPARTICULO>\n";
            /// Campos adicionales a los articulos.
            stream << "\t<CODIGOCOMPLETOFAMILIA>" << blXMLEncode ( curc->value( "codigocompletofamilia" ) )   << "</CODIGOCOMPLETOFAMILIA>\n";
            stream << "\t<NOMBREFAMILIA>" << blXMLEncode ( curc->value( "nombrefamilia" ) ) << "</NOMBREFAMILIA>\n";
            stream << "\t<CODTIPO_ARTICULO>" << blXMLEncode ( curc->value( "codtipo_articulo" ) ) << "</CODTIPO_ARTICULO>\n";
            stream << "\t<DESCTIPO_ARTICULO>" << blXMLEncode ( curc->value( "desctipo_articulo" ) ) << "</DESCTIPO_ARTICULO>\n";
            stream << "\t<DESCTIPO_IVA>" << blXMLEncode ( curc->value( "desctipo_iva" ) ) << "</DESCTIPO_IVA>\n";
            stream << "</ARTICULO>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    /// Hacemos la exportacion de facturas de clientes.
    if ( tipo & IMPORT_FACTURASCLIENTE ) {
        QString query = "SELECT * FROM factura ";
        query += " LEFT JOIN almacen ON factura.idalmacen = almacen.idalmacen ";
        query += " LEFT JOIN cliente ON cliente.idcliente = factura.idcliente ";
        query += " LEFT JOIN trabajador ON trabajador.idtrabajador = factura.idtrabajador ";
        query += " LEFT JOIN forma_pago ON forma_pago.idforma_pago = factura.idforma_pago ";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<FACTURA>\n";
            stream << "\t<IDFACTURA>" << blXMLEncode ( curc->value( "idfactura" ) ) << "</IDFACTURA>\n";
            stream << "\t<CODIGOSERIE_FACTURA>" << blXMLEncode ( curc->value( "codigoserie_factura" ) ) << "</CODIGOSERIE_FACTURA>\n";
            stream << "\t<NUMFACTURA>" << blXMLEncode ( curc->value( "numfactura" ) ) << "</NUMFACTURA>\n";
            stream << "\t<REFFACTURA>" << blXMLEncode ( curc->value( "reffactura" ) ) << "</REFFACTURA>\n";
            stream << "\t<FFACTURA>" << blXMLEncode ( curc->value( "ffactura" ) ) << "</FFACTURA>\n";
            stream << "\t<DESCFACTURA>" << blXMLEncode ( curc->value( "descfactura" ) ) << "</DESCFACTURA>\n";
            stream << "\t<IDALMACEN>" << blXMLEncode ( curc->value( "idalmacen" ) ) << "</IDALMACEN>\n";
            stream << "\t<CONTACTFACTURA>" << blXMLEncode ( curc->value( "contactfactura" ) ) << "</CONTACTFACTURA>\n";
            stream << "\t<TELFACTURA>" << blXMLEncode ( curc->value( "telfactura" ) ) << "</TELFACTURA>\n";
            stream << "\t<COMENTFACTURA>" << blXMLEncode ( curc->value( "comentfactura" ) ) << "</COMENTFACTURA>\n";
            stream << "\t<PROCESADAFACTURA>" << blXMLEncode ( curc->value( "procesadafactura" ) ) << "</PROCESADAFACTURA>\n";
            stream << "\t<IDUSUARI>" << blXMLEncode ( curc->value( "idusuari" ) ) << "</IDUSUARI>\n";
            stream << "\t<IDCLIENTE>" << blXMLEncode ( curc->value( "idcliente" ) ) << "</IDCLIENTE>\n";
            stream << "\t<IDFORMA_PAGO>" << blXMLEncode ( curc->value( "idforma_pago" ) ) << "</IDFORMA_PAGO>\n";
            stream << "\t<IDTRABAJADOR>" << blXMLEncode ( curc->value( "idtrabajador" ) ) << "</IDTRABAJADOR>\n";
            /// Datos iniciales para la factura que pueden ser de utilidad.
            stream << "\t<CODIGOALMACEN>" << blXMLEncode ( curc->value( "codigoalmacen" ) ) << "</CODIGOALMACEN>\n";
            stream << "\t<NOMALMACEN>" << blXMLEncode ( curc->value( "nomalmacen" ) ) << "</NOMALMACEN>\n";
            stream << "\t<DIRALMACEN>" << blXMLEncode ( curc->value( "diralmacen" ) ) << "</DIRALMACEN>\n";
            stream << "\t<POBLALMACEN>" << blXMLEncode ( curc->value( "poblalmacen" ) ) << "</POBLALMACEN>\n";
            stream << "\t<CPALMACEN>" << blXMLEncode ( curc->value( "cpalmacen" ) ) << "</CPALMACEN>\n";
            stream << "\t<TELALMACEN>" << blXMLEncode ( curc->value( "telalmacen" ) ) << "</TELALMACEN>\n";
            /// Datos referentes al cliente.
            stream << "\t<NOMCLIENTE>" << blXMLEncode ( curc->value( "nomcliente" ) ) << "</NOMCLIENTE>\n";
            stream << "\t<NOMALTCLIENTE>" << blXMLEncode ( curc->value( "nomaltcliente" ) ) << "</NOMALTCLIENTE>\n";
            stream << "\t<CIFCLIENTE>" << blXMLEncode ( curc->value( "cifcliente" ) ) << "</CIFCLIENTE>\n";
            stream << "\t<BANCOCLIENTE>" << blXMLEncode ( curc->value( "bancocliente" ) ) << "</BANCOCLIENTE>\n";
            stream << "\t<DIRCLIENTE>" << blXMLEncode ( curc->value( "dircliente" ) ) << "</DIRCLIENTE>\n";
            stream << "\t<POBLCLIENTE>" << blXMLEncode ( curc->value( "poblcliente" ) ) << "</POBLCLIENTE>\n";
            stream << "\t<CPCLIENTE>" << blXMLEncode ( curc->value( "cpcliente" ) ) << "</CPCLIENTE>\n";
            stream << "\t<TELCLIENTE>" << blXMLEncode ( curc->value( "telcliente" ) ) << "</TELCLIENTE>\n";
            stream << "\t<FAXCLIENTE>" << blXMLEncode ( curc->value( "faxcliente" ) ) << "</FAXCLIENTE>\n";
            stream << "\t<MAILCLIENTE>" << blXMLEncode ( curc->value( "mailcliente" ) ) << "</MAILCLIENTE>\n";
            stream << "\t<URLCLIENTE>" << blXMLEncode ( curc->value( "urlcliente" ) ) << "</URLCLIENTE>\n";
            stream << "\t<PROVCLIENTE>" << blXMLEncode ( curc->value( "provcliente" ) ) << "</PROVCLIENTE>\n";
            /// Datos sobre el trabajador
            stream << "\t<NOMTRABAJADOR>" << blXMLEncode ( curc->value( "nomtrabajador" ) ) << "</NOMTRABAJADOR>\n";
            stream << "\t<APELLIDOSTRABAJADOR>" << blXMLEncode ( curc->value( "apellidostrabajador" ) ) << "</APELLIDOSTRABAJADOR>\n";
            stream << "\t<DIRTRABAJADOR>" << blXMLEncode ( curc->value( "dirtrabajador" ) ) << "</DIRTRABAJADOR>\n";
            stream << "\t<NSSTRABAJADOR>" << blXMLEncode ( curc->value( "nsstrabajador" ) ) << "</NSSTRABAJADOR>\n";
            stream << "\t<TELTRABAJADOR>" << blXMLEncode ( curc->value( "teltrabajador" ) ) << "</TELTRABAJADOR>\n";
            stream << "\t<MOVILTRABAJADOR>" << blXMLEncode ( curc->value( "moviltrabajador" ) ) << "</MOVILTRABAJADOR>\n";
            stream << "\t<EMAILTRABAJADOR>" << blXMLEncode ( curc->value( "emailtrabajador" ) ) << "</EMAILTRABAJADOR>\n";
            /// Datos sobre la forma de pago
            stream << "\t<DESCFORMA_PAGO>" << blXMLEncode ( curc->value( "descforma_pago" ) ) << "</DESCFORMA_PAGO>\n";
            stream << "\t<DIAS1TFORMA_PAGO>" << blXMLEncode ( curc->value( "dias1tforma_pago" ) ) << "</DIAS1TFORMA_PAGO>\n";
            stream << "\t<DESCUENTOFORMA_PAGO>" << blXMLEncode ( curc->value( "descuentoforma_pago" ) ) << "</DESCUENTOFORMA_PAGO>\n";

            /// Incorporamos las lineas de detalles de la factura.
            BlDbRecordSet *curlc = dbConnection->loadQuery ( "SELECT * FROM lfactura LEFT JOIN articulo ON lfactura.idarticulo = articulo.idarticulo WHERE idfactura = " + curc->value( "idfactura" ) );
            while ( !curlc->eof() ) {
                stream << "\t\t<LFACTURA>\n";
                stream << "\t\t\t<IDLFACTURA>" << blXMLEncode ( curlc->value( "idlfactura" ) ) << "</IDLFACTURA>\n";
                stream << "\t\t\t<DESCLFACTURA>" << blXMLEncode ( curlc->value( "desclfactura" ) ) << "</DESCLFACTURA>\n";
                stream << "\t\t\t<CANTLFACTURA>" << blXMLEncode ( curlc->value( "cantlfactura" ) ) << "</CANTLFACTURA>\n";
                stream << "\t\t\t<PVPLFACTURA>" << blXMLEncode ( curlc->value( "pvplfactura" ) ) << "</PVPLFACTURA>\n";
                stream << "\t\t\t<IVALFACTURA>" << blXMLEncode ( curlc->value( "ivalfactura" ) ) << "</IVALFACTURA>\n";
                stream << "\t\t\t<DESCUENTOLFACTURA>" << blXMLEncode ( curlc->value( "descuentolfactura" ) ) << "</DESCUENTOLFACTURA>\n";
                stream << "\t\t\t<IDFACTURA>" << blXMLEncode ( curlc->value( "idfactura" ) ) << "</IDFACTURA>\n";
                stream << "\t\t\t<IDARTICULO>" << blXMLEncode ( curlc->value( "idarticulo" ) ) << "</IDARTICULO>\n";
                /// Los datos relacionados con el articulo
                stream << "\t\t\t<CODARTICULO>" << blXMLEncode ( curlc->value( "codarticulo" ) ) << "</CODARTICULO>\n";
                stream << "\t\t\t<NOMARTICULO>" << blXMLEncode ( curlc->value( "nomarticulo" ) ) << "</NOMARTICULO>\n";
                stream << "\t\t\t<ABREVARTICULO>" << blXMLEncode ( curlc->value( "abrevarticulo" ) ) << "</ABREVARTICULO>\n";
                stream << "\t<CODIGOCOMPLETOARTICULO>" << blXMLEncode ( curlc->value( "codigocompletoarticulo" ) ) << "</CODIGOCOMPLETOARTICULO>\n";
                stream << "\t\t</LFACTURA>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            /// Incorporamos los descuentos de la factura.
            curlc = dbConnection->loadQuery ( "SELECT * FROM dfactura WHERE idfactura = " + curc->value( "idfactura" ) );
            while ( !curlc->eof() ) {
                stream << "\t\t<DFACTURA>\n";
                stream << "\t\t\t<IDDFACTURA>" << blXMLEncode ( curlc->value( "iddfactura" ) ) << "</IDDFACTURA>\n";
                stream << "\t\t\t<CONCEPTDFACTURA>" << blXMLEncode ( curlc->value( "conceptdfactura" ) ) << "</CONCEPTDFACTURA>\n";
                stream << "\t\t\t<PROPORCIONDFACTURA>" << blXMLEncode ( curlc->value( "proporciondfactura" ) ) << "</PROPORCIONDFACTURA>\n";
                stream << "\t\t\t<IDFACTURA>" << blXMLEncode ( curlc->value( "idfactura" ) ) << "</IDFACTURA>\n";
                stream << "\t\t</DFACTURA>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            stream << "</FACTURA>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    /// Hacemos la exportacion de facturas de clientes.
    if ( tipo & IMPORT_PRESUPUESTOSCLIENTE ) {
        QString query = "SELECT * FROM presupuesto ";
        query += " LEFT JOIN almacen ON presupuesto.idalmacen = almacen.idalmacen ";
        query += " LEFT JOIN cliente ON cliente.idcliente = presupuesto.idcliente ";
        query += " LEFT JOIN trabajador ON trabajador.idtrabajador = presupuesto.idtrabajador ";
        query += " LEFT JOIN forma_pago ON forma_pago.idforma_pago = presupuesto.idforma_pago ";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<PRESUPUESTO>\n";
            stream << "\t<IDPRESUPUESTO>" << blXMLEncode ( curc->value( "idpresupuesto" ) ) << "</IDPRESUPUESTO>\n";
            stream << "\t<NUMPRESUPUESTO>" << blXMLEncode ( curc->value( "numpresupuesto" ) ) << "</NUMPRESUPUESTO>\n";
            stream << "\t<REFPRESUPUESTO>" << blXMLEncode ( curc->value( "refpresupuesto" ) ) << "</REFPRESUPUESTO>\n";
            stream << "\t<FPRESUPUESTO>" << blXMLEncode ( curc->value( "fpresupuesto" ) ) << "</FPRESUPUESTO>\n";
            stream << "\t<DESCPRESUPUESTO>" << blXMLEncode ( curc->value( "descpresupuesto" ) ) << "</DESCPRESUPUESTO>\n";
            stream << "\t<CONTACTPRESUPUESTO>" << blXMLEncode ( curc->value( "contactpresupuesto" ) ) << "</CONTACTPRESUPUESTO>\n";
            stream << "\t<TELPRESUPUESTO>" << blXMLEncode ( curc->value( "telpresupuesto" ) ) << "</TELPRESUPUESTO>\n";
            stream << "\t<VENCPRESUPUESTO>" << blXMLEncode ( curc->value( "vencpresupuesto" ) ) << "</VENCPRESUPUESTO>\n";
            stream << "\t<COMENTPRESUPUESTO>" << blXMLEncode ( curc->value( "comentpresupuesto" ) ) << "</COMENTPRESUPUESTO>\n";
            stream << "\t<IDUSUARI>" << blXMLEncode ( curc->value( "idusuari" ) ) << "</IDUSUARI>\n";
            stream << "\t<PROCESADOPRESUPUESTO>" << blXMLEncode ( curc->value( "procesadopresupuesto" ) ) << "</PROCESADOPRESUPUESTO>\n";
            stream << "\t<IDCLIENTE>" << blXMLEncode ( curc->value( "idcliente" ) ) << "</IDCLIENTE>\n";
            stream << "\t<IDALMACEN>" << blXMLEncode ( curc->value( "idalmacen" ) ) << "</IDALMACEN>\n";
            stream << "\t<IDFORMA_PAGO>" << blXMLEncode ( curc->value( "idforma_pago" ) ) << "</IDFORMA_PAGO>\n";
            stream << "\t<IDTRABAJADOR>" << blXMLEncode ( curc->value( "idtrabajador" ) ) << "</IDTRABAJADOR>\n";
            /// Datos iniciales para el presupuesto  que pueden ser de utilidad.
            stream << "\t<CODIGOALMACEN>" << blXMLEncode ( curc->value( "codigoalmacen" ) ) << "</CODIGOALMACEN>\n";
            stream << "\t<NOMALMACEN>" << blXMLEncode ( curc->value( "nomalmacen" ) ) << "</NOMALMACEN>\n";
            stream << "\t<DIRALMACEN>" << blXMLEncode ( curc->value( "diralmacen" ) ) << "</DIRALMACEN>\n";
            stream << "\t<POBLALMACEN>" << blXMLEncode ( curc->value( "poblalmacen" ) ) << "</POBLALMACEN>\n";
            stream << "\t<CPALMACEN>" << blXMLEncode ( curc->value( "cpalmacen" ) ) << "</CPALMACEN>\n";
            stream << "\t<TELALMACEN>" << blXMLEncode ( curc->value( "telalmacen" ) ) << "</TELALMACEN>\n";
            /// Datos referentes al cliente.
            stream << "\t<NOMCLIENTE>" << blXMLEncode ( curc->value( "nomcliente" ) ) << "</NOMCLIENTE>\n";
            stream << "\t<NOMALTCLIENTE>" << blXMLEncode ( curc->value( "nomaltcliente" ) ) << "</NOMALTCLIENTE>\n";
            stream << "\t<CIFCLIENTE>" << blXMLEncode ( curc->value( "cifcliente" ) ) << "</CIFCLIENTE>\n";
            stream << "\t<BANCOCLIENTE>" << blXMLEncode ( curc->value( "bancocliente" ) ) << "</BANCOCLIENTE>\n";
            stream << "\t<DIRCLIENTE>" << blXMLEncode ( curc->value( "dircliente" ) ) << "</DIRCLIENTE>\n";
            stream << "\t<POBLCLIENTE>" << blXMLEncode ( curc->value( "poblcliente" ) ) << "</POBLCLIENTE>\n";
            stream << "\t<CPCLIENTE>" << blXMLEncode ( curc->value( "cpcliente" ) ) << "</CPCLIENTE>\n";
            stream << "\t<TELCLIENTE>" << blXMLEncode ( curc->value( "telcliente" ) ) << "</TELCLIENTE>\n";
            stream << "\t<FAXCLIENTE>" << blXMLEncode ( curc->value( "faxcliente" ) ) << "</FAXCLIENTE>\n";
            stream << "\t<MAILCLIENTE>" << blXMLEncode ( curc->value( "mailcliente" ) ) << "</MAILCLIENTE>\n";
            stream << "\t<URLCLIENTE>" << blXMLEncode ( curc->value( "urlcliente" ) ) << "</URLCLIENTE>\n";
            stream << "\t<PROVCLIENTE>" << blXMLEncode ( curc->value( "provcliente" ) ) << "</PROVCLIENTE>\n";
            /// Datos sobre el trabajador.
            stream << "\t<NOMTRABAJADOR>" << blXMLEncode ( curc->value( "nomtrabajador" ) ) << "</NOMTRABAJADOR>\n";
            stream << "\t<APELLIDOSTRABAJADOR>" << blXMLEncode ( curc->value( "apellidostrabajador" ) ) << "</APELLIDOSTRABAJADOR>\n";
            stream << "\t<DIRTRABAJADOR>" << blXMLEncode ( curc->value( "dirtrabajador" ) ) << "</DIRTRABAJADOR>\n";
            stream << "\t<NSSTRABAJADOR>" << blXMLEncode ( curc->value( "nsstrabajador" ) ) << "</NSSTRABAJADOR>\n";
            stream << "\t<TELTRABAJADOR>" << blXMLEncode ( curc->value( "teltrabajador" ) ) << "</TELTRABAJADOR>\n";
            stream << "\t<MOVILTRABAJADOR>" << blXMLEncode ( curc->value( "moviltrabajador" ) ) << "</MOVILTRABAJADOR>\n";
            stream << "\t<EMAILTRABAJADOR>" << blXMLEncode ( curc->value( "emailtrabajador" ) ) << "</EMAILTRABAJADOR>\n";
            /// Datos sobre la forma de pago.
            stream << "\t<DESCFORMA_PAGO>" << blXMLEncode ( curc->value( "descforma_pago" ) ) << "</DESCFORMA_PAGO>\n";
            stream << "\t<DIAS1TFORMA_PAGO>" << blXMLEncode ( curc->value( "dias1tforma_pago" ) ) << "</DIAS1TFORMA_PAGO>\n";
            stream << "\t<DESCUENTOFORMA_PAGO>" << blXMLEncode ( curc->value( "descuentoforma_pago" ) ) << "</DESCUENTOFORMA_PAGO>\n";
            /// Incorporamos las lineas de detalles del presupuesto.
            BlDbRecordSet *curlc = dbConnection->loadQuery ( "SELECT * FROM lpresupuesto LEFT JOIN articulo ON lpresupuesto.idarticulo = articulo.idarticulo WHERE idpresupuesto = " + curc->value( "idpresupuesto" ) );
            while ( !curlc->eof() ) {
                stream << "\t<LPRESUPUESTO>\n";
                stream << "\t\t<IDLPRESUPUESTO>" << blXMLEncode ( curlc->value( "idlpresupuesto" ) ) << "</IDLPRESUPUESTO>\n";
                stream << "\t\t\t<DESCLPRESUPUESTO>" << blXMLEncode ( curlc->value( "desclpresupuesto" ) ) << "</DESCLPRESUPUESTO>\n";
                stream << "\t\t<CANTLPRESUPUESTO>" << blXMLEncode ( curlc->value( "cantlpresupuesto" ) ) << "</CANTLPRESUPUESTO>\n";
                stream << "\t\t<PVPLPRESUPUESTO>" << blXMLEncode ( curlc->value( "pvplpresupuesto" ) ) << "</PVPLPRESUPUESTO>\n";
                stream << "\t\t<IVALPRESUPUESTO>" << blXMLEncode ( curlc->value( "ivalpresupuesto" ) ) << "</IVALPRESUPUESTO>\n";
                stream << "\t\t<DESCUENTOLPRESUPUESTO>" << blXMLEncode ( curlc->value( "descuentolpresupuesto" ) ) << "</DESCUENTOLPRESUPUESTO>\n";
                stream << "\t\t<IDPRESUPUESTO>" << blXMLEncode ( curlc->value( "idpresupuesto" ) ) << "</IDPRESUPUESTO>\n";
                stream << "\t\t<IDARTICULO>" << blXMLEncode ( curlc->value( "idarticulo" ) ) << "</IDARTICULO>\n";
                /// Los datos relacionados con el articulo
                stream << "\t\t<CODARTICULO>" << blXMLEncode ( curlc->value( "codarticulo" ) ) << "</CODARTICULO>\n";
                stream << "\t\t<NOMARTICULO>" << blXMLEncode ( curlc->value( "nomarticulo" ) ) << "</NOMARTICULO>\n";
                stream << "\t\t<ABREVARTICULO>" << blXMLEncode ( curlc->value( "abrevarticulo" ) ) << "</ABREVARTICULO>\n";
                stream << "\t\t<CODIGOCOMPLETOARTICULO>" << blXMLEncode ( curlc->value( "codigocompletoarticulo" ) ) << "</CODIGOCOMPLETOARTICULO>\n";
                stream << "\t</LPRESUPUESTO>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            /// Incorporamos los descuentos del presupuesto.
            curlc = dbConnection->loadQuery ( "SELECT * FROM dpresupuesto WHERE idpresupuesto = " + curc->value( "idpresupuesto" ) );
            while ( !curlc->eof() ) {
                stream << "\t<DPRESUPUESTO>\n";
                stream << "\t\t<IDDPRESUPUESTO>" << blXMLEncode ( curlc->value( "iddpresupuesto" ) ) << "</IDDPRESUPUESTO>\n";
                stream << "\t\t<CONCEPTDPRESUPUESTO>" << blXMLEncode ( curlc->value( "conceptdpresupuesto" ) ) << "</CONCEPTDPRESUPUESTO>\n";
                stream << "\t\t<PROPORCIONDPRESUPUESTO>" << blXMLEncode ( curlc->value( "proporciondpresupuesto" ) ) << "</PROPORCIONDPRESUPUESTO>\n";
                stream << "\t\t<IDPRESUPUESTO>" << blXMLEncode ( curlc->value( "idpresupuesto" ) ) << "</IDPRESUPUESTO>\n";
                stream << "\t</DPRESUPUESTO>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            stream << "</PRESUPUESTO>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    /// Hacemos la exportacion de facturas de clientes.
    if ( tipo & IMPORT_PEDIDOSCLIENTE ) {
        QString query = "SELECT * FROM pedidocliente ";
        query += " LEFT JOIN almacen ON pedidocliente.idalmacen = almacen.idalmacen ";
        query += " LEFT JOIN cliente ON cliente.idcliente = pedidocliente.idcliente ";
        query += " LEFT JOIN trabajador ON trabajador.idtrabajador = pedidocliente.idtrabajador ";
        query += " LEFT JOIN forma_pago ON forma_pago.idforma_pago = pedidocliente.idforma_pago ";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<PEDIDOCLIENTE>\n";
            stream << "\t<IDPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "idpedidocliente" ) ) << "</IDPEDIDOCLIENTE>\n";
            stream << "\t<NUMPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "numpedidocliente" ) ) << "</NUMPEDIDOCLIENTE>\n";
            stream << "\t<FECHAPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "fechapedidocliente" ) ) << "</FECHAPEDIDOCLIENTE>\n";
            stream << "\t<REFPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "refpedidocliente" ) ) << "</REFPEDIDOCLIENTE>\n";
            stream << "\t<DESCPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "descpedidocliente" ) ) << "</DESCPEDIDOCLIENTE>\n";
            stream << "\t<CONTACTPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "contactpedidocliente" ) ) << "</CONTACTPEDIDOCLIENTE>\n";
            stream << "\t<TELPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "telpedidocliente" ) ) << "</TELPEDIDOCLIENTE>\n";
            stream << "\t<IDPRESUPUESTO>" << blXMLEncode ( curc->value( "idpresupuesto" ) ) << "</IDPRESUPUESTO>\n";
            stream << "\t<COMENTPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "comentpedidocliente" ) ) << "</COMENTPEDIDOCLIENTE>\n";
            stream << "\t<IDUSUARI>" << blXMLEncode ( curc->value( "idusuari" ) ) << "</IDUSUARI>\n";
            stream << "\t<PROCESADOPEDIDOCLIENTE>" << blXMLEncode ( curc->value( "procesadopedidocliente" ) ) << "</PROCESADOPEDIDOCLIENTE>\n";
            stream << "\t<IDCLIENTE>" << blXMLEncode ( curc->value( "idcliente" ) ) << "</IDCLIENTE>\n";
            stream << "\t<IDALMACEN>" << blXMLEncode ( curc->value( "idalmacen" ) ) << "</IDALMACEN>\n";
            stream << "\t<IDFORMA_PAGO>" << blXMLEncode ( curc->value( "idforma_pago" ) ) << "</IDFORMA_PAGO>\n";
            stream << "\t<IDTRABAJADOR>" << blXMLEncode ( curc->value( "idtrabajador" ) ) << "</IDTRABAJADOR>\n";
            /// Datos iniciales para el presupuesto  que pueden ser de utilidad.
            stream << "\t<CODIGOALMACEN>" << blXMLEncode ( curc->value( "codigoalmacen" ) ) << "</CODIGOALMACEN>\n";
            stream << "\t<NOMALMACEN>" << blXMLEncode ( curc->value( "nomalmacen" ) ) << "</NOMALMACEN>\n";
            stream << "\t<DIRALMACEN>" << blXMLEncode ( curc->value( "diralmacen" ) ) << "</DIRALMACEN>\n";
            stream << "\t<POBLALMACEN>" << blXMLEncode ( curc->value( "poblalmacen" ) ) << "</POBLALMACEN>\n";
            stream << "\t<CPALMACEN>" << blXMLEncode ( curc->value( "cpalmacen" ) ) << "</CPALMACEN>\n";
            stream << "\t<TELALMACEN>" << blXMLEncode ( curc->value( "telalmacen" ) ) << "</TELALMACEN>\n";
            /// Datos referentes al cliente.
            stream << "\t<NOMCLIENTE>" << blXMLEncode ( curc->value( "nomcliente" ) ) << "</NOMCLIENTE>\n";
            stream << "\t<NOMALTCLIENTE>" << blXMLEncode ( curc->value( "nomaltcliente" ) ) << "</NOMALTCLIENTE>\n";
            stream << "\t<CIFCLIENTE>" << blXMLEncode ( curc->value( "cifcliente" ) ) << "</CIFCLIENTE>\n";
            stream << "\t<BANCOCLIENTE>" << blXMLEncode ( curc->value( "bancocliente" ) ) << "</BANCOCLIENTE>\n";
            stream << "\t<DIRCLIENTE>" << blXMLEncode ( curc->value( "dircliente" ) ) << "</DIRCLIENTE>\n";
            stream << "\t<POBLCLIENTE>" << blXMLEncode ( curc->value( "poblcliente" ) ) << "</POBLCLIENTE>\n";
            stream << "\t<CPCLIENTE>" << blXMLEncode ( curc->value( "cpcliente" ) ) << "</CPCLIENTE>\n";
            stream << "\t<TELCLIENTE>" << blXMLEncode ( curc->value( "telcliente" ) ) << "</TELCLIENTE>\n";
            stream << "\t<FAXCLIENTE>" << blXMLEncode ( curc->value( "faxcliente" ) ) << "</FAXCLIENTE>\n";
            stream << "\t<MAILCLIENTE>" << blXMLEncode ( curc->value( "mailcliente" ) ) << "</MAILCLIENTE>\n";
            stream << "\t<URLCLIENTE>" << blXMLEncode ( curc->value( "urlcliente" ) ) << "</URLCLIENTE>\n";
            stream << "\t<PROVCLIENTE>" << blXMLEncode ( curc->value( "provcliente" ) ) << "</PROVCLIENTE>\n";
            /// Datos sobre el trabajador
            stream << "\t<NOMTRABAJADOR>" << blXMLEncode ( curc->value( "nomtrabajador" ) ) << "</NOMTRABAJADOR>\n";
            stream << "\t<APELLIDOSTRABAJADOR>" << blXMLEncode ( curc->value( "apellidostrabajador" ) ) << "</APELLIDOSTRABAJADOR>\n";
            stream << "\t<DIRTRABAJADOR>" << blXMLEncode ( curc->value( "dirtrabajador" ) ) << "</DIRTRABAJADOR>\n";
            stream << "\t<NSSTRABAJADOR>" << blXMLEncode ( curc->value( "nsstrabajador" ) ) << "</NSSTRABAJADOR>\n";
            stream << "\t<TELTRABAJADOR>" << blXMLEncode ( curc->value( "teltrabajador" ) ) << "</TELTRABAJADOR>\n";
            stream << "\t<MOVILTRABAJADOR>" << blXMLEncode ( curc->value( "moviltrabajador" ) ) << "</MOVILTRABAJADOR>\n";
            stream << "\t<EMAILTRABAJADOR>" << blXMLEncode ( curc->value( "emailtrabajador" ) ) << "</EMAILTRABAJADOR>\n";
            /// Datos sobre la forma de pago
            stream << "\t<DESCFORMA_PAGO>" << blXMLEncode ( curc->value( "descforma_pago" ) ) << "</DESCFORMA_PAGO>\n";
            stream << "\t<DIAS1TFORMA_PAGO>" << blXMLEncode ( curc->value( "dias1tforma_pago" ) ) << "</DIAS1TFORMA_PAGO>\n";
            stream << "\t<DESCUENTOFORMA_PAGO>" << blXMLEncode ( curc->value( "descuentoforma_pago" ) ) << "</DESCUENTOFORMA_PAGO>\n";
            /// Incorporamos las lineas de detalles del presupuesto.
            BlDbRecordSet *curlc = dbConnection->loadQuery ( "SELECT * FROM lpedidocliente LEFT JOIN articulo ON lpedidocliente.idarticulo = articulo.idarticulo WHERE idpedidocliente = " + curc->value( "idpedidocliente" ) );
            while ( !curlc->eof() ) {
                stream << "\t\t<LPEDIDOCLIENTE>\n";
                stream << "\t\t\t<IDLPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "idlpedidocliente" ) ) << "</IDLPEDIDOCLIENTE>\n";
                stream << "\t\t\t<DESCLPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "desclpedidocliente" ) ) << "</DESCLPEDIDOCLIENTE>\n";
                stream << "\t\t\t<CANTLPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "cantlpedidocliente" ) ) << "</CANTLPEDIDOCLIENTE>\n";
                stream << "\t\t\t<PVPLPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "pvplpedidocliente" ) ) << "</PVPLPEDIDOCLIENTE>\n";
                stream << "\t\t\t<IVALPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "ivalpedidocliente" ) ) << "</IVALPEDIDOCLIENTE>\n";
                stream << "\t\t\t<DESCUENTOLPEDIDOCLIENTEO>" << blXMLEncode ( curlc->value( "descuentolpedidocliente" ) ) << "</DESCUENTOLPEDIDOCLIENTEO>\n";
                stream << "\t\t\t<IDPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "idpresupuesto" ) ) << "</IDPEDIDOCLIENTE>\n";
                stream << "\t\t\t<IDARTICULO>" << blXMLEncode ( curlc->value( "idarticulo" ) ) << "</IDARTICULO>\n";
                /// Los datos relacionados con el articulo.
                stream << "\t\t\t<CODARTICULO>" << blXMLEncode ( curlc->value( "codarticulo" ) ) << "</CODARTICULO>\n";
                stream << "\t\t\t<NOMARTICULO>" << blXMLEncode ( curlc->value( "nomarticulo" ) ) << "</NOMARTICULO>\n";
                stream << "\t\t\t<ABREVARTICULO>" << blXMLEncode ( curlc->value( "abrevarticulo" ) ) << "</ABREVARTICULO>\n";
                stream << "\t<CODIGOCOMPLETOARTICULO>" << blXMLEncode ( curlc->value( "codigocompletoarticulo" ) ) << "</CODIGOCOMPLETOARTICULO>\n";
                stream << "\t\t</LPRESUPUESTO>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            /// Incorporamos los descuentos del presupuesto.
            curlc = dbConnection->loadQuery ( "SELECT * FROM dpedidocliente WHERE idpedidocliente = " + curc->value( "idpedidocliente" ) );
            while ( !curlc->eof() ) {
                stream << "\t\t<DPEDIDOCLIENTE>\n";
                stream << "\t\t\t<IDDPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "iddpedidocliente" ) ) << "</IDDPEDIDOCLIENTE>\n";
                stream << "\t\t\t<CONCEPTDPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "conceptdpedidocliente" ) ) << "</CONCEPTDPEDIDOCLIENTE>\n";
                stream << "\t\t\t<PROPORCIONDPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "proporciondpedidocliente" ) ) << "</PROPORCIONDPEDIDOCLIENTE>\n";
                stream << "\t\t\t<IDPEDIDOCLIENTE>" << blXMLEncode ( curlc->value( "idpedidocliente" ) ) << "</IDPEDIDOCLIENTE>\n";
                stream << "\t\t</DPEDIDOCLIENTE>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            stream << "</PEDIDOCLIENTE>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if

    /// Hacemos la exportacion de facturas de clientes.
    if ( tipo & IMPORT_ALBARANESCLIENTE ) {
        QString query = "SELECT * FROM albaran ";
        query += " LEFT JOIN almacen ON albaran.idalmacen = almacen.idalmacen ";
        query += " LEFT JOIN cliente ON cliente.idcliente = albaran.idcliente ";
        query += " LEFT JOIN trabajador ON trabajador.idtrabajador = albaran.idtrabajador ";
        query += " LEFT JOIN forma_pago ON forma_pago.idforma_pago = albaran.idforma_pago ";
        BlDbRecordSet *curc = dbConnection->loadQuery ( query );
        while ( !curc->eof() ) {
            stream << "<ALBARAN>\n";
            stream << "\t<IDALBARAN>" << blXMLEncode ( curc->value( "idalbaran" ) ) << "</IDALBARAN>\n";
            stream << "\t<NUMALBARAN>" << blXMLEncode ( curc->value( "numalbaran" ) ) << "</NUMALBARAN>\n";
            stream << "\t<DESCALBARAN>" << blXMLEncode ( curc->value( "descalbaran" ) ) << "</DESCALBARAN>\n";
            stream << "\t<REFALBARAN>" << blXMLEncode ( curc->value( "refalbaran" ) ) << "</REFALBARAN>\n";
            stream << "\t<FECHAALBARAN>" << blXMLEncode ( curc->value( "fechaalbaran" ) ) << "</FECHAALBARAN>\n";
            stream << "\t<COMENTALBARAN>" << blXMLEncode ( curc->value( "comentalbaran" ) ) << "</COMENTALBARAN>\n";
            stream << "\t<PROCESADOALBARAN>" << blXMLEncode ( curc->value( "procesadoalbaran" ) ) << "</PROCESADOALBARAN>\n";
            stream << "\t<CONTACTALBARAN>" << blXMLEncode ( curc->value( "contactalbaran" ) ) << "</CONTACTALBARAN>\n";
            stream << "\t<TELALBARAN>" << blXMLEncode ( curc->value( "telalbaran" ) ) << "</TELALBARAN>\n";
            stream << "\t<IDUSUARI>" << blXMLEncode ( curc->value( "idusuari" ) ) << "</IDUSUARI>\n";
            stream << "\t<IDCLIENTE>" << blXMLEncode ( curc->value( "idcliente" ) ) << "</IDCLIENTE>\n";
            stream << "\t<IDALMACEN>" << blXMLEncode ( curc->value( "idalmacen" ) ) << "</IDALMACEN>\n";
            stream << "\t<IDFORMA_PAGO>" << blXMLEncode ( curc->value( "idforma_pago" ) ) << "</IDFORMA_PAGO>\n";
            stream << "\t<IDTRABAJADOR>" << blXMLEncode ( curc->value( "idtrabajador" ) ) << "</IDTRABAJADOR>\n";
            /// Datos iniciales para el albaran  que pueden ser de utilidad.
            stream << "\t<CODIGOALMACEN>" << blXMLEncode ( curc->value( "codigoalmacen" ) ) << "</CODIGOALMACEN>\n";
            stream << "\t<NOMALMACEN>" << blXMLEncode ( curc->value( "nomalmacen" ) ) << "</NOMALMACEN>\n";
            stream << "\t<DIRALMACEN>" << blXMLEncode ( curc->value( "diralmacen" ) ) << "</DIRALMACEN>\n";
            stream << "\t<POBLALMACEN>" << blXMLEncode ( curc->value( "poblalmacen" ) ) << "</POBLALMACEN>\n";
            stream << "\t<CPALMACEN>" << blXMLEncode ( curc->value( "cpalmacen" ) ) << "</CPALMACEN>\n";
            stream << "\t<TELALMACEN>" << blXMLEncode ( curc->value( "telalmacen" ) ) << "</TELALMACEN>\n";
            /// Datos referentes al cliente.
            stream << "\t<NOMCLIENTE>" << blXMLEncode ( curc->value( "nomcliente" ) ) << "</NOMCLIENTE>\n";
            stream << "\t<NOMALTCLIENTE>" << blXMLEncode ( curc->value( "nomaltcliente" ) ) << "</NOMALTCLIENTE>\n";
            stream << "\t<CIFCLIENTE>" << blXMLEncode ( curc->value( "cifcliente" ) ) << "</CIFCLIENTE>\n";
            stream << "\t<BANCOCLIENTE>" << blXMLEncode ( curc->value( "bancocliente" ) ) << "</BANCOCLIENTE>\n";
            stream << "\t<DIRCLIENTE>" << blXMLEncode ( curc->value( "dircliente" ) ) << "</DIRCLIENTE>\n";
            stream << "\t<POBLCLIENTE>" << blXMLEncode ( curc->value( "poblcliente" ) ) << "</POBLCLIENTE>\n";
            stream << "\t<CPCLIENTE>" << blXMLEncode ( curc->value( "cpcliente" ) ) << "</CPCLIENTE>\n";
            stream << "\t<TELCLIENTE>" << blXMLEncode ( curc->value( "telcliente" ) ) << "</TELCLIENTE>\n";
            stream << "\t<FAXCLIENTE>" << blXMLEncode ( curc->value( "faxcliente" ) ) << "</FAXCLIENTE>\n";
            stream << "\t<MAILCLIENTE>" << blXMLEncode ( curc->value( "mailcliente" ) ) << "</MAILCLIENTE>\n";
            stream << "\t<URLCLIENTE>" << blXMLEncode ( curc->value( "urlcliente" ) ) << "</URLCLIENTE>\n";
            stream << "\t<PROVCLIENTE>" << blXMLEncode ( curc->value( "provcliente" ) ) << "</PROVCLIENTE>\n";
            /// Datos sobre el trabajador.
            stream << "\t<NOMTRABAJADOR>" << blXMLEncode ( curc->value( "nomtrabajador" ) ) << "</NOMTRABAJADOR>\n";
            stream << "\t<APELLIDOSTRABAJADOR>" << blXMLEncode ( curc->value( "apellidostrabajador" ) ) << "</APELLIDOSTRABAJADOR>\n";
            stream << "\t<DIRTRABAJADOR>" << blXMLEncode ( curc->value( "dirtrabajador" ) ) << "</DIRTRABAJADOR>\n";
            stream << "\t<NSSTRABAJADOR>" << blXMLEncode ( curc->value( "nsstrabajador" ) ) << "</NSSTRABAJADOR>\n";
            stream << "\t<TELTRABAJADOR>" << blXMLEncode ( curc->value( "teltrabajador" ) ) << "</TELTRABAJADOR>\n";
            stream << "\t<MOVILTRABAJADOR>" << blXMLEncode ( curc->value( "moviltrabajador" ) ) << "</MOVILTRABAJADOR>\n";
            stream << "\t<EMAILTRABAJADOR>" << blXMLEncode ( curc->value( "emailtrabajador" ) ) << "</EMAILTRABAJADOR>\n";
            /// Datos sobre la forma de pago.
            stream << "\t<DESCFORMA_PAGO>" << blXMLEncode ( curc->value( "descforma_pago" ) ) << "</DESCFORMA_PAGO>\n";
            stream << "\t<DIAS1TFORMA_PAGO>" << blXMLEncode ( curc->value( "dias1tforma_pago" ) ) << "</DIAS1TFORMA_PAGO>\n";
            stream << "\t<DESCUENTOFORMA_PAGO>" << blXMLEncode ( curc->value( "descuentoforma_pago" ) ) << "</DESCUENTOFORMA_PAGO>\n";
            /// Incorporamos las lineas de detalles del presupuesto.
            BlDbRecordSet *curlc = dbConnection->loadQuery ( "SELECT * FROM lalbaran LEFT JOIN articulo ON lalbaran.idarticulo = articulo.idarticulo WHERE idalbaran = " + curc->value( "idalbaran" ) );
            while ( !curlc->eof() ) {
                stream << "\t\t<LALBARAN>\n";
                stream << "\t\t\t<IDLALBARAN>" << blXMLEncode ( curlc->value( "idlalbaran" ) ) << "</IDLALBARAN>\n";
                stream << "\t\t\t<DESCLALBARAN>" << blXMLEncode ( curlc->value( "desclalbaran" ) ) << "</DESCLPRESUPUESTO>\n";
                stream << "\t\t\t<CANTLALBARANE>" << blXMLEncode ( curlc->value( "cantlalbaran" ) ) << "</CANTLALBARANE>\n";
                stream << "\t\t\t<PVPLALBARAN>" << blXMLEncode ( curlc->value( "pvplalbaran" ) ) << "</PVPLALBARAN>\n";
                stream << "\t\t\t<IVALALBARAN>" << blXMLEncode ( curlc->value( "ivalalbaran" ) ) << "</IVALALBARAN>\n";
                stream << "\t\t\t<DESCUENTOLALBARAN>" << blXMLEncode ( curlc->value( "descuentolalbaran" ) ) << "</DESCUENTOLALBARAN>\n";
                stream << "\t\t\t<IDALBARAN>" << blXMLEncode ( curlc->value( "idalbaran" ) ) << "</IDALBARAN>\n";
                stream << "\t\t\t<IDARTICULO>" << blXMLEncode ( curlc->value( "idarticulo" ) ) << "</IDARTICULO>\n";
                /// Los datos relacionados con el articulo.
                stream << "\t\t\t<CODARTICULO>" << blXMLEncode ( curlc->value( "codarticulo" ) ) << "</CODARTICULO>\n";
                stream << "\t\t\t<NOMARTICULO>" << blXMLEncode ( curlc->value( "nomarticulo" ) ) << "</NOMARTICULO>\n";
                stream << "\t\t\t<ABREVARTICULO>" << blXMLEncode ( curlc->value( "abrevarticulo" ) ) << "</ABREVARTICULO>\n";
                stream << "\t<CODIGOCOMPLETOARTICULO>" << blXMLEncode ( curlc->value( "codigocompletoarticulo" ) ) << "</CODIGOCOMPLETOARTICULO>\n";
                stream << "\t\t</LALBARAN>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            /// Incorporamos los descuentos del presupuesto.
            curlc = dbConnection->loadQuery ( "SELECT * FROM dalbaran WHERE idalbaran = " + curc->value( "idalbaran" ) );
            while ( !curlc->eof() ) {
                stream << "\t\t<DALBARAN>\n";
                stream << "\t\t\t<IDDALBARAN>" << blXMLEncode ( curlc->value( "iddalbaran" ) ) << "</IDDALBARAN>\n";
                stream << "\t\t\t<CONCEPTDALBARAN>" << blXMLEncode ( curlc->value( "conceptdalbaran" ) ) << "</CONCEPTDALBARAN>\n";
                stream << "\t\t\t<PROPORCIONDALBARAN>" << blXMLEncode ( curlc->value( "proporciondalbaran" ) ) << "</PROPORCIONDALBARAN>\n";
                stream << "\t\t\t<IDALBARAN>" << blXMLEncode ( curlc->value( "idalbaran" ) ) << "</IDALBARAN>\n";
                stream << "\t\t</DALBARAN>\n";
                curlc->nextRecord();
            } // end while
            delete curlc;
            stream << "</ALBARAN>\n";
            curc->nextRecord();
        } // end while
        delete curc;
    } // end if
    stream << "</FUGIT>\n";
    alerta ( 100, 100 );
    
    return 0;
}


/// Esta funcion se encarga de pasar los datos de BulmaGes a XML.
/// Los datos pasados de esta forma son mucho mas sencillos de pasar.
/**
\param xmlfile
\param tipo
\return
**/
int BlImportExport::bulmages2XML ( QFile &xmlfile, unsigned long long int tipo )
{
    BL_FUNC_DEBUG
    QString codigo, descripcion;
    QString strblancomax;
    QString query;
    int numreg = 0;
    QTextStream stream ( &xmlfile );
    stream << "<?xml version=\"1.0\" encoding = \"iso-8859-1\"?>\n"
    "<!DOCTYPE FUGIT>\n"
    "<FUGIT version='0.3.1' origen=''"
    " date='" << QDate().toString ( Qt::ISODate ) << "'>\n";

    /// Comprobamos que tenemos que importar cuentas o no
    if ( tipo & IMPORT_CUENTAS ) {
        /// Se exporta todo el plan contable
        query = "SELECT * FROM cuenta WHERE padre ISNULL ORDER BY codigo";
        BlDbRecordSet *curcta = dbConnection->loadQuery ( query );
        while ( !curcta->eof() ) {
            stream << "<CUENTA>\n";
            stream << "\t<IDCUENTA>" << blXMLEncode ( curcta->value( "idcuenta" ) ) << "</IDCUENTA>\n";
            stream << "\t<CODIGO>" << blXMLEncode ( curcta->value( "codigo" ) ) << "</CODIGO>\n";
            stream << "\t<DESCRIPCION>" << blXMLEncode ( curcta->value( "descripcion" ) ) << "</DESCRIPCION>\n";
            stream << "\t<CIFENT_CUENTA>" << blXMLEncode ( curcta->value( "cifent_cuenta" ) ) << "</CIFENT_CUENTA>\n";
            stream << "\t<DIRENT_CUENTA>" << blXMLEncode ( curcta->value( "dirent_cuenta" ) ) << "</DIRENT_CUENTA>\n";
            stream << "\t<BLOQUEADA>" << blXMLEncode ( curcta->value( "bloqueada" ) ) << "</BLOQUEADA>\n";
            stream << "\t<NODEBE>" << blXMLEncode ( curcta->value( "nodebe" ) ) << "</NODEBE>\n";
            stream << "\t<NOHABER>" << blXMLEncode ( curcta->value( "nohaber" ) ) << "</NOHABER>\n";
            stream << "\t<NOMBREENT_CUENTA>" << blXMLEncode ( curcta->value( "nombreent_cuenta" ) ) << "</NOMBREENT_CUENTA>\n";
            stream << "\t<TIPOCUENTA>" << blXMLEncode ( curcta->value( "tipocuenta" ) ) << "</TIPOCUENTA>\n";
            stream << "\t<WEBENT_CUENTA>" << blXMLEncode ( curcta->value( "webent_cuenta" ) ) << "</WEBENT_CUENTA>\n";
            stream << "\t<EMAILENT_CUENTA>" << blXMLEncode ( curcta->value( "emailent_cuenta" ) ) << "</EMAILENT_CUENTA>\n";
            stream << "\t<BANCOENT_CUENTA>" << blXMLEncode ( curcta->value( "bancoent_cuenta" ) ) << "</BANCOENT_CUENTA>\n";
            stream << "\t<COMENT_CUENTA>" << blXMLEncode ( curcta->value( "coment_cuenta" ) ) << "</COMENT_CUENTA>\n";
            stream << "\t<TELENT_CUENTA>" << blXMLEncode ( curcta->value( "telent_cuenta" ) ) << "</TELENT_CUENTA>\n";
            stream << "\t<CPENT_CUENTA>" << blXMLEncode ( curcta->value( "cpent_cuenta" ) ) << "</CPENT_CUENTA>\n";
            stream << "\t<REGULARIZACION>" << blXMLEncode ( curcta->value( "regularizacion" ) ) << "</REGULARIZACION>\n";
            stream << "\t<IMPUTACION>" << blXMLEncode ( curcta->value( "imputacion" ) ) << "</IMPUTACION>\n";
            stream << "</CUENTA>\n";
            curcta->nextRecord();
        } // end while
        delete curcta;
        query = "SELECT * FROM cuenta LEFT JOIN (SELECT codigo AS codpadre, idcuenta as idpadre FROM cuenta) AS t1 ON cuenta.padre = t1.idpadre WHERE padre IS NOT NULL ORDER BY idpadre";
        curcta = dbConnection->loadQuery ( query );
        while ( !curcta->eof() ) {
            stream << "<CUENTA>\n";
            stream << "\t<IDCUENTA>" << blXMLEncode ( curcta->value( "idcuenta" ) ) << "</IDCUENTA>\n";
            stream << "\t<CODIGO>" << blXMLEncode ( curcta->value( "codigo" ) ) << "</CODIGO>\n";
            stream << "\t<DESCRIPCION>" << blXMLEncode ( curcta->value( "descripcion" ) ) << "</DESCRIPCION>\n";
            stream << "\t<CIFENT_CUENTA>" << blXMLEncode ( curcta->value( "cifent_cuenta" ) ) << "</CIFENT_CUENTA>\n";
            stream << "\t<DIRENT_CUENTA>" << blXMLEncode ( curcta->value( "dirent_cuenta" ) ) << "</DIRENT_CUENTA>\n";
            stream << "\t<CODPADRE>" << blXMLEncode ( curcta->value( "codpadre" ) ) << "</CODPADRE>\n";
            stream << "\t<BLOQUEADA>" << blXMLEncode ( curcta->value( "bloqueada" ) ) << "</BLOQUEADA>\n";
            stream << "\t<NODEBE>" << blXMLEncode ( curcta->value( "nodebe" ) ) << "</NODEBE>\n";
            stream << "\t<NOHABER>" << blXMLEncode ( curcta->value( "nohaber" ) ) << "</NOHABER>\n";
            stream << "\t<NOMBREENT_CUENTA>" << blXMLEncode ( curcta->value( "nombreent_cuenta" ) ) << "</NOMBREENT_CUENTA>\n";
            stream << "\t<TIPOCUENTA>" << blXMLEncode ( curcta->value( "tipocuenta" ) ) << "</TIPOCUENTA>\n";
            stream << "\t<WEBENT_CUENTA>" << blXMLEncode ( curcta->value( "webent_cuenta" ) ) << "</WEBENT_CUENTA>\n";
            stream << "\t<EMAILENT_CUENTA>" << blXMLEncode ( curcta->value( "emailent_cuenta" ) ) << "</EMAILENT_CUENTA>\n";
            stream << "\t<BANCOENT_CUENTA>" << blXMLEncode ( curcta->value( "bancoent_cuenta" ) ) << "</BANCOENT_CUENTA>\n";
            stream << "\t<COMENT_CUENTA>" << blXMLEncode ( curcta->value( "coment_cuenta" ) ) << "</COMENT_CUENTA>\n";
            stream << "\t<TELENT_CUENTA>" << blXMLEncode ( curcta->value( "telent_cuenta" ) ) << "</TELENT_CUENTA>\n";
            stream << "\t<CPENT_CUENTA>" << blXMLEncode ( curcta->value( "cpent_cuenta" ) ) << "</CPENT_CUENTA>\n";
            stream << "\t<REGULARIZACION>" << blXMLEncode ( curcta->value( "regularizacion" ) ) << "</REGULARIZACION>\n";
            stream << "\t<IMPUTACION>" << blXMLEncode ( curcta->value( "imputacion" ) ) << "</IMPUTACION>\n";
            stream << "</CUENTA>\n";
            curcta->nextRecord();
        } // end while
        delete curcta;
    } // end if

    /// Se va a comprobar si hay que exportar los tipos de IVA.
    if ( tipo & IMPORT_TIPOSIVA ) {
        /// Se vana exportar los tipos de IVA.
        query = "SELECT * from tipoiva LEFT JOIN cuenta ON cuenta.idcuenta = tipoiva.idcuenta";
        BlDbRecordSet *curtiva = dbConnection->loadQuery ( query );
        while ( !curtiva->eof() ) {
            stream << "<TIPOIVA>\n";
            stream << "\t<IDTIPOIVA>" << blXMLEncode ( curtiva->value( "idtipoiva" ) ) << "</IDTIPOIVA>\n";
            stream << "\t<NOMBRETIPOIVA>" << blXMLEncode ( curtiva->value( "nombretipoiva" ) ) << "</NOMBRETIPOIVA>\n";
            stream << "\t<PORCENTAJETIPOIVA>" << blXMLEncode ( curtiva->value( "porcentajetipoiva" ) ) << "</PORCENTAJETIPOIVA>\n";
            stream << "\t<CUENTATIPOIVA>" << blXMLEncode ( curtiva->value( "codigo" ) ) << "</CUENTATIPOIVA>\n";
            stream << "</TIPOIVA>\n";
            curtiva->nextRecord();
        } // end while
        delete curtiva;
    } // end if

    if ( tipo & IMPORT_ASIENTOS ) {
        /// Hacemos la exportacion de asientos
        /// Montamos el query
        query = "SELECT * FROM asiento WHERE 1 = 1 ";
        if ( m_fInicial != "" )
            query += " AND asiento.fecha >= '" + m_fInicial + "'";
        if ( m_fFinal != "" )
            query += " AND asiento.fecha <= '" + m_fFinal + "'";
        query += " ORDER BY asiento.fecha, asiento.idasiento ";
        BlDbRecordSet *curas = dbConnection->loadQuery ( query );
        int i = 0;
        /// Iteramos.
        numreg = curas->numregistros() + 1;
        while ( !curas->eof() ) {
            alerta ( i++, numreg );
            stream << "<ASIENTO>\n";
            stream << "\t<ORDENASIENTO>" << curas->value( "ordenasiento" ) << "</ORDENASIENTO>\n";
            QString fechas = curas->value( "fecha" );
            fechas = fechas.mid ( 6, 4 ) + fechas.mid ( 3, 2 ) + fechas.mid ( 0, 2 );
            stream << "\t<FECHA>" << fechas << "</FECHA>\n";
            query = "SELECT * FROM apunte LEFT JOIN cuenta ON apunte.idcuenta = cuenta.idcuenta ";
            query += "LEFT JOIN (SELECT nombre AS nomcanal, idcanal FROM canal) AS canal1 ON apunte.idcanal = canal1.idcanal ";
            query += "LEFT JOIN (SELECT nombre AS nc_coste, idc_coste FROM c_coste) AS c_coste1 ON c_coste1.idc_coste = apunte.idc_coste ";
            query += "LEFT JOIN (SELECT codigo AS codcontrapartida, idcuenta FROM cuenta) AS contra ON contra.idcuenta = apunte.contrapartida ";
            query += " WHERE " + curas->value( "idasiento" ) + " = apunte.idasiento ";
            BlDbRecordSet *curap = dbConnection->loadQuery ( query, "masquery1" );
            while ( !curap->eof() ) {
                stream << "\t<APUNTE>\n";
                QString fecha = curap->value( "fecha" );
                fecha = fecha.mid ( 6, 4 ) + fecha.mid ( 3, 2 ) + fecha.mid ( 0, 2 );
                stream << "\t\t<FECHA>" << blXMLEncode ( fecha ) << "</FECHA>\n";
                stream << "\t\t<CODIGO>" << blXMLEncode ( curap->value( "codigo" ) ) << "</CODIGO>\n";
                stream << "\t\t<DEBE>" << blXMLEncode ( curap->value( "debe" ) ) << "</DEBE>\n";
                stream << "\t\t<HABER>" << blXMLEncode ( curap->value( "haber" ) ) << "</HABER>\n";
                stream << "\t\t<CONCEPTOCONTABLE>" << blXMLEncode ( curap->value( "conceptocontable" ) ) << "</CONCEPTOCONTABLE>\n";
                stream << "\t\t<IDCANAL>" << blXMLEncode ( curap->value( "idcanal" ) ) << "</IDCANAL>\n";
                stream << "\t\t<CANAL>" << blXMLEncode ( curap->value( "nomcanal" ) ) << "</CANAL>\n";
                stream << "\t\t<IDC_COSTE>" << blXMLEncode ( curap->value( "idc_coste" ) ) << "</IDC_COSTE>\n";
                stream << "\t\t<C_COSTE>" << blXMLEncode ( curap->value( "nc_coste" ) ) << "</C_COSTE>\n";
                stream << "\t\t<PUNTEO>" << blXMLEncode ( curap->value( "punteo" ) ) << "</PUNTEO>\n";
                stream << "\t\t<ORDEN>" << blXMLEncode ( curap->value( "orden" ) ) << "</ORDEN>\n";
                stream << "\t\t<CONTRAPARTIDA>" << blXMLEncode ( curap->value( "codcontrapartida" ) ) << "</CONTRAPARTIDA>\n";
                /// Hacemos la exportacion de registros de IVA.
                query  = "SELECT * FROM registroiva";
                query += " LEFT JOIN (SELECT codigo, idcuenta FROM cuenta) AS t1 ON registroiva.contrapartida = t1.idcuenta ";
                query += " WHERE idborrador IN (SELECT idborrador FROM borrador WHERE idasiento=" + curas->value( "idasiento" ) + " AND orden = " + curap->value( "orden" ) + ")";
                BlDbRecordSet *curreg = dbConnection->loadQuery ( query, "queryregiva" );
                while ( !curreg->eof() ) {
                    stream << "\t\t<REGISTROIVA>\n";
                    stream << "\t\t\t<CONTRAPARTIDA>" << blXMLEncode ( curreg->value( "codigo" ) ) << "</CONTRAPARTIDA>\n";
                    stream << "\t\t\t<BASEIMP>" << blXMLEncode ( curreg->value( "baseimp" ) ) << "</BASEIMP>\n";
                    stream << "\t\t\t<IVA>" << blXMLEncode ( curreg->value( "iva" ) ) << "</IVA>\n";
                    stream << "\t\t\t<FFACTURA>" << blXMLEncode ( curreg->value( "ffactura" ) ) << "</FFACTURA>\n";
                    stream << "\t\t\t<FACTURA>" << blXMLEncode ( curreg->value( "factura" ) ) << "</FACTURA>\n";
                    stream << "\t\t\t<NUMORDEN>" << blXMLEncode ( curreg->value( "numorden" ) ) << "</NUMORDEN>\n";
                    stream << "\t\t\t<CIF>" << blXMLEncode ( curreg->value( "cif" ) ) << "</CIF>\n";
                    stream << "\t\t\t<IDFPAGO>" << blXMLEncode ( curreg->value( "idfpago" ) ) << "</IDFPAGO>\n";
                    stream << "\t\t\t<RECTIFICAAREGISTROIVA>" << blXMLEncode ( curreg->value( "rectificaaregistroiva" ) ) << "</RECTIFICAAREGISTROIVA>\n";
                    /// Hacemos la exportacion de IVAs.
                    query  = "SELECT * FROM iva ";
                    query += " LEFT JOIN tipoiva ON iva.idtipoiva = tipoiva.idtipoiva ";
                    query += " WHERE idregistroiva = " + curreg->value( "idregistroiva" );
                    BlDbRecordSet *curiva = dbConnection->loadQuery ( query, "queryiva" );
                    while ( !curiva->eof() ) {
                        stream << "\t\t\t<RIVA>\n";
                        stream << "\t\t\t\t<IDTIPOIVA>" << blXMLEncode ( curiva->value( "idtipoiva" ) ) << "</IDTIPOIVA>\n";
                        stream << "\t\t\t\t<NOMBRETIPOIVA>" << blXMLEncode ( curiva->value( "nombretipoiva" ) ) << "</NOMBRETIPOIVA>\n";
                        stream << "\t\t\t\t<BASEIVA>" << blXMLEncode ( curiva->value( "baseiva" ) ) << "</BASEIVA>\n";
                        stream << "\t\t\t</RIVA>\n";
                        curiva->nextRecord();
                    } // end while
                    delete curiva;
                    stream << "\t\t</REGISTROIVA>\n";
                    curreg->nextRecord();
                } // end while
                delete curreg;
                mensajeria ( _ ( "Exportando :" ) + curap->value( "codigo" ) + "--" + fecha + "\n" );
                curap->nextRecord();
                stream << "\t</APUNTE>\n";
            } // end while
            delete curap;
            stream << "</ASIENTO>\n";
            curas->nextRecord();
        } // end while
        delete curas;
    } // end if
    stream << "</FUGIT>\n";
    alerta ( numreg, numreg );
    
    return 0;
}


/// Funcion para pasar de un archivo XML a Bulmages.
/** Crea un objeto del tipo \ref StructureParser (sistema de proceso de XML mediante SAX)
    y lo ejecuta para que haga la imporacion del archivo XML. */
/**
\param fichero
\param tip
\return
**/
int BlImportExport::XML2Bulmages ( QFile &fichero, unsigned long long int tip )
{
    BL_FUNC_DEBUG
    StructureParser handler ( dbConnection, tip );
    QXmlInputSource source ( &fichero );
    QXmlSimpleReader reader;
    reader.setContentHandler ( &handler );
    reader.parse ( source );
    alerta ( 100, 100 );
    
    return 0;
}


/// Funcion para pasar de un archivo XML a BulmaFact.
/** Crea un objeto del tipo \ref StructureParser (sistema de proceso de XML mediante SAX) y lo ejecuta para
    que haga la imporacion del archivo XML. */
/**
\param fichero
\param tip
\return
**/
int BlImportExport::XML2BulmaFact ( QFile &fichero, unsigned long long int tip )
{
    BL_FUNC_DEBUG
    bool noerror = true;
    ImportBulmaFact handler ( this, dbConnection, tip );
    QXmlInputSource source ( &fichero );
    QXmlSimpleReader reader;
    reader.setContentHandler ( &handler );
    noerror = reader.parse ( source );
    if ( !noerror ) {
        QMessageBox::warning ( 0, _ ( "Application name" ), _ ( "Error en el XML.\nDocumento mal formado." ), "OK", 0, 0 );
    }
    alerta ( 100, 100 );
    
    return 0;
}


///
/**
\param con
\param tip
**/
StructureParser::StructureParser ( BlPostgreSqlClient *con, unsigned int tip )
{
    BL_FUNC_DEBUG
    dbConnection = con;
    m_tipo = tip;
    dbConnection->begin();
    QString query = "INSERT INTO cuenta (codigo, descripcion) VALUES ('AUX', 'Una descripcion auxiliar de cuenta')";
    dbConnection->runQuery ( query );
    for ( int i = 0; i <= 12; i++ ) {
        QString query2 = "INSERT INTO ejercicios (ejercicio, periodo, bloqueado) VALUES (2005, " + QString::number ( i ) + ", FALSE)";
        dbConnection->runQuery ( query2 );
    } // end for
    dbConnection->commit();
    
}


///
/**
**/
StructureParser::~StructureParser()
{
    BL_FUNC_DEBUG
    QString query = "DELETE FROM cuenta WHERE codigo = 'AUX'";
    dbConnection->runQuery ( query );
    
}

///
/**
\return
**/
bool StructureParser::startDocument()
{
    BL_FUNC_DEBUG
    indent = "";
    
    return true;
}


///
/**
\param qName
\param QXmlAttributes
\return
**/
bool StructureParser::startElement ( const QString&, const QString&, const QString& qName, const QXmlAttributes& )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "StructureParser::startElement", 0, qName );
    indent += "..";
    if ( qName == "ASIENTO" && m_tipo & IMPORT_ASIENTOS ) {
        tagpadre = "ASIENTO";
        QString query = "INSERT INTO asiento (descripcion, fecha) VALUES ('un nuevo asiento', '01/01/2005')";
        dbConnection->begin();
        dbConnection->runQuery ( query );
        BlDbRecordSet *cur = dbConnection->loadQuery ( "SELECT MAX(idasiento) AS max FROM asiento", "otroquery" );
        dbConnection->commit();
        if ( !cur->eof() ) {
            idasiento = cur->value( "max" );
        } // end if
        /// Iniciamos el orden para que los apuntes salgan en orden empezando desde cero.
        m_ordenapunte = 0;
        delete cur;
    } // end if
    if ( qName == "APUNTE" && m_tipo & IMPORT_ASIENTOS ) {
        QString query = "INSERT INTO borrador (idasiento, debe, haber, idcuenta, fecha, orden) VALUES (" + idasiento + ", 0, 0, id_cuenta('AUX'), '01/01/2003', " + QString::number ( m_ordenapunte++ ) + ")";
        dbConnection->begin();
        dbConnection->runQuery ( query );
        BlDbRecordSet *cur = dbConnection->loadQuery ( "SELECT MAX(idborrador) AS max FROM borrador", "otroquery1" );
        dbConnection->commit();
        if ( !cur->eof() ) {
            idborrador = cur->value( "max" );
        } // end if
        delete cur;
        tagpadre = "APUNTE";
    } // end if
    if ( qName == "REGISTROIVA" && m_tipo & IMPORT_FACTURAS ) {
        QString query = "INSERT INTO registroiva (contrapartida, idborrador) VALUES (id_cuenta('AUX'), " + idborrador + ")";
        dbConnection->begin();
        dbConnection->runQuery ( query );
        BlDbRecordSet *cur = dbConnection->loadQuery ( "SELECT MAX(idregistroiva) AS max FROM registroiva", "otroquery13" );
        dbConnection->commit();
        if ( !cur->eof() ) {
            m_idRegistroIva = cur->value( "max" );
        } // end if
        delete cur;
        tagpadre = "REGISTROIVA";
    } // end if
    if ( qName == "RIVA" && m_tipo & IMPORT_FACTURAS ) {
        tagpadre = "RIVA";
    } // end if
    if ( qName == "CUENTA" && m_tipo & IMPORT_CUENTAS ) {
        tagpadre = "CUENTA";
        /// Borramos todos los elementos para que no haya traspasos de informacion.
        codigocuenta = "";
        descripcioncuenta = "";
        codigopadre = "";
        m_bloqueadaCuenta = "";
        m_nodebeCuenta = "";
        m_nohaberCuenta = "";
        m_tipoCuenta = "";
    } // end if
    cadintermedia = "";
    
    return true;
}


///
/**
\param qName
\return
**/
bool StructureParser::endElement ( const QString&, const QString&, const QString& qName )
{
    BL_FUNC_DEBUG
    BlDebug::blDebug ( "StructureParser::endElement", 0, qName );
    indent.remove ( ( uint ) 0, 2 );
    /// Vamos a ir distinguiendo casos y actuando segun cada caso.
    /// En la mayoria de casos iremos actuando en consecuencia.
    /// Ha terminado un asiento, por tanto hacemos el update de los campos.
    if ( qName == "ASIENTO" && m_tipo & IMPORT_ASIENTOS ) {
        QString query = "UPDATE asiento set fecha = '" +
                        dbConnection->sanearCadena ( blXMLDecode ( fechaasiento ) ) + "' WHERE idasiento = " +
                        dbConnection->sanearCadena ( blXMLDecode ( idasiento ) );
        dbConnection->begin();
        dbConnection->runQuery ( query );
        BlDbRecordSet *cur = dbConnection->loadQuery ( "SELECT cierraasiento(" + idasiento + ")" );
        dbConnection->commit();
        delete cur;
    } // end if
    /// Si es un apunte hacemos su insercion.
    if ( qName == "APUNTE" && m_tipo & IMPORT_ASIENTOS ) {
        QString query = "UPDATE borrador SET debe = " +
                        dbConnection->sanearCadena ( debeapunte ) + ", haber = " +
                        dbConnection->sanearCadena ( haberapunte ) + ", idcuenta = id_cuenta('" +
                        dbConnection->sanearCadena ( codigocuentaapunte ) + "'), fecha = '" +
                        dbConnection->sanearCadena ( fechaapunte ) + "', conceptocontable = '" +
                        dbConnection->sanearCadena ( blXMLDecode ( conceptocontableapunte ) ) + "' WHERE idborrador = " + idborrador;
        dbConnection->runQuery ( query );
    } // end if
    if ( qName == "FECHA" && tagpadre == "ASIENTO" )
        fechaasiento = cadintermedia;
    if ( qName == "FECHA" && tagpadre == "APUNTE" )
        fechaapunte = cadintermedia;
    if ( qName == "DEBE" && tagpadre == "APUNTE" )
        debeapunte = cadintermedia;
    if ( qName == "HABER" && tagpadre == "APUNTE" )
        haberapunte = cadintermedia;
    if ( qName == "CODIGO" && tagpadre == "APUNTE" )
        codigocuentaapunte = cadintermedia;
    if ( qName == "CONCEPTOCONTABLE" && tagpadre == "APUNTE" )
        conceptocontableapunte = cadintermedia;
    if ( qName == "CUENTA" && m_tipo & IMPORT_CUENTAS ) {
        /// Ha terminado una cuenta, por tanto hacemos la insercion de la misma.
        /// Podemos hacer la insercion y no un sistema de update pq la cuenta no tiene hijos en el XML.
        /// Nuevo Socio M.Mezo
        QString idgrupo = codigocuenta.left ( 1 );
        /// Primero debemos determinar si existe o no dicha cuenta para hacer la insercion o la modificacion.
        QString vidcuenta;
        if ( codigopadre != "" )  {
            vidcuenta = "id_cuenta('" + dbConnection->sanearCadena ( blXMLDecode ( codigopadre ) ) + "')";
        } else {
            vidcuenta = "NULL";
        } // end if
        /// Si el tipo de cuenta esta vacio lo ponemos a NULL para que no haya error en la base de datos.
        if ( m_tipoCuenta == "" )
            m_tipoCuenta = "NULL";

        QString query = "SELECT * FROM cuenta WHERE codigo = '" + codigocuenta + "'";
        BlDbRecordSet *cur = dbConnection->loadQuery ( query );
        if ( cur->eof() ) {
            QString query = "INSERT INTO cuenta (tipocuenta, codigo, descripcion, padre, bloqueada, nodebe, nohaber) VALUES (" +
                            dbConnection->sanearCadena ( m_tipoCuenta ) + ",'" +
                            dbConnection->sanearCadena ( codigocuenta ) + "','" +
                            dbConnection->sanearCadena ( blXMLDecode ( descripcioncuenta ) ) + "', " +
                            vidcuenta + ", '" +
                            dbConnection->sanearCadena ( m_bloqueadaCuenta ) + "','" +
                            dbConnection->sanearCadena ( m_nodebeCuenta ) + "','" +
                            dbConnection->sanearCadena ( m_nohaberCuenta ) + "')";
            dbConnection->runQuery ( query );
        } else {
            QString query = "UPDATE cuenta SET ";
            query += "descripcion = '" + dbConnection->sanearCadena ( blXMLDecode ( descripcioncuenta ) ) + "'";
            query += ", tipocuenta = " + dbConnection->sanearCadena ( m_tipoCuenta );
            query += ", bloqueada = '" + dbConnection->sanearCadena ( m_bloqueadaCuenta ) + "'";
            query += ", nodebe = '" + dbConnection->sanearCadena ( m_nodebeCuenta ) + "'";
            query += ", nohaber = '" + dbConnection->sanearCadena ( m_nohaberCuenta ) + "'";
            query += " WHERE codigo = '" + dbConnection->sanearCadena ( codigocuenta ) + "'";
            dbConnection->runQuery ( query );
        } //end if
        delete cur;
    } // end if
    if ( qName == "CODIGO" && tagpadre == "CUENTA" )
        codigocuenta = cadintermedia;
    if ( qName == "DESCRIPCION" && tagpadre == "CUENTA" )
        descripcioncuenta = cadintermedia;
    if ( qName == "CODPADRE" && tagpadre == "CUENTA" )
        codigopadre = cadintermedia;
    if ( qName == "BLOQUEADA" && tagpadre == "CUENTA" )
        m_bloqueadaCuenta = cadintermedia;
    if ( qName == "NODEBE" && tagpadre == "CUENTA" )
        m_nodebeCuenta = cadintermedia;
    if ( qName == "NOHABER" && tagpadre == "CUENTA" )
        m_nohaberCuenta = cadintermedia;
    if ( qName == "TIPOCUENTA" && tagpadre == "CUENTA" )
        m_tipoCuenta = cadintermedia;
    /// Si es un registro de iva vamos a por el.
    if ( qName == "REGISTROIVA" && m_tipo & IMPORT_FACTURAS ) {
        QString query = "UPDATE registroiva SET contrapartida = id_cuenta('" + m_rIvaContrapartida + "'), ffactura = '" + m_rIvaFFactura + "'";
        if ( m_rIvaBaseImp != "" )
            query += ", baseimp = " + m_rIvaBaseImp;
        if ( m_rIvaIva != "" )
            query += ", iva = " + m_rIvaIva;
        query += " WHERE idregistroiva = " + m_idRegistroIva;
        dbConnection->runQuery ( query );
    } // end if
    if ( qName == "CONTRAPARTIDA" && tagpadre == "REGISTROIVA" )
        m_rIvaContrapartida = cadintermedia;
    if ( qName == "BASEIMP" && tagpadre == "REGISTROIVA" )
        m_rIvaBaseImp = cadintermedia;
    if ( qName == "IVA" && tagpadre == "REGISTROIVA" )
        m_rIvaIva = cadintermedia;
    if ( qName == "FFACTURA" && tagpadre == "REGISTROIVA" )
        m_rIvaFFactura = cadintermedia;
    if ( qName == "FACTURA" && tagpadre == "REGISTROIVA" )
        m_rIvaFactura = cadintermedia;
    if ( qName == "CIF" && tagpadre == "REGISTROIVA" )
        m_rIvaCIF = cadintermedia;
    if ( qName == "IDFPAGO" && tagpadre == "REGISTROIVA" )
        m_rIvaIdFPago = cadintermedia;
    if ( qName == "RECTIFICAAREGISTROIVA" && tagpadre == "REGISTROIVA" )
        m_rIvRecRegIva = cadintermedia;
    /// Insercion de IVA's dentro del registro de IVA.
    if ( qName == "RIVA" && m_tipo & IMPORT_FACTURAS ) {
        QString query1 = "SELECT idtipoiva FROM tipoiva WHERE nombretipoiva = '" + m_nombreTipoIva + "'";
        dbConnection->begin();
        BlDbRecordSet * cur = dbConnection->loadQuery ( query1, "elqueryd" );
        if ( !cur->eof() ) {
            QString query = "INSERT INTO IVA (idregistroiva, idtipoiva, baseiva) VALUES (" + m_idRegistroIva + ", " + cur->value( "idtipoiva" ) + ", " + m_baseIva + ")";
            dbConnection->runQuery ( query );
        } // end if
        delete cur;
        dbConnection->commit();
    } // end if
    if ( qName == "IDTIPOIVA" && tagpadre == "RIVA" )
        m_idTipoIva = cadintermedia;
    if ( qName == "NOMBRETIPOIVA" && tagpadre == "RIVA" )
        m_nombreTipoIva = cadintermedia;
    if ( qName == "BASEIVA" && tagpadre == "RIVA" )
        m_baseIva = cadintermedia;
    cadintermedia = "";
    
    return true;
}

///
/**
\param nl
\return
**/
bool StructureParser::characters ( const QString& n1 )
{
    BL_FUNC_DEBUG
    cadintermedia += n1;
    
    return true;
}


/// ==================================================================================
///
/**
\param imp
\param con
\param tip
**/
ImportBulmaFact::ImportBulmaFact ( BlImportExport *imp, BlPostgreSqlClient *con, unsigned long long int tip )
{
    BL_FUNC_DEBUG
    dbConnection = con;
    pgimport = imp;
    m_tipo = tip;
    
}


///
/**
**/
ImportBulmaFact::~ImportBulmaFact()
{
    BL_FUNC_DEBUG
    
}


///
/**
\return
**/
bool ImportBulmaFact::startDocument()
{
    BL_FUNC_DEBUG
    indent = "";
    
    return true;
}


///
/**
\return
**/
bool ImportBulmaFact::startElement ( const QString&, const QString&, const QString&, const QXmlAttributes& )
{
    BL_FUNC_DEBUG
    cadintermedia = "";
    
    return true;
}


///
/**
\param qName
\return
**/
bool ImportBulmaFact::endElement ( const QString&, const QString&, const QString& qName )
{
    BL_FUNC_DEBUG
    valores[qName] = cadintermedia;
    cadintermedia = "";
    fprintf ( stderr, "Tag de Cierre: %s\n", qName.toLatin1().data() );
    if ( qName == "CLIENTE" )
        trataCliente();
    if ( qName == "PROVEEDOR" )
        trataProveedor();
    if ( qName == "FORMA_PAGO" )
        trataFormaPago();
    if ( qName == "ALMACEN" )
        trataAlmacen();
    if ( qName == "ARTICULO" )
        trataArticulo();
    if ( qName == "PRESUPUESTO" )
        trataPresupuesto();
    if ( qName == "LPRESUPUESTO" )
        trataLPresupuesto();
    if ( qName == "DPRESUPUESTO" )
        trataDPresupuesto();
    if ( qName == "FAMILIA" )
        trataFamilia();
    if ( qName == "FACTURA" )
        trataFactura();
    if ( qName == "LFACTURA" )
        trataLFactura();
    if ( qName == "DFACTURA" )
        trataDFactura();
    if ( qName == "ALBARAN" )
        trataAlbaran();
    if ( qName == "LALBARAN" )
        trataLAlbaran();
    if ( qName == "DALBARAN" )
        trataDAlbaran();
    if ( qName == "PEDIDOCLIENTE" )
        trataPedidoCliente();
    if ( qName == "LPEDIDOCLIENTE" )
        trataLPedidoCliente();
    if ( qName == "DPEDIDOCLIENTE" )
        trataDPedidoCliente();
    
    return true;
}


///
/**
\param nl
\return
**/
bool ImportBulmaFact::characters ( const QString& n1 )
{
    BL_FUNC_DEBUG
    QString val = n1;
    if ( val == "\n" )
        val = "";
    cadintermedia += val;
    
    return true;
}


///
/**
**/
void ImportBulmaFact::printcontents()
{
    BL_FUNC_DEBUG
    fprintf ( stderr, "Impresion de contenidos\n" );
    tvalores::Iterator it;
    for ( it = valores.begin(); it != valores.end(); ++it ) {
        fprintf ( stderr, "Valores encontrados clave: %s Valor:%s\n",
                  it.key().toLatin1().data(),
                  it.value().toLatin1().data() );
    } // end for
    fprintf ( stderr, "Fin de impresion de contenidos\n" );
    
}


///
/**
\return
**/
int ImportBulmaFact::trataCliente()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( _ ( "<HR><B>Tratando el cliente " ) + valores["CIFCLIENTE"] + " " + valores["NOMCLIENTE"] + "</B><BR>" );
    /// En la importacion de un cliente hay que hacer la comprobacion del DNI para saber si existe o no.
    QString dcif = valores["CIFCLIENTE"];
    if ( dcif != "" ) {
        QString query = "SELECT * FROM cliente WHERE cifcliente SIMILAR TO '" + dcif + "'";
        BlDbRecordSet *cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            /// El cliente ya existe se pueden hacer modificaciones.
            pgimport->mensajeria ( _ ( "<LI> El cliente ya existe se pueden hacer modificaciones</LI>\n" ) );
	    
	    QString query = "UPDATE cliente SET ";
	    query += "nomcliente = "; query += valores["NOMCLIENTE"] == "" ? "NULL" : "'" + valores["NOMCLIENTE"]  + "'"; query += ",";
	    query += "nomaltcliente = "; query += valores["NOMALTCLIENTE"] == "" ? "NULL" : "'" + valores["NOMALTCLIENTE"]  + "'"; query += ",";
	    query += "cifcliente = "; query += valores["CIFCLIENTE"] == "" ? "NULL" : "'" + valores["CIFCLIENTE"]  + "'"; query += ",";
	    query += "codcliente = "; query += valores["CODCLIENTE"] == "" ? "NULL" : "'" + valores["CODCLIENTE"]  + "'"; query += ",";
	    query += "bancocliente = "; query += valores["BANCOCLIENTE"] == "" ? "NULL" : "'" + valores["BANCOCLIENTE"]  + "'"; query += ",";
	    query += "dircliente = "; query += valores["DIRCLIENTE"] == "" ? "NULL" : "'" + valores["DIRCLIENTE"]  + "'"; query += ",";
	    query += "poblcliente = "; query += valores["POBLCLIENTE"] == "" ? "NULL" : "'" + valores["POBLCLIENTE"]  + "'"; query += ",";
	    query += "cpcliente = "; query += valores["CPCLIENTE"] == "" ? "NULL" : "'" + valores["CPCLIENTE"]  + "'"; query += ",";
	    query += "telcliente = "; query += valores["TELCLIENTE"] == "" ? "NULL" : "'" + valores["TELCLIENTE"]  + "'"; query += ",";
	    query += "teltrabcliente = "; query += valores["TELTRABCLIENTE"] == "" ? "NULL" : "'" + valores["TELTRABCLIENTE"]  + "'"; query += ",";
	    query += "movilcliente = "; query += valores["MOVILCLIENTE"] == "" ? "NULL" : "'" + valores["MOVILCLIENTE"]  + "'"; query += ",";
	    query += "faxcliente = "; query += valores["FAXCLIENTE"] == "" ? "NULL" : "'" + valores["FAXCLIENTE"]  + "'"; query += ",";
	    query += "mailcliente = "; query += valores["MAILCLIENTE"] == "" ? "NULL" : "'" + valores["MAILCLIENTE"]  + "'"; query += ",";
	    query += "urlcliente = "; query += valores["URLCLIENTE"] == "" ? "NULL" : "'" + valores["URLCLIENTE"]  + "'"; query += ",";
	    query += "corpcliente = "; query += valores["CORPCLIENTE"] == "" ? "NULL" : "'" + valores["CORPCLIENTE"]  + "'"; query += ",";
	    query += "faltacliente = "; query += valores["FALTACLIENTE"] == "" ? "NULL" : "'" + valores["FALTACLIENTE"]  + "'"; query += ",";
	    query += "fbajacliente = "; query += valores["FBAJACLIENTE"] == "" ? "NULL" : "'" + valores["FBAJACLIENTE"]  + "'"; query += ",";
	    query += "regimenfiscalcliente = "; query += valores["REGIMENFISCALCLIENTE"] == "" ? "NULL" : "'" + valores["REGIMENFISCALCLIENTE"]  + "'"; query += ",";
	    query += "comentcliente = "; query += valores["COMENTCLIENTE"] == "" ? "NULL" : "'" + valores["COMENTCLIENTE"]  + "'"; query += ",";
	    query += "ecommercedatacliente = "; query += valores["ECOMMERCEDATACLIENTE"] == "" ? "NULL" : "'" + valores["ECOMMERCEDATACLIENTE"]  + "'"; query += ",";
	    query += "inactivocliente = "; query += valores["INACTIVOCLIENTE"] == "" ? "NULL" : "'" + valores["INACTIVOCLIENTE"]  + "'"; query += ",";
	    query += "recargoeqcliente = "; query += valores["RECARGOEQCLIENTE"] == "" ? "NULL" : "'" + valores["RECARGOEQCLIENTE"]  + "'"; query += ",";
	    query += "idforma_pago = "; query += valores["IDFORMA_PAGO"] == "" ? "NULL" : "'" + valores["IDFORMA_PAGO"]  + "'"; query += ",";
	    query += "idprovincia = "; query += valores["IDPROVINCIA"] == "" ? "NULL" : "'" + valores["IDPROVINCIA"]  + "'";
	    query += " WHERE cifcliente = '" + valores["CIFCLIENTE"] + "'";

            dbConnection->runQuery ( query );

	} else {

            /// El cliente no existe, se debe hacer una insercion de este.
            pgimport->mensajeria ( _ ( "<LI> El cliente no existe, se debe hacer una insercion de este</LI>\n" ) );

	    QString query = "INSERT INTO cliente (nomcliente, nomaltcliente, cifcliente, \
	    codcliente, bancocliente, dircliente, poblcliente, cpcliente, \
	    telcliente, teltrabcliente, movilcliente, faxcliente, mailcliente, \
	    urlcliente, corpcliente, faltacliente, fbajacliente, regimenfiscalcliente, \
	    comentcliente, ecommercedatacliente, inactivocliente, recargoeqcliente, \
	    idforma_pago, idprovincia) VALUES (";
	    
	    query += valores["NOMCLIENTE"] == "" ? "NULL" : "'" + valores["NOMCLIENTE"]  + "'"; query += ",";
	    query += valores["NOMALTCLIENTE"] == "" ? "NULL" : "'" + valores["NOMALTCLIENTE"]  + "'"; query += ",";
	    query += valores["CIFCLIENTE"] == "" ? "NULL" : "'" + valores["CIFCLIENTE"]  + "'"; query += ",";
	    query += valores["CODCLIENTE"] == "" ? "NULL" : "'" + valores["CODCLIENTE"]  + "'"; query += ",";
	    query += valores["BANCOCLIENTE"] == "" ? "NULL" : "'" + valores["BANCOCLIENTE"]  + "'"; query += ",";
	    query += valores["DIRCLIENTE"] == "" ? "NULL" : "'" + valores["DIRCLIENTE"]  + "'"; query += ",";
	    query += valores["POBLCLIENTE"] == "" ? "NULL" : "'" + valores["POBLCLIENTE"]  + "'"; query += ",";
	    query += valores["CPCLIENTE"] == "" ? "NULL" : "'" + valores["CPCLIENTE"]  + "'"; query += ",";
	    query += valores["TELCLIENTE"] == "" ? "NULL" : "'" + valores["TELCLIENTE"]  + "'"; query += ",";
	    query += valores["TELTRABCLIENTE"] == "" ? "NULL" : "'" + valores["TELTRABCLIENTE"]  + "'"; query += ",";
	    query += valores["MOVILCLIENTE"] == "" ? "NULL" : "'" + valores["MOVILCLIENTE"]  + "'"; query += ",";
	    query += valores["FAXCLIENTE"] == "" ? "NULL" : "'" + valores["FAXCLIENTE"]  + "'"; query += ",";
	    query += valores["MAILCLIENTE"] == "" ? "NULL" : "'" + valores["MAILCLIENTE"]  + "'"; query += ",";
	    query += valores["URLCLIENTE"] == "" ? "NULL" : "'" + valores["URLCLIENTE"]  + "'"; query += ",";
	    query += valores["CORPCLIENTE"] == "" ? "NULL" : "'" + valores["CORPCLIENTE"]  + "'"; query += ",";
	    query += valores["FALTACLIENTE"] == "" ? "NULL" : "'" + valores["FALTACLIENTE"]  + "'"; query += ",";
	    query += valores["FBAJACLIENTE"] == "" ? "NULL" : "'" + valores["FBAJACLIENTE"]  + "'"; query += ",";
	    query += valores["REGIMENFISCALCLIENTE"] == "" ? "NULL" : "'" + valores["REGIMENFISCALCLIENTE"]  + "'"; query += ",";
	    query += valores["COMENTCLIENTE"] == "" ? "NULL" : "'" + valores["COMENTCLIENTE"]  + "'"; query += ",";
	    query += valores["ECOMMERCEDATACLIENTE"] == "" ? "NULL" : "'" + valores["ECOMMERCEDATACLIENTE"]  + "'"; query += ",";
	    query += valores["INACTIVOCLIENTE"] == "" ? "NULL" : "'" + valores["INACTIVOCLIENTE"]  + "'"; query += ",";
	    query += valores["RECARGOEQCLIENTE"] == "" ? "NULL" : "'" + valores["RECARGOEQCLIENTE"]  + "'"; query += ",";
	    query += valores["IDFORMA_PAGO"] == "" ? "NULL" : "'" + valores["IDFORMA_PAGO"]  + "'"; query += ",";
	    query += valores["IDPROVINCIA"] == "" ? "NULL" : "'" + valores["IDPROVINCIA"]  + "'";
	    query += ")";
	    
            dbConnection->runQuery ( query );

	} // end if
	
        delete cur;
	
    } // end if
    
    pgimport->mensajeria ( "<HR>" );
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataProveedor()
{
    BL_FUNC_DEBUG

    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando el proveedor" ) + " " + valores["CIFPROVEEDOR"] + "</b><BR>" );
    /// En la importacion de un proveedor hay que hacer la comprobacion del DNI para saber si existe o no.
    QString cifprov = valores["CIFPROVEEDOR"];
    if ( cifprov != "" ) {
        QString query = "SELECT * FROM proveedor WHERE cifproveedor SIMILAR TO '" + cifprov + "'";
        BlDbRecordSet *cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            /// El cliente ya existe se pueden hacer modificaciones.
            pgimport->mensajeria ( "<LI>" + _ ( "El proveedor ya existe, se pueden hacer modificaciones" ) + "<LI>\n" );
            QString query1 = "UPDATE proveedor SET nomproveedor = '" + valores["NOMPROVEEDOR"] + "' WHERE cifproveedor = '" + valores["CIFPROVEEDOR"] + "'";
            dbConnection->runQuery ( query1 );
        } else {
            /// El cliente no existe, se debe hacer una insercion de este.
            pgimport->mensajeria ( "<LI>" +  _ ( "El proveedor no existe, se debe hacer una insercion de este" ) + "<LI>\n" );
            QString query1 = "INSERT INTO proveedor (cifproveedor, nomproveedor) VALUES ('" + valores["CIFPROVEEDOR"] + "','" + valores["NOMPROVEEDOR"] + "')";
            dbConnection->runQuery ( query1 );
        } // end if
        delete cur;
    } // end if
    pgimport->mensajeria ( "<HR>" );
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataFormaPago()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<BR><B>" + _ ( "Tratando la forma de pago" ) + " " + valores["DESCFORMA_PAGO"] + "</B><BR>" );
    QString idforma_pago = valores["IDFORMA_PAGO"];
    QString descforma_pago = valores["DESCFORMA_PAGO"];
    QString dias1tforma_pago = valores["DIAS1TFORMA_PAGO"];
    QString descuentoforma_pago = valores["DESCUENTOFORMA_PAGO"];

    /// Comprobamos que hayan datos suficientes.
    if ( descforma_pago == "" || dias1tforma_pago == "" || descuentoforma_pago == "" ) {
        pgimport->mensajeria ( "<LI>" + _ ( "Datos insuficientes para tratar la forma de pago." ) );
        valores.clear();
        return 1;
    } // end if

    /// Comprobamos que no este ya creada una forma de pago de este tipo.
    QString query = "SELECT * FROM forma_pago WHERE dias1tforma_pago = " + dias1tforma_pago + " AND descuentoforma_pago = " + descuentoforma_pago;
    BlDbRecordSet *cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        pgimport->mensajeria ( "<LI>" + _ ( "Ya existe esta forma de pago." ) );
        delete cur;
        valores.clear();
        return 1;
    } // end if
    delete cur;

    /// Hacemos la insercion de la forma de Pago.
    query = "INSERT INTO forma_pago (descforma_pago, dias1tforma_pago, descuentoforma_pago) VALUES ('" + descforma_pago + "'," + dias1tforma_pago + "," + descuentoforma_pago + ")";
    dbConnection->runQuery ( query );
    pgimport->mensajeria ( "<LI>" + _ ( "Forma de pago" ) + " <B>" + descforma_pago + "</B> " + _ ( "Insertada" ) );
    pgimport->mensajeria ( "<HR>" );
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataAlmacen()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de almacen que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando almacen" ) + " " + valores["CODIGOALMACEN"] + " " + valores["NOMALMACEN"] + "</B><BR>" );
    /// Primero hacemos la recoleccion de valores.
    QString idalmacen = valores["IDALMACEN"];
    QString codigoalmacen = valores["CODIGOALMACEN"];
    QString nomalmacen = valores["NOMALMACEN"];
    QString diralmacen = valores["DIRALMACEN"];
    QString poblalmacen = valores["POBLALMACEN"];
    QString cpalmacen = valores["CPALMACEN"];
    QString telalmacen = valores["TELALMACEN"];
    QString faxalmacen = valores["FAXALMACEN"];
    QString emailalmacen = valores["EMAILALMACEN"];
    QString inactivoalmacen = valores["INACTIVOALMACEN"];
    /// Comprobamos que hayan suficientes datos para procesar.
    if ( codigoalmacen == "" || nomalmacen == "" ) {
        pgimport->mensajeria ( "<LI>" + _ ( "Datos insuficientes para tratar el almacen." ) );
        valores.clear();
        return 1;
    } // end if
    /// Comprobamos que no este ya creada un almacen de este tipo.
    QString query = "SELECT * FROM almacen WHERE codigoalmacen = '" + codigoalmacen + "'";
    BlDbRecordSet *cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        pgimport->mensajeria ( "<LI>" + _ ( "Ya existe este almacen." ) );
        delete cur;
        valores.clear();
        return 1;
    } // end if
    delete cur;
    /// Hacemos la insercion del almacen.
    query = "INSERT INTO almacen (codigoalmacen, nomalmacen, diralmacen) VALUES ('" + codigoalmacen + "','" + nomalmacen + "','" + diralmacen + "')";
    dbConnection->runQuery ( query );
    pgimport->mensajeria ( "<LI> " + _ ( "Almacen" ) + " <B>" + codigoalmacen + "</B> " + _ ( "insertado" ) );
    /// Finalizamos.
    pgimport->mensajeria ( "<HR>" );
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataFamilia()
{
    BL_FUNC_DEBUG
    QString query;
    BlDbRecordSet *cur;
    /// En el XML se ha encontrado un tag de almacen que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando familia" ) + " " + valores["CODIGOCOMPLETOFAMILIA"] + " " + valores["NOMBREFAMILIA"] + "</B><BR>" );
    /// Primero hacemos la recoleccion de valores.
    QString idfamilia = valores["IDFAMILIA"];
    QString codigofamilia = valores["CODIGOFAMILIA"];
    QString nombrefamilia = valores["NOMBREFAMILIA"];
    QString descfamilia = valores["DESCFAMILIA"];
    QString padrefamilia = valores["PADREFAMILIA"];
    QString codigocompletofamilia = valores["CODIGOCOMPLETOFAMILIA"];
    /// Comprobamos que hayan suficientes datos para procesar.
    /// Comprobamos quien es el padre de la familia.
    QString codigopadre = codigocompletofamilia.left ( codigocompletofamilia.length() - codigofamilia.length() );
    QString idpadre = "";
    if ( codigopadre == "" ) {
        idpadre = "NULL";
    } else {
        query = "SELECT * FROM familia WHERE codigocompletofamilia = '" + codigopadre + "'";
        cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            idpadre = cur->value( "idfamilia" );
        } else {
            pgimport->mensajeria ( "<P> " + _ ( "No se ha encontrado el padre de esta familia." ) + "</P>" );
        } // end if
        delete cur;
    } // end if
    if ( codigocompletofamilia == "" || nombrefamilia == "" || idpadre == "" ) {
        pgimport->mensajeria ( "<P>" + _ ( "Datos insuficientes para tratar la familia." ) + "</P>" );
        valores.clear();
        return 1;
    } // end if
    /// Comprobamos que no este ya creada un familia de este tipo.
    query = "SELECT * FROM familia WHERE codigocompletofamilia = '" + codigocompletofamilia + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        pgimport->mensajeria ( "<P>" + _ ( "Ya existe esta familia." ) + "</P>" );
        delete cur;
        valores.clear();
        return 1;
    } // end if
    delete cur;
    /// Hacemos la insercion del familia.
    query = "INSERT INTO familia (codigofamilia, nombrefamilia, padrefamilia) VALUES ('" + codigofamilia + "','" + nombrefamilia + "'," + idpadre + ")";
    dbConnection->runQuery ( query );
    pgimport->mensajeria ( "<P>" + _ ( "Familia" ) + " <B>" + codigocompletofamilia + "</B> " + _ ( "insertada" ) + "</P>" );
    /// Finalizamos.
    pgimport->mensajeria ( "<HR>" );
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataArticulo()
{
    BL_FUNC_DEBUG
    QString query;
    BlDbRecordSet *cur;
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando articulo" ) + " " + valores["CODIGOCOMPLETOARTICULO"] + " </B><BR>" );
    /// Primero hacemos la recoleccion de valores.
    QString idarticulo = valores["IDARTICULO"];
    QString codarticulo =  valores["CODARTICULO"];
    QString nomarticulo = valores["NOMARTICULO"];
    QString abrevarticulo = valores["ABREVARTICULO"];
    QString obserarticulo = valores["OBSERARTICULO"];
    QString presentablearticulo = valores["PRESENTABLEARTICULO"];
    QString controlstockarticulo = valores["CONTROLSTOCKARTICULO"];
    QString idtipo_articulo = valores["IDTIPO_ARTICULO"];
    QString idtipo_iva = valores["IDTIPO_IVA"];
    QString codigocompletoarticulo = valores["CODIGOCOMPLETOARTICULO"];
    QString idfamilia = valores["IDFAMILIA"];
    QString stockarticulo = valores["STOCKARTICULO"];
    QString inactivoarticulo = valores["INACTIVOARTICULO"];
    QString pvparticulo = valores["PVPARTICULO"];
    QString codigocompletofamilia = valores["CODIGOCOMPLETOFAMILIA"];
    QString nombrefamilia = valores["NOMBREFAMILIA"];
    QString codtipo_articulo = valores["CODTIPO_ARTICULO"];
    QString desctipo_articulo = valores["DESCTIPO_ARTICULO"];
    QString desctipo_iva = valores["DESCTIPO_IVA"];

    query = "SELECT * FROM familia WHERE codigocompletofamilia = '" + codigocompletofamilia + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idfamilia = cur->value( "idfamilia" );
    } else {
        pgimport->mensajeria ( _ ( "La familia del articulo no existe" ) + "<BR>\n" );
        idfamilia = "";
    } // end if
    delete cur;

    query = "SELECT * from tipo_articulo WHERE codtipo_articulo = '" + codtipo_articulo + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idtipo_articulo = cur->value( "idtipo_articulo" );
    } else {
        idtipo_articulo = "NULL";
    } // end if
    delete cur;

    query = "SELECT * from tipo_iva WHERE desctipo_iva = '" + desctipo_iva + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idtipo_iva = cur->value( "idtipo_iva" );
    } else {
        idtipo_iva = "NULL";
    } // end if
    delete cur;

    /// Comprobamos que hayan suficientes datos para procesar.
    if ( codigocompletoarticulo == "" || nomarticulo == "" || idfamilia == "" ) {
        pgimport->mensajeria ( "<P>" + _ ( "Datos insuficientes para tratar el articulo." ) + "</P>" );
        valores.clear();
        return 1;
    } // end if

    /// Comprobamos que no este ya creada un articulo de este tipo.
    query = "SELECT * FROM articulo WHERE codigocompletoarticulo = '" + codigocompletoarticulo + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        pgimport->mensajeria ( "<P>" + _ ( "Ya existe este articulo." ) + "</P>" );
        delete cur;
        valores.clear();
        return 1;
    } // end if
    delete cur;

    /// Hacemos la insercion del articulo.
    query = "INSERT INTO articulo (codarticulo, nomarticulo, abrevarticulo, idfamilia, pvparticulo, idtipo_articulo, obserarticulo, presentablearticulo, inactivoarticulo, controlstockarticulo, idtipo_iva) VALUES (";
    query += "'" + codarticulo + "'";
    query += ",'" + nomarticulo + "'";
    query += ",'" + abrevarticulo;
    query += "'," + idfamilia;
    query += "," + pvparticulo;
    query += "," + idtipo_articulo;
    query += ",'" + obserarticulo + "'";
    query += ",'" + presentablearticulo + "'";
    query += ",'" + inactivoarticulo + "'";
    query += ",'" + controlstockarticulo + "'";
    query += "," + idtipo_iva;
    query += ")";
    dbConnection->runQuery ( query );
    pgimport->mensajeria ( "<P>" + _ ( "Articulo" ) + " <B>" + codigocompletoarticulo + "</B> " + _ ( "insertado" ) + "</P>" );
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataLPedidoCliente()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando LPedidoCliente" ) + " " + valores["IDLPEDIDOCLIENTE"] + "</B><BR>" );
    tvalores lpedidoclientemap;
    lpedidoclientemap.insert ( "IDLPEDIDOCLIENTE", valores["IDLPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "DESCLPEDIDOCLIENTE", valores["DESCLPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "CANTLPEDIDOCLIENTE", valores["CANTLPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "PVPLPEDIDOCLIENTE", valores["PVPLPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "IVALPEDIDOCLIENTE", valores["IVALPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "DESCUENTOLPEDIDOCLIENTE", valores["DESCUENTOLPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "IDPEDIDOCLIENTE", valores["IDPEDIDOCLIENTE"] );
    lpedidoclientemap.insert ( "IDARTICULO", valores["IDARTICULO"] );
    lpedidoclientemap.insert ( "CODARTICULO", valores["CODARTICULO"] );
    lpedidoclientemap.insert ( "NOMARTICULO", valores["NOMARTICULO"] );
    lpedidoclientemap.insert ( "ABREVARTICULO", valores["ABREVARTICULO"] );
    lpedidoclientemap.insert ( "CODIGOCOMPLETOARTICULO", valores["CODIGOCOMPLETOARTICULO"] );
    listalpedidocliente.append ( lpedidoclientemap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataDPedidoCliente()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando DPedidoCliente" ) + " " + valores["IDDPEDIDOCLIENTE"] + "</B><BR>" );
    tvalores dpedidoclientemap;
    dpedidoclientemap.insert ( "IDDPEDIDOCLIENTE", valores["IDDPEDIDOCLIENTE"] );
    dpedidoclientemap.insert ( "CONCEPTDPEDIDOCLIENTE", valores["CONCEPTDPEDIDOCLIENTE"] );
    dpedidoclientemap.insert ( "PROPORCIONDPEDIDOCLIENTE", valores["PROPORCIONDPEDIDOCLIENTE"] );
    listadpedidocliente.append ( dpedidoclientemap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataPedidoCliente()
{
    BL_FUNC_DEBUG
    QString query;
    BlDbRecordSet *cur;
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando pedido cliente" ) + " " + valores["IDPEDIDOCLIENTE"] + " " + valores["NOMBREFAMILIA"] + "</B><BR>" );
    QString idpedidocliente = valores["IDPEDIDOCLIENTE"];
    QString numpedidocliente = valores["NUMPEDIDOCLIENTE"];
    QString refpedidocliente = valores["REFPEDIDOCLIENTE"];
    QString fechapedidocliente = valores["FECHAPEDIDOCLIENTE"];
    QString descpedidocliente = valores["DESCPEDIDOCLIENTE"];
    QString idalmacen = valores["IDALMACEN"];
    QString comentpedidocliente = valores["COMENTPEDIDOCLIENTE"];
    QString procesadopedidocliente = valores["PROCESADOPEDIDOCLIENTE"];
    QString contactpedidocliente = valores["CONTACTPEDIDOCLIENTE"];
    QString telpedidocliente = valores["TELPEDIDOCLIENTE"];
    QString idusuari = valores["IDUSUARI"];
    QString idcliente = valores["IDCLIENTE"];
    QString idforma_pago = valores["IDFORMA_PAGO"];
    QString idtrabajador = valores["IDTRABAJADOR"];
    QString codigoalmacen = valores["CODIGOALMACEN"];
    QString nomalmacen = valores["NOMALMACEN"];
    QString diralmacen = valores["DIRALMACEN"];
    QString poblalmacen = valores["POBLALMACEN"];
    QString cpalmacen = valores["CPALMACEN"];
    QString telalmacen = valores["TELALMACEN"];
    QString nomcliente = valores["NOMCLIENTE"];
    QString nomaltcliente = valores["NOMALTCLIENTE"];
    QString cifcliente = valores["CIFCLIENTE"];
    QString bancocliente = valores["BANCOCLIENTE"];
    QString dircliente = valores["DIRCLIENTE"];
    QString poblcliente = valores["POBLCLIENTE"];
    QString cpcliente = valores["CPCLIENTE"];
    QString telcliente = valores["TELCLIENTE"];
    QString faxcliente = valores["FAXCLIENTE"];
    QString mailcliente = valores["MAILCLIENTE"];
    QString urlcliente = valores["URLCLIENTE"];
    QString provcliente = valores["PROVCLIENTE"];
    QString nomtrabajador = valores["NOMTRABAJADOR"];
    QString apellidostrabajador = valores["APELLIDOSTRABAJADOR"];
    QString dirtrabajador = valores["DIRTRABAJADOR"];
    QString nsstrabajador = valores["NSSTRABAJADOR"];
    QString teltrabajador = valores["TELTRABAJADOR"];
    QString moviltrabajador = valores["MOVILTRABAJADOR"];
    QString emailtrabajador = valores["EMAILTRABAJADOR"];
    QString descforma_pago = valores["DESCFORMA_PAGO"];
    QString dias1tforma_pago = valores["DIAS1TFORMA_PAGO"];
    QString descuentoforma_pago = valores["DESCUENTOFORMA_PAGO"];

    /// Completamos datos que puedan faltar.
    query = "SELECT * FROM almacen WHERE codigoalmacen = '" + codigoalmacen + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idalmacen = cur->value( "idalmacen" );
    } else {
        pgimport->mensajeria ( _ ( "El almacen del presupuesto no existe" ) + "<BR>\n" );
        idalmacen = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM cliente WHERE cifcliente = '" + cifcliente + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idcliente = cur->value( "idcliente" );
    } else {
        pgimport->mensajeria ( _ ( "El cliente del presupuesto no existe" ) + "<BR>\n" );
        idcliente = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM forma_pago WHERE descforma_pago = '" + descforma_pago + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idforma_pago = cur->value( "idforma_pago" );
    } else {
        pgimport->mensajeria ( _ ( "La forma de pago del presupuesto no existe" ) + "<BR>\n" );
        idforma_pago = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM trabajador WHERE nomtrabajador = '" + nomtrabajador + "' AND nsstrabajador = '" + nsstrabajador + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idtrabajador = cur->value( "idtrabajador" );
    } else {
        pgimport->mensajeria ( _ ( "El empleado del presupuesto no existe" ) + "<BR>\n" );
        idtrabajador = "NULL";
    } // end if
    delete cur;

    /// Hacemos la insercion.
    query = " INSERT INTO pedidocliente (numpedidocliente, refpedidocliente, fechapedidocliente, descpedidocliente, contactpedidocliente, telpedidocliente, comentpedidocliente, idusuari, procesadopedidocliente, idcliente, idalmacen, idforma_pago, idtrabajador) VALUES (";
    query += "NULL";
    query += ", '" + refpedidocliente + "'";
    query += ", '" + fechapedidocliente + "'";
    query += ", '" + descpedidocliente + "'";
    query += ", '" + contactpedidocliente + "'";
    query += ", '" + telpedidocliente + "'";
    query += ", '" + comentpedidocliente + "'";
    query += ", NULL";
    query += ",'" + procesadopedidocliente + "'";
    query += "," + idcliente;
    query += "," + idalmacen;
    query += "," + idforma_pago;
    query += "," + idtrabajador;
    query += ")";

    dbConnection->begin();
    dbConnection->runQuery ( query );
    cur = dbConnection->loadQuery ( "SELECT MAX(idpedidocliente) AS id FROM pedidocliente" );
    idpedidocliente = cur->value( "id" );
    delete cur;
    dbConnection->commit();
    /// Tratamos la insercion de las lineas de albaran.
    tvalores *lpedidoclientemap;

    QMutableListIterator<tvalores> ilistalpedidocliente ( listalpedidocliente );

    for ( lpedidoclientemap = &listalpedidocliente.first(); lpedidoclientemap; lpedidoclientemap = &ilistalpedidocliente.next() ) {
        QString idlpedidocliente = ( *lpedidoclientemap ) ["IDLPEDIDOCLIENTE"];
        QString desclpedidocliente = ( *lpedidoclientemap ) ["DESCLPEDIDOCLIENTE"];
        QString cantlpedidocliente = ( *lpedidoclientemap ) ["CANTLPEDIDOCLIENTE"];
        QString pvplpedidocliente = ( *lpedidoclientemap ) ["PVPLPEDIDOCLIENTE"];
        QString ivalpedidocliente = ( *lpedidoclientemap ) ["IVALPEDIDOCLIENTE"];
        QString descuentolpedidocliente = ( *lpedidoclientemap ) ["DESCUENTOLPEDIDOCLIENTE"];
        QString idarticulo = ( *lpedidoclientemap ) ["IDARTICULO"];
        QString codarticulo = ( *lpedidoclientemap ) ["CODARTICULO"];
        QString nomarticulo = ( *lpedidoclientemap ) ["NOMARTICULO"];
        QString abrevarticulo = ( *lpedidoclientemap ) ["ABREVARTICULO"];
        QString codigocompletoarticulo = ( *lpedidoclientemap ) ["CODIGOCOMPLETOARTICULO"];
        /// Completamos datos de la linea de presupuesto.
        query = "SELECT * FROM articulo WHERE codigocompletoarticulo = '" + codigocompletoarticulo + "'";
        cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            idarticulo = cur->value( "idarticulo" );
        } else {
            pgimport->mensajeria ( _ ( "El articulo de la linea de presupuesto no existe" ) + "<BR>\n" );
            idarticulo = "NULL";
        } // end if
        delete cur;
        /// Hacemos la insercion.
        query = " INSERT INTO lpedidocliente (desclpedidocliente, cantlpedidocliente, pvplpedidocliente, ivalpedidocliente, descuentolpedidocliente, idpedidocliente, idarticulo) VALUES (";
        query += "'" + desclpedidocliente + "'";
        query += ", " + cantlpedidocliente;
        query += ", " + pvplpedidocliente;
        query += ", " + ivalpedidocliente;
        query += ", " + descuentolpedidocliente;
        query += ", " + idpedidocliente;
        query += ", " + idarticulo;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    /// Tratamos la insercion de los descuentos de presupuesto.
    tvalores *dpedidoclientemap;

    QMutableListIterator<tvalores> ilistadpedidocliente ( listadpedidocliente );

    for ( dpedidoclientemap = &listadpedidocliente.first(); dpedidoclientemap; dpedidoclientemap = &ilistadpedidocliente.next() ) {
        QString iddpedidocliente = ( *dpedidoclientemap ) ["IDDPEDIDOCLIENTE"];
        QString conceptdpedidocliente = ( *dpedidoclientemap ) ["CONCEPTDPEDIDOCLIENTE"];
        QString proporciondpedidocliente = ( *dpedidoclientemap ) ["PROPORCIONDPEDIDOCLIENTE"];
        /// Hacemos la insercion.
        query = " INSERT INTO dpedidocliente (conceptdpedidocliente, proporciondpedidocliente, idpedidocliente) VALUES (";
        query += "'" + conceptdpedidocliente + "'";
        query += ", " + proporciondpedidocliente;
        query += ", " + idpedidocliente;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataLAlbaran()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando LAlbaran" ) + " " + valores["IDLALBARAN"] + "</B><BR>" );
    tvalores lalbaranmap;
    lalbaranmap.insert ( "IDLALBARAN", valores["IDLALBARAN"] );
    lalbaranmap.insert ( "DESCLALBARAN", valores["DESCLALBARAN"] );
    lalbaranmap.insert ( "CANTLALBARAN", valores["CANTLALBARAN"] );
    lalbaranmap.insert ( "PVPLALBARAN", valores["PVPLALBARAN"] );
    lalbaranmap.insert ( "IVALALBARAN", valores["IVALALBARAN"] );
    lalbaranmap.insert ( "DESCUENTOLALBARAN", valores["DESCUENTOLALBARAN"] );
    lalbaranmap.insert ( "IDALBARAN", valores["IDALBARAN"] );
    lalbaranmap.insert ( "IDARTICULO", valores["IDARTICULO"] );
    lalbaranmap.insert ( "CODARTICULO", valores["CODARTICULO"] );
    lalbaranmap.insert ( "NOMARTICULO", valores["NOMARTICULO"] );
    lalbaranmap.insert ( "ABREVARTICULO", valores["ABREVARTICULO"] );
    lalbaranmap.insert ( "CODIGOCOMPLETOARTICULO", valores["CODIGOCOMPLETOARTICULO"] );
    listalalbaran.append ( lalbaranmap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataDAlbaran()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando DAlbaran" ) + " " + valores["IDDALBARAN"] + "</B><BR>" );
    tvalores dalbaranmap;
    dalbaranmap.insert ( "IDDALBARAN", valores["IDDALBARAN"] );
    dalbaranmap.insert ( "CONCEPTDALBARAN", valores["CONCEPTDALBARAN"] );
    dalbaranmap.insert ( "PROPORCIONDALBARAN", valores["PROPORCIONDALBARAN"] );
    listadalbaran.append ( dalbaranmap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataAlbaran()
{
    BL_FUNC_DEBUG
    QString query;
    BlDbRecordSet *cur;
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando albaran" ) + " " + valores["IDALBARAN"] + " " + valores["NOMBREFAMILIA"] + "</B><BR>" );
    QString idalbaran = valores["IDALBARAN"];
    QString numalbaran = valores["NUMALBARAN"];
    QString refalbaran = valores["REFALBARAN"];
    QString fechaalbaran = valores["FECHAALBARAN"];
    QString descalbaran = valores["DESCALBARAN"];
    QString idalmacen = valores["IDALMACEN"];
    QString contactfactura = valores["CONTACTFACTURA"];
    QString telfactura = valores["TELFACTURA"];
    QString comentalbaran = valores["COMENTALBARAN"];
    QString procesadoalbaran = valores["PROCESADOALBARAN"];
    QString contactalbaran = valores["CONTACTALBARAN"];
    QString telalbaran = valores["TELALBARAN"];
    QString idusuari = valores["IDUSUARI"];
    QString idcliente = valores["IDCLIENTE"];
    QString idforma_pago = valores["IDFORMA_PAGO"];
    QString idtrabajador = valores["IDTRABAJADOR"];
    QString codigoalmacen = valores["CODIGOALMACEN"];
    QString nomalmacen = valores["NOMALMACEN"];
    QString diralmacen = valores["DIRALMACEN"];
    QString poblalmacen = valores["POBLALMACEN"];
    QString cpalmacen = valores["CPALMACEN"];
    QString telalmacen = valores["TELALMACEN"];
    QString nomcliente = valores["NOMCLIENTE"];
    QString nomaltcliente = valores["NOMALTCLIENTE"];
    QString cifcliente = valores["CIFCLIENTE"];
    QString bancocliente = valores["BANCOCLIENTE"];
    QString dircliente = valores["DIRCLIENTE"];
    QString poblcliente = valores["POBLCLIENTE"];
    QString cpcliente = valores["CPCLIENTE"];
    QString telcliente = valores["TELCLIENTE"];
    QString faxcliente = valores["FAXCLIENTE"];
    QString mailcliente = valores["MAILCLIENTE"];
    QString urlcliente = valores["URLCLIENTE"];
    QString provcliente = valores["PROVCLIENTE"];
    QString nomtrabajador = valores["NOMTRABAJADOR"];
    QString apellidostrabajador = valores["APELLIDOSTRABAJADOR"];
    QString dirtrabajador = valores["DIRTRABAJADOR"];
    QString nsstrabajador = valores["NSSTRABAJADOR"];
    QString teltrabajador = valores["TELTRABAJADOR"];
    QString moviltrabajador = valores["MOVILTRABAJADOR"];
    QString emailtrabajador = valores["EMAILTRABAJADOR"];
    QString descforma_pago = valores["DESCFORMA_PAGO"];
    QString dias1tforma_pago = valores["DIAS1TFORMA_PAGO"];
    QString descuentoforma_pago = valores["DESCUENTOFORMA_PAGO"];

    /// Completamos datos que puedan faltar.
    query = "SELECT * FROM almacen WHERE codigoalmacen = '" + codigoalmacen + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idalmacen = cur->value( "idalmacen" );
    } else {
        pgimport->mensajeria ( _ ( "El almacen del presupuesto no existe" ) + "<BR>\n" );
        idalmacen = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM cliente WHERE cifcliente = '" + cifcliente + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idcliente = cur->value( "idcliente" );
    } else {
        pgimport->mensajeria ( _ ( "El cliente del presupuesto no existe" ) + "<BR>\n" );
        idcliente = "NULL";
    } // end if
    delete cur;
    query = "SELECT * FROM forma_pago WHERE descforma_pago = '" + descforma_pago + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idforma_pago = cur->value( "idforma_pago" );
    } else {
        pgimport->mensajeria ( _ ( "La forma de pago del presupuesto no existe" ) + "<BR>\n" );
        idforma_pago = "NULL";
    } // end if
    delete cur;
    query = "SELECT * FROM trabajador WHERE nomtrabajador = '" + nomtrabajador + "' AND nsstrabajador = '" + nsstrabajador + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idtrabajador = cur->value( "idtrabajador" );
    } else {
        pgimport->mensajeria ( _ ( "El empleado del presupuesto no existe" ) + "<BR>\n" );
        idtrabajador = "NULL";
    } // end if
    delete cur;
    /// Hacemos la insercion.
    query = " INSERT INTO albaran (numalbaran, refalbaran, fechaalbaran, descalbaran, contactalbaran , telalbaran  , comentalbaran , idusuari, procesadoalbaran, idcliente, idalmacen, idforma_pago, idtrabajador) VALUES (";
    query += "NULL";
    query += ", '" + refalbaran + "'";
    query += ", '" + fechaalbaran + "'";
    query += ", '" + descalbaran + "'";
    query += ", '" + contactalbaran + "'";
    query += ", '" + telalbaran + "'";
    query += ", '" + comentalbaran + "'";
    query += ", NULL";
    query += ",'" + procesadoalbaran + "'";
    query += "," + idcliente;
    query += "," + idalmacen;
    query += "," + idforma_pago;
    query += "," + idtrabajador;
    query += ")";

    dbConnection->begin();
    dbConnection->runQuery ( query );
    cur = dbConnection->loadQuery ( "SELECT MAX(idalbaran) AS id FROM albaran" );
    idalbaran = cur->value( "id" );
    delete cur;
    dbConnection->commit();
    /// Tratamos la insercion de las lineas de albaran.
    tvalores *lalbaranmap;

    QMutableListIterator<tvalores> ilistalalbaran ( listalalbaran );

    for ( lalbaranmap = &listalalbaran.first(); lalbaranmap; lalbaranmap = &ilistalalbaran.next() ) {
        QString idlalbaran = ( *lalbaranmap ) ["IDLALBARAN"];
        QString desclalbaran = ( *lalbaranmap ) ["DESCLALBARAN"];
        QString cantlalbaran = ( *lalbaranmap ) ["CANTLALBARAN"];
        QString pvplalbaran = ( *lalbaranmap ) ["PVPLALBARAN"];
        QString ivalalbaran = ( *lalbaranmap ) ["IVALALBARAN"];
        QString descuentolalbaran = ( *lalbaranmap ) ["DESCUENTOLALBARAN"];
        QString idarticulo = ( *lalbaranmap ) ["IDARTICULO"];
        QString codarticulo = ( *lalbaranmap ) ["CODARTICULO"];
        QString nomarticulo = ( *lalbaranmap ) ["NOMARTICULO"];
        QString abrevarticulo = ( *lalbaranmap ) ["ABREVARTICULO"];
        QString codigocompletoarticulo = ( *lalbaranmap ) ["CODIGOCOMPLETOARTICULO"];
        /// Completamos datos de la linea de presupuesto.
        query = "SELECT * FROM articulo WHERE codigocompletoarticulo = '" + codigocompletoarticulo + "'";
        cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            idarticulo = cur->value( "idarticulo" );
        } else {
            pgimport->mensajeria ( _ ( "El articulo de la linea de presupuesto no existe" ) + "<BR>\n" );
            idarticulo = "NULL";
        } // end if
        delete cur;
        /// Hacemos la insercion.
        query = " INSERT INTO lalbaran(desclalbaran, cantlalbaran, pvplalbaran, ivalalbaran, descuentolalbaran, idalbaran, idarticulo) VALUES (";
        query += "'" + desclalbaran + "'";
        query += ", " + cantlalbaran;
        query += ", " + pvplalbaran;
        query += ", " + ivalalbaran;
        query += ", " + descuentolalbaran;
        query += ", " + idalbaran;
        query += ", " + idarticulo;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    /// Tratamos la insercion de los descuentos de presupuesto.
    tvalores *dalbaranmap;

    QMutableListIterator<tvalores> ilistadalbaran ( listadalbaran );

    for ( dalbaranmap = &listadalbaran.first(); dalbaranmap; dalbaranmap = &ilistadalbaran.next() ) {
        QString iddalbaran = ( *dalbaranmap ) ["IDDALBARAN"];
        QString conceptdalbaran = ( *dalbaranmap ) ["CONCEPTDALBARAN"];
        QString proporciondalbaran = ( *dalbaranmap ) ["PROPORCIONDALBARAN"];
        /// Hacemos la insercion.
        query = " INSERT INTO dalbaran(conceptdalbaran, proporciondalbaran, idalbaran) VALUES (";
        query += "'" + conceptdalbaran + "'";
        query += ", " + proporciondalbaran;
        query += ", " + idalbaran;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataLFactura()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando LFactura" ) + " " + valores["IDLFACTURA"] + "</B><BR>" );
    tvalores lfacturamap;
    lfacturamap.insert ( "IDLFACTURA", valores["IDLFACTURA"] );
    lfacturamap.insert ( "DESCLFACTURA", valores["DESCLFACTURA"] );
    lfacturamap.insert ( "CANTLFACTURA", valores["CANTLFACTURA"] );
    lfacturamap.insert ( "PVPLFACTURA", valores["PVPLFACTURA"] );
    lfacturamap.insert ( "IVALFACTURA", valores["IVALFACTURA"] );
    lfacturamap.insert ( "DESCUENTOLFACTURA", valores["DESCUENTOLFACTURA"] );
    lfacturamap.insert ( "IDFACTURA", valores["IDFACTURA"] );
    lfacturamap.insert ( "IDARTICULO", valores["IDARTICULO"] );
    lfacturamap.insert ( "CODARTICULO", valores["CODARTICULO"] );
    lfacturamap.insert ( "NOMARTICULO", valores["NOMARTICULO"] );
    lfacturamap.insert ( "ABREVARTICULO", valores["ABREVARTICULO"] );
    lfacturamap.insert ( "CODIGOCOMPLETOARTICULO", valores["CODIGOCOMPLETOARTICULO"] );
    listalfactura.append ( lfacturamap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataDFactura()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando DFactura" ) + " " + valores["IDDFACTURA"] + "</B><BR>" );
    tvalores dfacturamap;
    dfacturamap.insert ( "IDDFACTURA", valores["IDDFACTURA"] );
    dfacturamap.insert ( "CONCEPTDFACTURA", valores["CONCEPTDFACTURA"] );
    dfacturamap.insert ( "PROPORCIONDFACTURA", valores["PROPORCIONDFACTURA"] );
    listadfactura.append ( dfacturamap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataFactura()
{
    BL_FUNC_DEBUG
    QString query;
    BlDbRecordSet *cur;
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando factura" ) + " " + valores["IDFACTURA"] + " " + valores["NOMBREFAMILIA"] + "</B><BR>" );
    QString idfactura = valores["IDFACTURA"];
    QString codigoserie_factura = valores["CODIGOSERIE_FACTURA"];
    QString numfactura = valores["NUMFACTURA"];
    QString reffactura = valores["REFFACTURA"];
    QString ffactura = valores["FFACTURA"];
    QString descfactura = valores["DESCFACTURA"];
    QString idalmacen = valores["IDALMACEN"];
    QString contactfactura = valores["CONTACTFACTURA"];
    QString telfactura = valores["TELFACTURA"];
    QString comentfactura = valores["COMENTFACTURA"];
    QString procesadafactura = valores["PROCESADAFACTURA"];
    QString idusuari = valores["IDUSUARI"];
    QString idcliente = valores["IDCLIENTE"];
    QString idforma_pago = valores["IDFORMA_PAGO"];
    QString idtrabajador = valores["IDTRABAJADOR"];
    QString codigoalmacen = valores["CODIGOALMACEN"];
    QString nomalmacen = valores["NOMALMACEN"];
    QString diralmacen = valores["DIRALMACEN"];
    QString poblalmacen = valores["POBLALMACEN"];
    QString cpalmacen = valores["CPALMACEN"];
    QString telalmacen = valores["TELALMACEN"];
    QString nomcliente = valores["NOMCLIENTE"];
    QString nomaltcliente = valores["NOMALTCLIENTE"];
    QString cifcliente = valores["CIFCLIENTE"];
    QString bancocliente = valores["BANCOCLIENTE"];
    QString dircliente = valores["DIRCLIENTE"];
    QString poblcliente = valores["POBLCLIENTE"];
    QString cpcliente = valores["CPCLIENTE"];
    QString telcliente = valores["TELCLIENTE"];
    QString faxcliente = valores["FAXCLIENTE"];
    QString mailcliente = valores["MAILCLIENTE"];
    QString urlcliente = valores["URLCLIENTE"];
    QString provcliente = valores["PROVCLIENTE"];
    QString nomtrabajador = valores["NOMTRABAJADOR"];
    QString apellidostrabajador = valores["APELLIDOSTRABAJADOR"];
    QString dirtrabajador = valores["DIRTRABAJADOR"];
    QString nsstrabajador = valores["NSSTRABAJADOR"];
    QString teltrabajador = valores["TELTRABAJADOR"];
    QString moviltrabajador = valores["MOVILTRABAJADOR"];
    QString emailtrabajador = valores["EMAILTRABAJADOR"];
    QString descforma_pago = valores["DESCFORMA_PAGO"];
    QString dias1tforma_pago = valores["DIAS1TFORMA_PAGO"];
    QString descuentoforma_pago = valores["DESCUENTOFORMA_PAGO"];

    /// Completamos datos que puedan faltar.
    query = "SELECT * FROM almacen WHERE codigoalmacen = '" + codigoalmacen + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idalmacen = cur->value( "idalmacen" );
    } else {
        pgimport->mensajeria ( _ ( "El almacen del presupuesto no existe" ) + "<BR>\n" );
        idalmacen = "NULL";
    } // end if
    delete cur;
    query = "SELECT * FROM cliente WHERE cifcliente = '" + cifcliente + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idcliente = cur->value( "idcliente" );
    } else {
        pgimport->mensajeria ( _ ( "El cliente del presupuesto no existe" ) + "<BR>\n" );
        idcliente = "NULL";
    } // end if
    delete cur;
    query = "SELECT * FROM forma_pago WHERE descforma_pago = '" + descforma_pago + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idforma_pago = cur->value( "idforma_pago" );
    } else {
        pgimport->mensajeria ( _ ( "La forma de pago del presupuesto no existe" ) + "<BR>\n" );
        idforma_pago = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM trabajador WHERE nomtrabajador = '" + nomtrabajador + "' AND nsstrabajador = '" + nsstrabajador + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idtrabajador = cur->value( "idtrabajador" );
    } else {
        pgimport->mensajeria ( _ ( "El empleado del presupuesto no existe" ) + "<BR>\n" );
        idtrabajador = "NULL";
    } // end if
    delete cur;

    /// Hacemos comprobaciones de integridad para saber si la factura es insertable o no.
    query = "SELECT * FROM factura WHERE numfactura = '" + numfactura + "' AND codigoserie_factura = '" + codigoserie_factura + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        delete cur;
        pgimport->mensajeria ( _ ( "La factura ya existe y no se va a insertar." ) );
        valores.clear();
        return 1;
    } // end if
    delete cur;

    /// Hacemos la insercion.
    query = " INSERT INTO factura(numfactura, reffactura, ffactura, descfactura, contactfactura, telfactura, comentfactura, idusuari, procesadafactura, idcliente, idalmacen, idforma_pago, idtrabajador, codigoserie_factura) VALUES (";
    query += numfactura;
    query += ", '" + reffactura + "'";
    query += ", '" + ffactura + "'";
    query += ", '" + descfactura + "'";
    query += ", '" + contactfactura + "'";
    query += ", '" + telfactura + "'";
    query += ", '" + comentfactura + "'";
    query += ", NULL";
    query += ",'" + procesadafactura + "'";
    query += "," + idcliente;
    query += "," + idalmacen;
    query += "," + idforma_pago;
    query += "," + idtrabajador;
    query += ",'" + codigoserie_factura + "'";
    query += ")";

    dbConnection->begin();
    dbConnection->runQuery ( query );
    cur = dbConnection->loadQuery ( "SELECT MAX(idfactura) AS id FROM factura" );
    idfactura = cur->value( "id" );
    delete cur;
    dbConnection->commit();
    /// Tratamos la insercion de las lineas de presupuesto.
    tvalores *lfacturamap;

    QMutableListIterator<tvalores> ilistalfactura ( listalfactura );

    for ( lfacturamap = &listalfactura.first(); lfacturamap; lfacturamap = &ilistalfactura.next() ) {
        QString idlfactura = ( *lfacturamap ) ["IDLFACTURA"];
        QString desclfactura = ( *lfacturamap ) ["DESCLFACTURA"];
        QString cantlfactura = ( *lfacturamap ) ["CANTLFACTURA"];
        QString pvplfactura = ( *lfacturamap ) ["PVPLFACTURA"];
        QString ivalfactura = ( *lfacturamap ) ["IVALFACTURA"];
        QString descuentolfactura = ( *lfacturamap ) ["DESCUENTOLFACTURA"];
        QString idarticulo = ( *lfacturamap ) ["IDARTICULO"];
        QString codarticulo = ( *lfacturamap ) ["CODARTICULO"];
        QString nomarticulo = ( *lfacturamap ) ["NOMARTICULO"];
        QString abrevarticulo = ( *lfacturamap ) ["ABREVARTICULO"];
        QString codigocompletoarticulo = ( *lfacturamap ) ["CODIGOCOMPLETOARTICULO"];
        /// Completamos datos de la linea de presupuesto.
        query = "SELECT * FROM articulo WHERE codigocompletoarticulo = '" + codigocompletoarticulo + "'";
        cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            idarticulo = cur->value( "idarticulo" );
        } else {
            pgimport->mensajeria ( _ ( "El articulo de la linea de presupuesto no existe" ) + "<BR>\n" );
            idarticulo = "NULL";
        } // end if
        delete cur;
        /// Hacemos la insercion.
        query = " INSERT INTO lfactura(desclfactura, cantlfactura, pvplfactura, ivalfactura, descuentolfactura, idfactura, idarticulo) VALUES (";
        query += "'" + desclfactura + "'";
        query += ", " + cantlfactura;
        query += ", " + pvplfactura;
        query += ", " + ivalfactura;
        query += ", " + descuentolfactura;
        query += ", " + idfactura;
        query += ", " + idarticulo;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    /// Tratamos la insercion de los descuentos de presupuesto.
    tvalores *dfacturamap;

    QMutableListIterator<tvalores> ilistadfactura ( listadfactura );

    for ( dfacturamap = &listadfactura.first(); dfacturamap; dfacturamap = &ilistadfactura.next() ) {
        QString iddfactura = ( *dfacturamap ) ["IDDFACTURA"];
        QString conceptdfactura = ( *dfacturamap ) ["CONCEPTDFACTURA"];
        QString proporciondfactura = ( *dfacturamap ) ["PROPORCIONDFACTURA"];
        /// Hacemos la insercion.
        query = " INSERT INTO dfactura(conceptdfactura, proporciondfactura, idfactura) VALUES (";
        query += "'" + conceptdfactura + "'";
        query += ", " + proporciondfactura;
        query += ", " + idfactura;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    valores.clear();
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataLPresupuesto()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando LPresupuesto" ) + " " + valores["IDLPRESUPUESTO"] + "</B><BR>" );
    tvalores lpresupuestomap;
    lpresupuestomap.insert ( "IDLPRESUPUESTO", valores["IDLPRESUPUESTO"] );
    lpresupuestomap.insert ( "DESCLPRESUPUESTO", valores["DESCLPRESUPUESTO"] );
    lpresupuestomap.insert ( "CANTLPRESUPUESTO", valores["CANTLPRESUPUESTO"] );
    lpresupuestomap.insert ( "PVPLPRESUPUESTO", valores["PVPLPRESUPUESTO"] );
    lpresupuestomap.insert ( "IVALPRESUPUESTO", valores["IVALPRESUPUESTO"] );
    lpresupuestomap.insert ( "DESCUENTOLPRESUPUESTO", valores["DESCUENTOLPRESUPUESTO"] );
    lpresupuestomap.insert ( "IDPRESUPUESTO", valores["IDPRESUPUESTO"] );
    lpresupuestomap.insert ( "IDARTICULO", valores["IDARTICULO"] );
    lpresupuestomap.insert ( "CODARTICULO", valores["CODARTICULO"] );
    lpresupuestomap.insert ( "NOMARTICULO", valores["NOMARTICULO"] );
    lpresupuestomap.insert ( "ABREVARTICULO", valores["ABREVARTICULO"] );
    lpresupuestomap.insert ( "CODIGOCOMPLETOARTICULO", valores["CODIGOCOMPLETOARTICULO"] );
    listalpresupuesto.append ( lpresupuestomap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataDPresupuesto()
{
    BL_FUNC_DEBUG
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando DPresupuesto" ) + " " + valores["IDDPRESUPUESTO"] + "</B><BR>" );
    tvalores dpresupuestomap;
    dpresupuestomap.insert ( "IDDPRESUPUESTO", valores["IDDPRESUPUESTO"] );
    dpresupuestomap.insert ( "CONCEPTDPRESUPUESTO", valores["CONCEPTDPRESUPUESTO"] );
    dpresupuestomap.insert ( "PROPORCIONDPRESUPUESTO", valores["PROPORCIONDPRESUPUESTO"] );
    listadpresupuesto.append ( dpresupuestomap );
    
    return 0;
}


///
/**
\return
**/
int ImportBulmaFact::trataPresupuesto()
{
    BL_FUNC_DEBUG
    QString query;
    BlDbRecordSet *cur;
    /// En el XML se ha encontrado un tag de cliente que esta almacenado en la estructura valores.
    pgimport->mensajeria ( "<HR><B>" + _ ( "Tratando presupuesto" ) + " " + valores["IDPRESUPUESTO"] + "</B><BR>" );
    /// Primero hacemos la recoleccion de valores.
    QString idpresupuesto = valores["IDPRESUPUESTO"];
    QString numpresupuesto = valores["NUMPRESUPUESTO"];
    QString refpresupuesto = valores["REFPRESUPUESTO"];
    QString fpresupuesto = valores["FPRESUPUESTO"];
    QString descpresupuesto = valores["DESCPRESUPUESTO"];
    QString contactpresupuesto = valores["CONTACTPRESUPUESTO"];
    QString telpresupuesto = valores["TELPRESUPUESTO"];
    QString vencpresupuesto = valores["VENCPRESUPUESTO"];
    QString comentpresupuesto = valores["COMENTPRESUPUESTO"];
    QString idusuari = valores["IDUSUARI"];
    QString procesadopresupuesto = valores["PROCESADOPRESUPUESTO"];
    QString idcliente = valores["IDCLIENTE"];
    QString idalmacen = valores["IDALMACEN"];
    QString idforma_pago = valores["IDFORMA_PAGO"];
    QString idtrabajador = valores["IDTRABAJADOR"];
    QString codigoalmacen = valores["CODIGOALMACEN"];
    QString nomalmacen = valores["NOMALMACEN"];
    QString diralmacen = valores["DIRALMACEN"];
    QString poblalamacen = valores["POBLALMACEN"];
    QString cpalmacen = valores["CPALMACEN"];
    QString telalmacen = valores["TELALMACEN"];
    QString nomcliente = valores["NOMCLIENTE"];
    QString nomaltcliente = valores["NOMALTCLIENTE"];
    QString cifcliente = valores["CIFCLIENTE"];
    QString bancocliente = valores["BANCOCLIENTE"];
    QString dircliente = valores["DIRCLIENTE"];
    QString poblcliente = valores["POBLCLIENTE"];
    QString cpcliente = valores["CPCLIENTE"];
    QString telcliente = valores["TELCLIENTE"];
    QString faxcliente = valores["FAXCLIENTE"];
    QString mailcliente = valores["MAILCLIENTE"];
    QString urlcliente = valores["URLCLIENTE"];
    QString provcliente = valores["PROVCLIENTE"];
    QString nomtrabajador = valores["NOMTRABAJADOR"];
    QString apellidostrabajador = valores["APELLIDOSTRABAJADOR"];
    QString dirtrabajador = valores["DIRTRABAJADOR"];
    QString nsstrabajador = valores["NSSTRABAJADOR"];
    QString teltrabajador = valores["TELTRABAJADOR"];
    QString moviltrabajador = valores["MOVILTRABAJADOR"];
    QString emailtrabajador = valores["EMAILTRABAJADOR"];
    QString descforma_pago = valores["DESCFORMA_PAGO"];
    QString dias1tforma_pago = valores["DIAS1TFORMA_PAGO"];
    QString descuentoforma_pago = valores["DESCUENTOFORMA_PAGO"];
    /// Completamos datos que puedan faltar.
    query = "SELECT * FROM almacen WHERE codigoalmacen = '" + codigoalmacen + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idalmacen = cur->value( "idalmacen" );
    } else {
        pgimport->mensajeria ( _ ( "El almacen del presupuesto no existe" ) + "<BR>\n" );
        idalmacen = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM cliente WHERE cifcliente='" + cifcliente + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idcliente = cur->value( "idcliente" );
    } else {
        pgimport->mensajeria ( _ ( "El cliente del presupuesto no existe" ) + "<BR>\n" );
        idcliente = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM forma_pago WHERE descforma_pago = '" + descforma_pago + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idforma_pago = cur->value( "idforma_pago" );
    } else {
        pgimport->mensajeria ( _ ( "La forma de pago del presupuesto no existe" ) + "<BR>\n" );
        idforma_pago = "NULL";
    } // end if
    delete cur;

    query = "SELECT * FROM trabajador WHERE nomtrabajador = '" + nomtrabajador + "' AND nsstrabajador = '" + nsstrabajador + "'";
    cur = dbConnection->loadQuery ( query );
    if ( !cur->eof() ) {
        idtrabajador = cur->value( "idtrabajador" );
    } else {
        pgimport->mensajeria ( _ ( "El empleado del presupuesto no existe" ) + "<BR>\n" );
        idtrabajador = "NULL";
    } // end if
    delete cur;
    /// Hacemos la insercion.
    query = " INSERT INTO presupuesto(numpresupuesto, refpresupuesto, fpresupuesto, descpresupuesto, contactpresupuesto, telpresupuesto, vencpresupuesto, comentpresupuesto, idusuari, procesadopresupuesto, idcliente, idalmacen, idforma_pago, idtrabajador) VALUES (";
    query += "NULL";
    query += ", '" + refpresupuesto + "'";
    query += ", '" + fpresupuesto + "'";
    query += ", '" + descpresupuesto + "'";
    query += ", '" + contactpresupuesto + "'";
    query += ", '" + telpresupuesto + "'";
    query += ", '" + vencpresupuesto + "'";
    query += ", '" + comentpresupuesto + "'";
    query += ", NULL";
    query += ",'" + procesadopresupuesto + "'";
    query += "," + idcliente;
    query += "," + idalmacen;
    query += "," + idforma_pago;
    query += "," + idtrabajador;
    query += ")";

    dbConnection->begin();
    dbConnection->runQuery ( query );
    cur = dbConnection->loadQuery ( "SELECT MAX(idpresupuesto) AS id FROM presupuesto" );
    idpresupuesto = cur->value( "id" );
    delete cur;
    dbConnection->commit();

    /// Tratamos la insercion de las lineas de presupuesto.
    tvalores *lpresupuestomap;

    QMutableListIterator<tvalores> ilistalpresupuesto ( listalpresupuesto );

    for ( lpresupuestomap = &listalpresupuesto.first(); lpresupuestomap; lpresupuestomap = &ilistalpresupuesto.next() ) {
        QString idlpresupuesto = ( *lpresupuestomap ) ["IDLPRESUPUESTO"];
        QString desclpresupuesto = ( *lpresupuestomap ) ["DESCLPRESUPUESTO"];
        QString cantlpresupuesto = ( *lpresupuestomap ) ["CANTLPRESUPUESTO"];
        QString pvplpresupuesto = ( *lpresupuestomap ) ["PVPLPRESUPUESTO"];
        QString ivalpresupuesto = ( *lpresupuestomap ) ["IVALPRESUPUESTO"];
        QString descuentolpresupuesto = ( *lpresupuestomap ) ["DESCUENTOLPRESUPUESTO"];
        QString idarticulo = ( *lpresupuestomap ) ["IDARTICULO"];
        QString codarticulo = ( *lpresupuestomap ) ["CODARTICULO"];
        QString nomarticulo = ( *lpresupuestomap ) ["NOMARTICULO"];
        QString abrevarticulo = ( *lpresupuestomap ) ["ABREVARTICULO"];
        QString codigocompletoarticulo = ( *lpresupuestomap ) ["CODIGOCOMPLETOARTICULO"];
        /// Completamos datos de la linea de presupuesto.
        query = "SELECT * FROM articulo WHERE codigocompletoarticulo = '" + codigocompletoarticulo + "'";
        cur = dbConnection->loadQuery ( query );
        if ( !cur->eof() ) {
            idarticulo = cur->value( "idarticulo" );
        } else {
            pgimport->mensajeria ( _ ( "El articulo de la linea de presupuesto no existe" ) + "<BR>\n" );
            idarticulo = "NULL";
        } // end if
        delete cur;
        /// Hacemos la insercion.
        query = " INSERT INTO lpresupuesto(desclpresupuesto, cantlpresupuesto, pvplpresupuesto, ivalpresupuesto, descuentolpresupuesto, idpresupuesto, idarticulo) VALUES (";
        query += "'" + desclpresupuesto + "'";
        query += ", " + cantlpresupuesto;
        query += ", " + pvplpresupuesto;
        query += ", " + ivalpresupuesto;
        query += ", " + descuentolpresupuesto;
        query += ", " + idpresupuesto;
        query += ", " + idarticulo;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    /// Tratamos la insercion de los descuentos de presupuesto.
    tvalores *dpresupuestomap;

    QMutableListIterator<tvalores> ilistadpresupuesto ( listadpresupuesto );

    for ( dpresupuestomap = &listadpresupuesto.first(); dpresupuestomap; dpresupuestomap = &ilistadpresupuesto.next() ) {
        QString iddpresupuesto = ( *dpresupuestomap ) ["IDDPRESUPUESTO"];
        QString conceptdpresupuesto = ( *dpresupuestomap ) ["CONCEPTDPRESUPUESTO"];
        QString proporciondpresupuesto = ( *dpresupuestomap ) ["PROPORCIONDPRESUPUESTO"];
        /// Hacemos la insercion.
        query = " INSERT INTO dpresupuesto(conceptdpresupuesto, proporciondpresupuesto, idpresupuesto) VALUES (";
        query += "'"  + conceptdpresupuesto + "'";
        query += ", " + proporciondpresupuesto;
        query += ", " + idpresupuesto;
        query += ")";
        dbConnection->runQuery ( query );
    } // end for
    valores.clear();
    
    return 0;
}

