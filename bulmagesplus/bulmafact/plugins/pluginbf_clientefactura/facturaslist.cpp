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

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "bfbuscararticulo.h"
#include "bldatesearch.h"
#include "bfcompany.h"
#include "blconfiguration.h"
#include "facturaslist.h"
#include "facturaview.h"
#include "blfunctions.h"
#include "blplugins.h"


/** Prepara todos los componentes i los inicializa a NULL para que no haya dudas sobre si
    ya han sido inicializados o no.
    Inicializando la clase con este constructor precisa que sea establecido el company con setcompany.
*/
/**
\param parent
\param flag
\param editmodo
**/
FacturasList::FacturasList ( QWidget *parent, Qt::WindowFlags flag, edmode editmodo )
        : BlFormList ( NULL, parent, flag, editmodo )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "FacturasList_FacturasList", this );
    if ( res != 0 )
        return;
    iniciaForm();
    mdb_idfactura = "";
    /// Establecemos los parametros de busqueda del Cliente
    m_cliente->setLabel ( _ ( "Cliente:" ) );
    m_cliente->setTableName ( "cliente" );
    m_cliente->m_valores["cifcliente"] = "";
    m_cliente->m_valores["nomcliente"] = "";
    setSubForm ( mui_list );
    hideBusqueda();
    /// Llamamos a los scripts
    blScript(this);
    
}


/** Inicializa todos los componentes y prepara la ventana para funcionar.
*/
/**
\param comp
\param parent
\param flag
\param editmodo
**/
FacturasList::FacturasList ( BfCompany *comp, QWidget *parent, Qt::WindowFlags flag, edmode editmodo )
        : BlFormList ( comp, parent, flag, editmodo )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "FacturasList_FacturasList", this );
    if ( res != 0 )
        return;
    iniciaForm();
    m_cliente->setMainCompany ( mainCompany() );
    m_articulo->setMainCompany ( mainCompany() );
    mui_list->setMainCompany ( mainCompany() );
    setSubForm ( mui_list );
    /// Establecemos los parametros de busqueda del cliente
    m_cliente->setLabel ( _ ( "Cliente:" ) );
    m_cliente->setTableName ( "cliente" );
    m_cliente->m_valores["cifcliente"] = "";
    m_cliente->m_valores["nomcliente"] = "";
    
    /// Iniciamos el buscador de series.
    mui_seriefactura->setMainCompany ( mainCompany() );
    mui_seriefactura->setQuery ( "SELECT * FROM serie_factura ORDER BY descserie_factura" );
    mui_seriefactura->setTableName ( "serie_factura" );
    mui_seriefactura->setFieldId ( "codigoserie_factura" );
    mui_seriefactura->m_valores["codigoserie_factura"] = "";
    mui_seriefactura->m_valores["descserie_factura"] = "";
    mui_seriefactura->setAllowNull ( true );
    mui_seriefactura->setId("");

    /// Iniciamos el buscador de almacenes.
    mui_almacen->setMainCompany ( mainCompany() );
    mui_almacen->setQuery ( "SELECT * FROM almacen ORDER BY idalmacen" );
    mui_almacen->setTableName ( "almacen" );
    mui_almacen->setFieldId ( "codigoalmacen" );
    mui_almacen->m_valores["codigoalmacen"] = "";
    mui_almacen->m_valores["nomalmacen"] = "";
    mui_almacen->setAllowNull ( true );
    mui_almacen->setId("");
    
    /// Cargamos los filtros guardados.
    cargaFiltrosXML();
    
    presentar();

    mdb_idfactura = "";
    if ( editMode() ) {
        mainCompany() ->insertWindow ( windowTitle(), this );
    } // end if
    hideBusqueda();
    /// Hacemos el tratamiento de los permisos que desabilita botones en caso de no haber suficientes permisos.
    trataPermisos ( "factura" );
    /// Llamamos a los scripts
    blScript(this);
    
}


