/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
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

#include <QString>
#include <QFile>
#include <QTextStream>

#include "bfinformecliente.h"
#include "bfcompany.h"
#include "blpostgresqlclient.h"
#include "blfunctions.h"


/// ================================================================
/// =================== INFORME CLIENTE ============================
/// ================================================================
/**
\param comp
**/
BfInformeCliente::BfInformeCliente ( BfCompany *comp )
{
    BL_FUNC_DEBUG

    companyact = comp;
}


///
/**
**/
BfInformeCliente::~BfInformeCliente()
{
    BL_FUNC_DEBUG
}


/// Establece el identificador del cliente a partir del cual generar el informe.
/**
\param val el valor del identificador de cliente.
**/
void BfInformeCliente::setCliente ( QString val )
{
    BL_FUNC_DEBUG

    m_idcliente = val;
}


///
/**
**/
void BfInformeCliente::generarInforme()
{
    BL_FUNC_DEBUG
    
    /// Copiamos el archivo.
    QString archivo = g_confpr->value( CONF_DIR_OPENREPORTS ) + "informecliente.rml";
    QString archivod = g_confpr->value( CONF_DIR_USER ) + "informecliente.rml";
    blCopyFile(archivo, archivod);

    /// Copiamos el logo.
    QString archivologo = g_confpr->value( CONF_DIR_OPENREPORTS ) + "logo.jpg";
    QString logousuario = g_confpr->value( CONF_DIR_USER ) + "logo.jpg";
    blCopyFile(archivologo, logousuario);
    
    
    QFile file;
    file.setFileName ( archivod );
    file.open ( QIODevice::ReadOnly );
    QTextStream stream ( &file );
    QString buff = stream.readAll();
    file.close();
    QString fitxersortidatxt = "";

    /// Sacamos los datos del cliente
    QString SQLQuery = "SELECT * FROM cliente WHERE idcliente = " + m_idcliente;
    BlDbRecordSet *cur = companyact->loadQuery ( SQLQuery );
    while ( !cur->eof() ) {
        buff.replace ( "[nomcliente]", cur->value( "nomcliente" ) );
        buff.replace ( "[telcliente]", cur->value( "telcliente" ) );
        buff.replace ( "[dircliente]", cur->value( "dircliente" ) );
        buff.replace ( "[poblcliente]", cur->value( "poblcliente" ) );
        cur->nextRecord();
    } // end while
    delete cur;

    /// Sacamos todas las referencias de este cliente y las guardamos en el string referencias.
    QString referencias = "(";
    QString coma = "";
    SQLQuery = "SELECT refpresupuesto AS referencia FROM presupuesto WHERE idcliente = " + m_idcliente;
    SQLQuery += " UNION SELECT refpedidocliente AS referencia FROM pedidocliente WHERE idcliente = " + m_idcliente;
    SQLQuery += " UNION SELECT refalbaran FROM albaran AS referencia WHERE idcliente = " + m_idcliente;
    SQLQuery += " UNION SELECT reffactura FROM factura AS referencia WHERE idcliente = " + m_idcliente;
    SQLQuery += " UNION SELECT refcobro   FROM cobro   AS referencia WHERE idcliente = " + m_idcliente;
    cur = companyact->loadQuery ( SQLQuery );
    if ( cur->eof() ) {
        delete cur;
    } else {
      while ( !cur->eof() ) {
	  referencias += coma + "'" + cur->value( "referencia" ) + "' ";
	  coma = ",";
	  cur->nextRecord();
      }
      delete cur;
      referencias += ")";

      /// Generacion del informe de ventas.
      fitxersortidatxt = "<spacer length=\"15\"/>";
      fitxersortidatxt += "<para>Resumen de ventas por articulo</para>\n";
      fitxersortidatxt += "<blockTable style=\"tablaresumen\" colWidths=\"9cm, 2.5cm, 2.5cm, 2.5cm, 2.5cm\" repeatRows=\"1\">\n";
      fitxersortidatxt += "<tr>\n";
      fitxersortidatxt += " <td>" + _ ( "Articulo" ) + "</td>\n";
      fitxersortidatxt += " <td>" + _ ( "Pres." ) + "</td>\n";
      fitxersortidatxt += " <td>" + _ ( "Pedido" ) + "</td>\n";
      fitxersortidatxt += " <td>" + _ ( "Entregado" ) + "</td>\n";
      fitxersortidatxt += " <td>" + _ ( "Facturado" ) + "</td>\n";
      fitxersortidatxt += "</tr>\n";

      SQLQuery = " SELECT * FROM articulo ";
      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlpresupuesto) AS cantlpresupuestot FROM lpresupuesto WHERE idpresupuesto IN (SELECT idpresupuesto FROM presupuesto WHERE refpresupuesto IN " + referencias + ") GROUP BY idarticulo) AS t1 ON t1.idarticulo = articulo.idarticulo ";
      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlpedidocliente) AS cantlpedidoclientet FROM lpedidocliente WHERE idpedidocliente IN (SELECT idpedidocliente FROM pedidocliente WHERE refpedidocliente IN " + referencias + ") GROUP BY idarticulo) AS t2 ON t2.idarticulo = articulo.idarticulo ";
      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlalbaran) AS cantlalbarant FROM lalbaran WHERE idalbaran IN (SELECT idalbaran FROM albaran WHERE refalbaran IN " + referencias + ") GROUP BY idarticulo) AS t3 ON t3.idarticulo = articulo.idarticulo ";
      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlfactura) AS cantlfacturat FROM lfactura WHERE idfactura IN (SELECT idfactura FROM factura WHERE reffactura IN " + referencias + ") GROUP BY idarticulo) AS t4 ON t4.idarticulo = articulo.idarticulo ";
      SQLQuery += " WHERE (cantlpresupuestot <> 0 OR cantlpedidoclientet <> 0 OR cantlalbarant <> 0 OR cantlfacturat <> 0) ";
      cur = companyact->loadQuery ( SQLQuery );
      while ( !cur->eof() ) {
	  fitxersortidatxt += "<tr>\n";
	  fitxersortidatxt += "<td>" + cur->value( "nomarticulo" ) + "</td>\n";
	  fitxersortidatxt += "<td>" + cur->value( "cantlpresupuestot" ) + "</td>\n";
	  fitxersortidatxt += "<td>" + cur->value( "cantlpedidoclientet" ) + "</td>\n";
	  fitxersortidatxt += "<td>" + cur->value( "cantlalbarant" ) + "</td>\n";
	  fitxersortidatxt += "<td>" + cur->value( "cantlfacturat" ) + "</td>\n";
	  fitxersortidatxt += "</tr>\n";
	  cur->nextRecord();
      } // end while
      delete cur;

      fitxersortidatxt += "</blockTable>\n";

      /// Generacion del informe de compras.
      fitxersortidatxt += "<spacer length=\"15\"/>";
      fitxersortidatxt += "<para>Resumen de compras por articulo</para>\n";
      fitxersortidatxt += "<blockTable style=\"tablaresumen\" colWidths=\"10cm, 3cm, 3cm, 3cm\" repeatRows=\"1\">\n";
      fitxersortidatxt += "<tr>\n";
      fitxersortidatxt += "    <td>" + _ ( "Articulo" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Pedido" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Entregado" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Facturado" ) + "</td>\n";
      fitxersortidatxt += "</tr>\n";

      SQLQuery = " SELECT * FROM articulo ";
      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlpedidoproveedor) AS cantlpedidoproveedort  FROM lpedidoproveedor WHERE idpedidoproveedor IN (SELECT idpedidoproveedor FROM pedidoproveedor WHERE refpedidoproveedor IN " + referencias + ") GROUP BY idarticulo) AS t2 ON t2.idarticulo = articulo.idarticulo ";

      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlalbaranp) AS cantlalbaranpt  FROM lalbaranp WHERE idalbaranp IN (SELECT idalbaranp FROM albaranp WHERE refalbaranp IN " + referencias + ") GROUP BY idarticulo) AS t3 ON t3.idarticulo = articulo.idarticulo ";

      SQLQuery += " LEFT JOIN (SELECT idarticulo, SUM(cantlfacturap) AS cantlfacturapt  FROM lfacturap WHERE idfacturap IN (SELECT idfacturap FROM facturap WHERE reffacturap IN " + referencias + ") GROUP BY idarticulo) AS t4 ON t4.idarticulo = articulo.idarticulo ";
      SQLQuery += " WHERE  ( cantlpedidoproveedort <> 0 OR cantlalbaranpt <> 0 OR cantlfacturapt <> 0) ";

      fprintf ( stdout, "%s\n", SQLQuery.toLatin1().constData() );

      cur = companyact->loadQuery ( SQLQuery );
      while ( !cur->eof() ) {
	  fitxersortidatxt += "<tr>\n";
	  fitxersortidatxt += "    <td>" + cur->value( "nomarticulo" ) + "</td>\n";
	  fitxersortidatxt += "    <td>" + cur->value( "cantlpedidoproveedort" ) + "</td>\n";
	  fitxersortidatxt += "    <td>" + cur->value( "cantlalbaranpt" ) + "</td>\n";
	  fitxersortidatxt += "    <td>" + cur->value( "cantlfacturapt" ) + "</td>\n";
	  fitxersortidatxt += "</tr>\n";
	  cur->nextRecord();
      } // end while
      delete cur;

      fitxersortidatxt += "</blockTable>\n";


      buff.replace ( "[story]", fitxersortidatxt );

      fitxersortidatxt = "<spacer length=\"15\"/>";
      /// Generacion del informe de totales de ventas.
      fitxersortidatxt += "<para>Totales ventas</para>\n";
      fitxersortidatxt += "<blockTable style=\"tablatotales\" colWidths=\"3cm, 3cm, 3cm, 3cm, 3cm\" repeatRows=\"1\">\n";
      fitxersortidatxt += "<tr>\n";
      fitxersortidatxt += "    <td>" + _ ( "Pres." ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Pedido" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Entregado" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Facturado" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Cobrado" ) + "</td>\n";
      fitxersortidatxt += "</tr>\n";

      /// Calculo de las cantidades totales en moneda.
      fitxersortidatxt += "<tr>\n";

      /// Total presupuestado.
      SQLQuery = "SELECT SUM(totalpresupuesto) AS tpres FROM presupuesto WHERE refpresupuesto IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "tpres" ) + "</td>\n";
      delete cur;

      /// Total pedido.
      SQLQuery = "SELECT SUM(totalpedidocliente) AS tpedcli FROM pedidocliente WHERE refpedidocliente IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "tpedcli" ) + "</td>\n";
      delete cur;

      /// Total trabajado.
      SQLQuery = "SELECT SUM(totalalbaran) AS talb FROM albaran WHERE refalbaran IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "talb" ) + "</td>\n";
      delete cur;

      /// Total facturado.
      SQLQuery = "SELECT SUM(totalfactura) AS tfact FROM factura WHERE reffactura IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "tfact" ) + "</td>\n";
      delete cur;

      /// Total cobrado.
      SQLQuery = "SELECT SUM(cantcobro) AS tcobro FROM cobro WHERE refcobro IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "     <td>" + cur->value( "tcobro" ) + "</td>\n";
      delete cur;

      fitxersortidatxt += "</tr>\n";
      fitxersortidatxt += "</blockTable>\n";

      fitxersortidatxt += "<spacer length=\"15\"/>";
      /// Generacion del informe de totales de compras.
      fitxersortidatxt += "<para>Totales compras</para>\n";
      fitxersortidatxt += "<blockTable style=\"tablatotales\" colWidths=\" 4cm, 4cm, 4cm, 3cm\" repeatRows=\"1\">\n";
      fitxersortidatxt += "<tr>\n";
      fitxersortidatxt += "    <td>" + _ ( "Pedido" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Entregado" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Facturado" ) + "</td>\n";
      fitxersortidatxt += "    <td>" + _ ( "Pagado" ) + "</td>\n";
      fitxersortidatxt += "</tr>\n";

      /// Calculo de las cantidades totales en moneda.
      fitxersortidatxt += "<tr>\n";

      /// Total pedido.
      SQLQuery = "SELECT SUM(totalpedidoproveedor) AS tpedpro FROM pedidoproveedor WHERE refpedidoproveedor IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "tpedpro" ) + "</td>\n";
      delete cur;

      /// Total trabajado.
      SQLQuery = "SELECT SUM(totalalbaranp) AS talbp FROM albaranp WHERE refalbaranp IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "talbp" ) + "</td>\n";
      delete cur;

      /// Total facturado.
      SQLQuery = "SELECT SUM(totalfacturap) AS tfactp FROM facturap WHERE reffacturap IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "    <td>" + cur->value( "tfactp" ) + "</td>\n";
      delete cur;

      /// Total cobrado.
      SQLQuery = "SELECT SUM(cantpago) AS tpago FROM pago WHERE refpago IN " + referencias;
      cur = companyact->loadQuery ( SQLQuery );
      fitxersortidatxt += "     <td>" + cur->value( "tpago" ) + "</td>\n";
      delete cur;

      fitxersortidatxt += "</tr>\n";
      fitxersortidatxt += "</blockTable>\n";


      buff.replace ( "[totales]", fitxersortidatxt );
    } // end if

    if ( file.open ( QIODevice::WriteOnly ) ) {
        QTextStream stream ( &file );
        stream << buff;
        file.close();
    } // end if
    blCreateAndLoadPDF ( "informecliente" );
    
}


