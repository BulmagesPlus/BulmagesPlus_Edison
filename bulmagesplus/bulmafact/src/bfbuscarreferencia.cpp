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

#include "bfbuscarreferencia.h"
#include "bfcompany.h"
#include "blform.h"
#include "blplugins.h"


/** Inicializa todos los componentes del Widget a NULL para que no haya
    confusion en determinar si un objeto ha sido creado o no.
*/
/**
\param parent
**/
BfBuscarReferencia::BfBuscarReferencia ( QWidget *parent )
        : BlWidget ( parent )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    connect ( mui_referencia, SIGNAL ( returnPressed() ), this, SLOT ( on_mui_rferencia_returnPressed() ) );

    /// Establecemos el focusProxy
    setFocusProxy (mui_referencia);
    
    
}


/** No requiere de acciones adicionales en la destruccion de la clase.
*/
/**
**/
BfBuscarReferencia::~BfBuscarReferencia()
{
    BL_FUNC_DEBUG
    
}


/** SLOT que responde a la peticion del informe por referencia.
    Crea una instancia de InformeReferencia, la inicializa y la lanza.
*/
/**
**/
void BfBuscarReferencia::on_mui_informe_clicked()
{
    BL_FUNC_DEBUG

    BlForm newfich ( mainCompany(), NULL );
    newfich.setTitleName ( _ ( "Informe" ) );
    newfich.addDbField ( "referencia", BlDbField::DbVarChar, BlDbField::DbPrimaryKey, _ ( "Referencia" ) );
    newfich.setDbValue ( "referencia", mui_referencia->text() );
    if ( newfich.generateRML ( "informereferencia.rml" ) ) {
        blCreateAndLoadPDF ( "informereferencia" );
    } // end if

    

}


/** SLOT que responde al fin de edicion de la casilla de referencia.
    Emite la se&ntilde;al valueChanged() para notificaciones.
*/
/**
**/
void BfBuscarReferencia::on_mui_referencia_editingFinished()
{
    BL_FUNC_DEBUG
    emit ( valueChanged ( mui_referencia->text() ) );
    
}


/** SLOT que responde a la pulsacion de RETURN sobre la casilla de referencia.
    Emite la se&ntilde;al returnPressed() para notificaciones.
*/
/**
**/
void BfBuscarReferencia::on_mui_rferencia_returnPressed()
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins
    int res = g_plugins->run ( "BfBuscarReferencia_on_mui_rferencia_returnPressed", this );
    if ( res == 0 ) {
        emit ( returnPressed ( ) );
    } // end if
    
}


/** SLOT que responde a la pulsacion del boton abrirtodo.
    Busca en todos los documentos de la base de datos (presupuestos, pedidos, albaranes, facturas, cobros y pagos)
    Todos aquellos elementos que tengan la misma referencia que el objeto abierto.
    Instancia los objetos correspondientes y los lanza.
*/
/**
**/
void BfBuscarReferencia::on_mui_abrirtodo_clicked()
{
    BL_FUNC_DEBUG
    /// Empezamos con los presupuestos.
    /*
        QString SQLQuery = "SELECT * FROM presupuesto WHERE refpresupuesto = '" + mui_referencia->text() + "'";
        BlDbRecordSet *cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            PresupuestoView * bud = ( ( BfCompany * ) mainCompany() ) ->nuevoPresupuestoView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idpresupuesto" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */
    /*
        SQLQuery = "SELECT * FROM pedidocliente WHERE refpedidocliente = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            PedidoClienteView * bud = ( ( BfCompany * ) mainCompany() ) ->newPedidoClienteView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idpedidocliente" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */
    /*
        SQLQuery = "SELECT * FROM albaran WHERE refalbaran = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            AlbaranClienteView * bud = ( ( BfCompany * ) mainCompany() ) ->newAlbaranClienteView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idalbaran" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */

    /*
        SQLQuery = "SELECT * FROM factura WHERE reffactura = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            FacturaView * bud = ( ( BfCompany * ) mainCompany() ) ->newFacturaView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idfactura" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */

    /*
        SQLQuery = "SELECT * FROM cobro WHERE refcobro = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            CobroView * bud = ( ( BfCompany * ) mainCompany() ) ->newCobroView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idcobro" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */

    /*
        SQLQuery = "SELECT * FROM pedidoproveedor WHERE refpedidoproveedor = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            PedidoProveedorView * bud = ( ( BfCompany * ) mainCompany() ) ->nuevoPedidoProveedorView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idpedidoproveedor" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */

    /*
        SQLQuery = "SELECT * FROM albaranp WHERE refalbaranp = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            AlbaranProveedorView * bud = ( ( BfCompany * ) mainCompany() ) ->newAlbaranProveedorView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idalbaranp" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */

    /*
        SQLQuery = "SELECT * FROM facturap WHERE reffacturap = '" + mui_referencia->text() + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        while ( !cur->eof() ) {
            FacturaProveedorView * bud = ( ( BfCompany * ) mainCompany() ) ->newFacturaProveedorView();
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->load ( cur->value( "idfacturap" ) );
            bud->show();
            cur->nextRecord();
        } // end while
        delete cur;
    */
    g_plugins->run ( "BfBuscarReferencia_on_mui_abrirtodo_clicked_Post", this );

    
}


/** Establece la referencia que va a mostrar el control
**/
/**
\param val
**/
void BfBuscarReferencia::setText ( QString val )
{
    BL_FUNC_DEBUG
    mui_referencia->setText ( val );
    
}


/** Establece la referencia que va a mostrar el control
**/
/**
\param val
**/
void BfBuscarReferencia::setFieldValue ( QString val )
{
    BL_FUNC_DEBUG
    mui_referencia->setText ( val );
    
}


/** Devuelve el texto que esta escrito en el control
**/
/**
\return
**/
QString BfBuscarReferencia::text() const
{
    BL_FUNC_DEBUG
    
    return mui_referencia->text();
}


/** Devuelve el texto que esta escrito en el control
**/
/**
\return
**/
QString BfBuscarReferencia::fieldValue() const
{
    BL_FUNC_DEBUG
    
    return mui_referencia->text();
}