///
/**
\return
**/
void FacturasList::iniciaForm()
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins.
    int res = g_plugins->run ( "FacturasList_iniciaForm", this );
    if ( res != 0 )
        return;
    mui_procesada->insertItem ( 0, _ ( "Todas las facturas" ) );
    mui_procesada->insertItem ( 1, _ ( "Facturas procesadas" ) );
    mui_procesada->insertItem ( 2, _ ( "Facturas no procesadas" ) );
    mui_procesada->setCurrentIndex ( 2 );
    
}


/** No precisa de acciones especiales en el destructor de la clase.
*/
/**
**/
FacturasList::~FacturasList()
{
    BL_FUNC_DEBUG
    
}


/** Hace la carga del subformulario y el calculo de los totales.
*/
/**
**/
void FacturasList::presentar()
{
    BL_FUNC_DEBUG

    mui_list->load ( "SELECT *, totalfactura AS total, bimpfactura AS base, impfactura AS impuestos FROM factura LEFT JOIN cliente ON factura.idcliente = cliente.idcliente LEFT JOIN almacen ON factura.idalmacen = almacen.idalmacen LEFT JOIN forma_pago ON factura.idforma_pago = forma_pago.idforma_pago WHERE 1 = 1 " + generaFiltro() );

    /// Hacemos el calculo del total.
    BlDbRecordSet *cur = mainCompany() ->loadQuery ( "SELECT SUM(totalfactura)::NUMERIC(12,2) AS total, SUM(bimpfactura)::NUMERIC(12,2) AS base, SUM(impfactura)::NUMERIC(12,2) AS impuestos FROM factura LEFT JOIN cliente ON factura.idcliente = cliente.idcliente LEFT JOIN almacen ON factura.idalmacen = almacen.idalmacen WHERE 1 = 1 " + generaFiltro() );
    /// Esta consulta podria resultar en NULL y por tanto debe tratarse el caso
    /// Usamos el localeformat porque los datos son presentados en pantalla y el punto decimal debe salir bien.
    if ( cur ) {
        mui_totalbimponible->setText ( cur->value( "base", -1, true ) );
        mui_totalimpuestos->setText ( cur->value( "impuestos", -1, true ) );
        mui_totalfacturas->setText ( cur->value( "total", -1, true ) );
        delete cur;
    } // end if

    
}


/** Funcion auxiliar que genera la clausula WHERE de la consulta del listado a partir de las opciones de filtrado que el usuario haya especificado.
*/
/**
\return
**/
const QString FacturasList::generaFiltro()
{
    BL_FUNC_DEBUG
    /// Tratamiento de los filtros.
    QString filtro = "";
    
    /// Hacemos el filtrado like del campo m_filtro
    filtro += mui_list->likeFilterSQL(m_filtro->text());
    
    if ( m_cliente->id() != "" ) {
        filtro += " AND factura.idcliente = " + m_cliente->id();
    } // end if

    /// Tratamos los procesados y no procesados en las facturas para el filtraje.
    if ( mui_procesada->currentIndex() == 1 ) {
        /// Muestra solo las procesadas.
        filtro += " AND procesadafactura ";
    } else if ( mui_procesada->currentIndex() == 2 ) {
        /// Muestra solo las NO procesadas.
        filtro += " AND NOT procesadafactura ";
    } // end if

    if ( m_articulo->idarticulo() != "" ) {
        filtro += " AND idfactura IN (SELECT DISTINCT idfactura FROM lfactura WHERE idarticulo = '" + m_articulo->idarticulo() + "') ";
    } // end if
    if ( m_fechain->text() != "" ) {
        filtro += " AND ffactura >= '" + m_fechain->text() + "' ";
    } // end if
    if ( m_fechafin->text() != "" ) {
        filtro += " AND ffactura <= '" + m_fechafin->text() + "' ";
    } // end if
    
    if (mui_seriefactura->id() != "") {
	filtro += " AND codigoserie_factura = '" + mui_seriefactura->id() + "'";
    } // end if

    if (mui_almacen->id() != "") {
	filtro += " AND codigoalmacen = '" + mui_almacen->id() + "'";
    } // end if
    
    
    
    return ( filtro );
}


