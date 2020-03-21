/***************************************************************************
 *   Copyright (C) 2005 by Tomeu Borras Riera                              *
 *   tborras@conetxia.com                                                  *
 *   http://www.iglues.org                                                 *
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

#include <fstream>

#include <QCloseEvent>
#include <QComboBox>
#include <QLayout>
#include <QMessageBox>
#include <QObject>
#include <QToolButton>
#include <QWidget>
#include <QIntValidator>

#include "blplugins.h"
#include "albaranclientelist.h"
#include "albaranclienteview.h"
#include "clientslist.h"
#include "bfcompany.h"
#include "facturaview.h"
#include "listdescfacturaview.h"
#include "listlinfacturaview.h"


/** Inicializa la pantalla.
    Inicializa todos los componentes y mete la pantalla en el workSpace.
*/
/**
\param comp
\param parent
**/
FacturaView::FacturaView ( BfCompany *comp, QWidget *parent )
        : BfForm ( comp, parent )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    setAttribute ( Qt::WA_DeleteOnClose );

    mui_numfactura->setValidator( new QIntValidator(1, INT_MAX, this) );

    try {
        setTitleName ( _ ( "Factura" ) );
        setDbTableName ( "factura" );
        setDbFieldId ( "idfactura" );
        addDbField ( "idfactura", BlDbField::DbInt, BlDbField::DbPrimaryKey, _ ( "Id factura" ) );
        addDbField ( "idcliente", BlDbField::DbInt, BlDbField::DbNotNull, _ ( "Cliente" ) );
        addDbField ( "idalmacen", BlDbField::DbInt, BlDbField::DbNotNull, _ ( "Almacen" ) );
        addDbField ( "numfactura", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Numero factura" ) );
        addDbField ( "ffactura", BlDbField::DbDate, BlDbField::DbNothing, _ ( "Ffactura" ) );
        addDbField ( "procesadafactura", BlDbField::DbBoolean, BlDbField::DbNothing, _ ( "Procesada factura" ) );
        addDbField ( "codigoserie_factura", BlDbField::DbVarChar, BlDbField::DbNotNull, _ ( "Serie" ) );
        addDbField ( "comentfactura", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Comentario" ) );
        addDbField ( "reffactura", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Referencia" ) );
        addDbField ( "descfactura", BlDbField::DbVarChar, BlDbField::DbNothing, _ ( "Descripcion" ) );
        addDbField ( "idtrabajador", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Trabajador" ) );
        addDbField ( "idforma_pago", BlDbField::DbInt, BlDbField::DbNothing, _ ( "Forma de pago" ) );

        /// Establecemos algunos Buddies.
        mui_labelAlmacen->setText ( _ ( "Al&macen" ) );
        mui_labelAlmacen->setBuddy ( mui_idalmacen );

        /// Disparamos los plugins.
        int res = g_plugins->run ( "FacturaView_FacturaView", this );
        if ( res != 0 ) {
            return;
        } // end if
        subform2->setMainCompany ( comp );
        mui_idalmacen->setMainCompany ( comp );
        mui_idforma_pago->setMainCompany ( comp );
        mui_idcliente->setMainCompany ( comp );
        m_descuentos->setMainCompany ( comp );
        mui_codigoserie_factura->setMainCompany ( comp );
        mui_reffactura->setMainCompany ( comp );
        mui_idtrabajador->setMainCompany ( comp );
      
        /// Inicializamos BfForm.
        setListaLineas ( subform2 );
        setListaDescuentos ( m_descuentos );

        /// Establecemos los parametros de busqueda del Cliente
        mui_idcliente->setLabel ( _ ( "Cliente *:" ) );
        mui_idcliente->setTableName ( "cliente" );
        mui_idcliente->m_valores["cifcliente"] = "";
        mui_idcliente->m_valores["nomcliente"] = "";

        /// Establecemos valores por defecto en los combo boxes para que no se queden sin inicializar.
        mui_idalmacen->setId ( "" );
        mui_codigoserie_factura->setId ( "" );
        mui_idforma_pago->setId ( "" );
        mui_idtrabajador->setId ( "0" );

        m_totalBases->setReadOnly ( true );
        m_totalBases->setAlignment ( Qt::AlignRight );
        m_totalTaxes->setReadOnly ( true );
        m_totalTaxes->setAlignment ( Qt::AlignRight );
        m_totalDiscounts->setReadOnly ( true );
        m_totalDiscounts->setAlignment ( Qt::AlignRight );
        m_totalfactura->setReadOnly ( true );
        m_totalfactura->setAlignment ( Qt::AlignRight );
        insertWindow ( windowTitle(), this, false );
        blScript(this);
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al crear la factura" ), this );
    }
    
}


/** Al destruir la clase provocamos que se refresque el listado de facturas.
    Para que salga actualizado.
*/
/**
**/
FacturaView::~FacturaView()
{
    BL_FUNC_DEBUG
    
}


/** Inicializa los subformularios y resetea el control de cambios.
*/
/**
**/
void FacturaView::inicializar()
{
    BL_FUNC_DEBUG
    subform2->inicializar();
    m_descuentos->inicializar();
    pintar();
    dialogChanges_readValues();
    
}


/** Pinta los campos de totales, que al no estar en la base de datos son tratados
    de forma distinta.
*/
/**
\param iva
\param base
\param total
\param desc
\param irpf
\param reqeq
**/
void FacturaView::pintatotales ( BlFixed iva, BlFixed base, BlFixed total, BlFixed desc, BlFixed irpf, BlFixed reqeq )
{
    BL_FUNC_DEBUG
    
    int decimals = g_confpr->value(CONF_TOTAL_DECIMAL_PLACES).toInt();
    
    m_totalBases->setText ( base.toQString('0', decimals) );
    m_totalTaxes->setText ( iva.toQString('0', decimals) );
    m_totalfactura->setText ( total.toQString('0', decimals) );
    m_totalDiscounts->setText ( desc.toQString('0', decimals) );
    m_totalIRPF->setText ( QString ( irpf.toQString('0', decimals) ) );
    m_totalReqEq->setText ( QString ( reqeq.toQString('0', decimals) ) );
    
}




/** SLOT que responde a la pulsacion del boton mui_veralbaranes.
    Busca todos los albaranes con la misma referencia y para cada uno de ellos instancia la clase \ref AlbaranClienteView
*/
/**
**/
void FacturaView::on_mui_veralbaranes_clicked()
{
    BL_FUNC_DEBUG
    AlbaranClienteView *bud = NULL;
    BlDbRecordSet *cur = NULL;
    try {
        QString SQLQuery = "SELECT * FROM albaran WHERE refalbaran = '" + dbValue ( "reffactura" ) + "'";
        cur = mainCompany() ->loadQuery ( SQLQuery );
        if ( !cur->eof() ) {
            while ( !cur->eof() ) {
                bud = new AlbaranClienteView ( mainCompany(), NULL );
                mainCompany() ->m_pWorkspace->addSubWindow ( bud );
                bud->load ( cur->value( "idalbaran" ) );
                bud->show();
                cur->nextRecord();
            } // end while
        } else {
            blMsgInfo ( _ ( "No hay albaranes con esta referencia" ), this );
        } // end if
        delete cur;
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error inesperado" ), this );
        if ( cur ) delete cur;
        if ( bud ) delete bud;
    } // end try
    
}


///
/**
\param id
**/
void FacturaView::on_mui_idcliente_valueChanged ( QString id )
{
    BL_FUNC_DEBUG
    subform2->setIdCliente ( id );
    mui_idforma_pago->setIdCliente ( id );
    
}


///
/**
**/
void FacturaView::on_m_descuentos_editFinish ( int, int )
{
    BL_FUNC_DEBUG
    calculaypintatotales();
    
}


///
/**
**/
void FacturaView::on_subform2_editFinish ( int, int )
{
    BL_FUNC_DEBUG
    calculaypintatotales();
    
}


/** Se encarga del borrado de una factura.
*/
/**
\return
**/
int FacturaView::beforeDelete()
{
    BL_FUNC_DEBUG
    g_plugins->run("FacturaView_beforeDelete", this);
    
    int error = m_listalineas->remove();
    error = m_listadescuentos->remove();
    
    return 0;
}


/** Carga una factura.
*/
/**
\param idbudget
\return
**/
int FacturaView::cargarPost ( QString idbudget )
{
    BL_FUNC_DEBUG

    m_listalineas->load ( idbudget );
    m_listadescuentos->load ( idbudget );

    /// Disparamos los plugins.
    g_plugins->run ( "FacturaView_cargarPost_Post", this );

    calculaypintatotales();

    
    return 0;
}


/** Se encarga del guardado de una factura.
*/
/**
\return
**/
int FacturaView::afterSave()
{
    BL_FUNC_DEBUG
    m_listadescuentos->setColumnValue ( "idfactura", dbValue ( "idfactura" ) );
    m_listalineas->setColumnValue ( "idfactura", dbValue ( "idfactura" ) );
    m_listalineas->save();
    m_listadescuentos->save();

    g_plugins->run("FacturaView_afterSave", this);
    
    return 0;
}


///
/**
\param id
**/
void FacturaView::on_mui_idalmacen_valueChanged ( QString id )
{
    BL_FUNC_DEBUG
    m_listalineas->setIdAlmacen ( id );
    
}