/** Metodo que responde a la accion por defecto sobre el listado (doble click o boton de editar)
    Si estamos en modo edicion abre la ficha de Factura y carga el elemento seleccionado.
    Si estamos en modo seleccion cierra la ventana y emite el SIGNAL de que se ha seleccionado una factura.
*/
/**
\param row
\return
**/
void FacturasList::editar ( int row )
{
    BL_FUNC_DEBUG
    mdb_idfactura = mui_list->dbValue ( QString ( "idfactura" ), row );
    if ( editMode() ) {
        FacturaView * prov = new FacturaView ( ( BfCompany * ) mainCompany(), 0 );
        if ( prov->load ( mdb_idfactura ) ) {
            delete prov;
            return;
        } // end if
        mainCompany() ->m_pWorkspace->addSubWindow ( prov );
        prov->show();
    } else {
        emit ( selected ( mdb_idfactura ) );
    } // end if
    
}



/** SLOT que responde a la pulsacion del boton de imprimir.
    La impresion de listados esta completamente delegada en BfSubForm
*/
/**
**/
void FacturasList::imprimir()
{
    BL_FUNC_DEBUG
    mui_list->printPDF ( _ ( "Facturas a clientes" ) );
    
}


/** SLOT que responde a la pulsacion del boton borrar.
    Crea una instancia de FacuturaView, carga el elemento seleccionado e invoca al
    metodo de borrar.
    La ventaja de hacerlo de esta forma es que si hay plugins en la factura, estos tambien se ejecutaran.
*/
///
/**
\return
**/
void FacturasList::remove()
{
    BL_FUNC_DEBUG
    int a = mui_list->currentRow();
    if ( a < 0 ) {
        blMsgInfo ( _ ( "Debe seleccionar una linea" ), this );
        return;
    } // end if
    try {
        mdb_idfactura = mui_list->dbValue ( QString ( "idfactura" ) );
        if ( editMode() ) {
            FacturaView * fv = new FacturaView ( ( BfCompany * ) mainCompany() );
            if ( fv->load ( mdb_idfactura ) )
                throw - 1;
            fv->on_mui_borrar_clicked();
            fv->close();
        } // end if
        presentar();
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al borrar la factura a cliente" ), this );
    } // end try
    
}


///
/**
\param comp
**/
void FacturasList::setMainCompany ( BfCompany *comp )
{
    BL_FUNC_DEBUG
    BlMainCompanyPointer::setMainCompany ( comp );
    m_cliente->setMainCompany ( comp );
    m_articulo->setMainCompany ( comp );
    mui_list->setMainCompany ( comp );
    
    /// Iniciamos el buscador de series.
    mui_seriefactura->setMainCompany ( mainCompany() );
    mui_seriefactura->setQuery ( "SELECT * FROM serie_factura ORDER BY descserie_factura" );
    mui_seriefactura->setTableName ( "serie_factura" );
    mui_seriefactura->setFieldId ( "codigoserie_factura" );
    mui_seriefactura->m_valores["codigoserie_factura"] = "";
    mui_seriefactura->m_valores["descserie_factura"] = "";
    mui_seriefactura->setAllowNull ( true );
    mui_seriefactura->setId("");

    /// Iniciamos el buscador de almacenes.
    mui_almacen->setMainCompany ( mainCompany() );
    mui_almacen->setQuery ( "SELECT * FROM almacen ORDER BY idalmacen" );
    mui_almacen->setTableName ( "almacen" );
    mui_almacen->setFieldId ( "codigoalmacen" );
    mui_almacen->m_valores["codigoalmacen"] = "";
    mui_almacen->m_valores["nomalmacen"] = "";
    mui_almacen->setAllowNull ( true );
    mui_almacen->setId("");

    
    
}


///
/**
\return
**/
QString FacturasList::idfactura()
{
    BL_FUNC_DEBUG
    
    return mdb_idfactura;
}


///
/**
\param val
**/
void FacturasList::setidcliente ( QString val )
{
    BL_FUNC_DEBUG
    m_cliente->setId ( val );
    
}


///
/**
\param val
**/
void FacturasList::setidarticulo ( QString val )
{
    BL_FUNC_DEBUG
    m_articulo->setidarticulo ( val );
    
}


///
/**
**/
void FacturasList::crear()
{
    BL_FUNC_DEBUG
    FacturaView *fv = new FacturaView ( ( BfCompany * ) mainCompany(), 0 );
    mainCompany()->m_pWorkspace->addSubWindow ( fv );
    fv->inicializar();
    fv->pintar();
    
    /// Si se crea el nueva factura desde la lista de facturas de un cliente,
    /// entonces se crea la factura para este cliente.
    if (m_cliente->id() != "") {
	fv->mui_idcliente->setId( m_cliente->id() );
    } // end if
    
    fv->show();
    fv->mui_descfactura->setFocus ( Qt::OtherFocusReason );
    
}

/// =============================================================================
///                    SUBFORMULARIO
/// =============================================================================
/** Prepara el subformulario para trabajar con la tabla factura.
*/
/**
\param parent
\return
**/
FacturasListSubform::FacturasListSubform ( QWidget *parent, const char * ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins.
    int res = g_plugins->run ( "FacturasListSubform_FacturasListSubform", this );
    if ( res != 0 )
        return;
    setDbTableName ( "factura" );
    setDbFieldId ( "idfactura" );
    addSubFormHeader ( "numfactura", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero" ) );
    addSubFormHeader ( "ffactura", BlDbField::DbDate, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Fecha" ) );
    addSubFormHeader ( "nomcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Nombre cliente" ) );
    addSubFormHeader ( "telfactura", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Telefono factura" ) );
    addSubFormHeader ( "base", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Base imponible" ) );
    addSubFormHeader ( "impuestos", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Impuestos" ) );
    addSubFormHeader ( "total", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Total" ) );
    addSubFormHeader ( "procesadafactura", BlDbField::DbBoolean, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Procesada" ) );
    addSubFormHeader ( "descfactura", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Descripcion" ) );
    addSubFormHeader ( "idfactura", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Id factura" ) );
    addSubFormHeader ( "reffactura", BlDbField::DbVarChar, BlDbField::DbNotNull | BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Ref factura" ) );
    addSubFormHeader ( "codigoserie_factura", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Serie" ) );
    addSubFormHeader ( "cifcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "CIF cliente" ) );
    addSubFormHeader ( "codigoalmacen", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Codigo almacen" ) );
    addSubFormHeader ( "contactfactura", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Contact factura" ) );
    addSubFormHeader ( "comentfactura", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Comentario factura" ) );
    addSubFormHeader ( "idtrabajador", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Id trabajador" ) );
    addSubFormHeader ( "idcliente", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Id cliente" ) );
    addSubFormHeader ( "idalmacen", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Id almacen" ) );
    addSubFormHeader ( "idforma_pago", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Id Forma de Pago" ) );
    addSubFormHeader ( "descforma_pago", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Forma de Pago" ) );
    setInsert ( false );
    setDelete ( false );
    setSortingEnabled ( true );
    
}


///
/**
**/
FacturasListSubform::~FacturasListSubform()
{
    BL_FUNC_DEBUG
    
}


///
/**
**/
void FacturasListSubform::load()
{
    BL_FUNC_DEBUG
    QString SQLQuery = "SELECT * FROM factura LEFT JOIN forma_pago ON factura.idforma_pago = forma_pago.idforma_pago";
    BlSubForm::load ( SQLQuery );
}


///
/**
\param query
**/
void FacturasListSubform::load ( QString query )
{
    BL_FUNC_DEBUG
    BlSubForm::load ( query );
    
}

