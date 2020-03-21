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

#include <QCheckBox>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "albaranclientelist.h"
#include "albaranclienteview.h"
#include "blplugins.h"
#include "bfcompany.h"
#include "blfunctions.h"


///
/**
**/
void AlbaranClienteListSubform::load()
{
    BL_FUNC_DEBUG
    QString SQLQuery = "SELECT * FROM albaran";
    BlSubForm::load ( SQLQuery );
    
}


///
/**
\param query
**/
void AlbaranClienteListSubform::load ( QString query )
{
    BL_FUNC_DEBUG
    BlSubForm::load ( query );
    
}


///
/**
\param val
**/
void AlbaranClienteList::setidcliente ( QString val )
{
    BL_FUNC_DEBUG
    m_cliente->setId ( val );
    
}


///
/**
\return
**/
QString AlbaranClienteList::idCliDelivNote()
{
    BL_FUNC_DEBUG
    
    return mdb_idalbaran;
}


///
/**
**/
void AlbaranClienteList::crear()
{
    BL_FUNC_DEBUG
    AlbaranClienteView *acv = new AlbaranClienteView ( ( BfCompany * ) mainCompany(), 0 );
    mainCompany()->m_pWorkspace->addSubWindow ( acv );
    acv->inicializar();
    acv->pintar();
    
    /// Si se crea el nuevo albaran desde la lista de albaranes de un cliente,
    /// entonces se crea el albaran para este cliente.
    if (m_cliente->id() != "") {
	acv->mui_idcliente->setId( m_cliente->id() );
    } // end if

    acv->show();
    acv->mui_descalbaran->setFocus ( Qt::OtherFocusReason );
    
}


/** Constructor de la clase sin inicializacion de company. Usando este
    constructor no se olvide de usar setcompany para que la clase no de
    excepciones.
    El constructor crea la pantalla, la pone en editmodo por defecto y la mete
    en el listado de ventanas.
    Usando esta clase tampoco se inicializan bien los widgets que contiene.
*/
/**
\param parent
\param flag
\param editmodo
\return
**/
AlbaranClienteList::AlbaranClienteList ( QWidget *parent, Qt::WindowFlags flag, edmode editmodo )
        : BlFormList ( NULL, parent, flag, editmodo )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "AlbaranClienteList_AlbaranClienteList", this );
    if ( res != 0 )
        return;
    mdb_idalbaran = "";
    setSubForm ( mui_list );
    mui_filtrar->setChecked(false);
    //hideBusqueda();
    iniciaForm();
    /// Llamamos a los scripts
    blScript(this);
    
}


/** Constructor de la clase.
    Inicializa toda la ventana y los widgets que esta contiene.
    Hace una presentacion inicial de los albaranes de cliente.
    Pone la pantalla en modo de edicion por defecto  a no ser que se especifique lo contrario.
    Mete la ventana en el workspace.
*/
/**
\param comp
\param parent
\param flag
\param editmodo
\return
**/
AlbaranClienteList::AlbaranClienteList ( BfCompany *comp, QWidget *parent, Qt::WindowFlags flag, edmode editmodo )
        : BlFormList ( comp, parent, flag, editmodo )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "AlbaranClienteList_AlbaranClienteList", this );
    if ( res != 0 )
        return;
    m_cliente->setMainCompany ( comp );
    m_articulo->setMainCompany ( comp );
    mui_list->setMainCompany ( comp );
    setSubForm ( mui_list );
    iniciaForm();

    /// Establecemos los parametros de busqueda del Cliente
    m_cliente->setLabel ( _ ( "Cliente:" ) );
    m_cliente->setTableName ( "cliente" );
    m_cliente->m_valores["cifcliente"] = "";
    m_cliente->m_valores["nomcliente"] = "";

    /// Iniciamos el buscador de trabajadores.
    m_idtrabajador->setMainCompany ( mainCompany() );
    m_idtrabajador->setQuery ( "SELECT * FROM trabajador ORDER BY apellidostrabajador, nomtrabajador" );
    m_idtrabajador->setTableName ( "trabajador" );
    m_idtrabajador->setFieldId ( "idtrabajador" );
    m_idtrabajador->m_valores["apellidostrabajador"] = "";
    m_idtrabajador->m_valores["nomtrabajador"] = "";
    m_idtrabajador->setAllowNull ( true );
    m_idtrabajador->setId("");
    
    /// Cargamos los filtros guardados.
    cargaFiltrosXML();
    
    presentar();
    mdb_idalbaran = "";
    if ( editMode() )
        mainCompany() ->insertWindow ( windowTitle(), this );

    mui_filtrar->setChecked(false);
    //hideBusqueda();
    /// Hacemos el tratamiento de los permisos que desabilita botones en caso de no haber suficientes permisos.
    trataPermisos ( "albaran" );
    /// Llamamos a los scripts
    blScript(this);
    
}


///
/**
\return
**/
void AlbaranClienteList::iniciaForm()
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins.
    int res = g_plugins->run ( "AlbaranClienteList_iniciaForm", this );
    if ( res != 0 )
        return;
    mui_procesada->insertItem ( 0, _ ( "Todos los albaranes" ) );
    mui_procesada->insertItem ( 1, _ ( "Albaranes procesados" ) );
    mui_procesada->insertItem ( 2, _ ( "Albaranes no procesados" ) );
    mui_procesada->setCurrentIndex ( 2 );
    
}


///
/**
\param comp
**/
void AlbaranClienteList::setMainCompany ( BfCompany *comp )
{
    BL_FUNC_DEBUG
    BlMainCompanyPointer::setMainCompany ( comp );
    m_cliente->setMainCompany ( comp );
    m_articulo->setMainCompany ( comp );
    mui_list->setMainCompany ( comp );
    
    
    /// Iniciamos el buscador de trabajadores.
    m_idtrabajador->setMainCompany ( mainCompany() );
    m_idtrabajador->setQuery ( "SELECT * FROM trabajador ORDER BY apellidostrabajador, nomtrabajador" );
    m_idtrabajador->setTableName ( "trabajador" );
    m_idtrabajador->setFieldId ( "idtrabajador" );
    m_idtrabajador->m_valores["apellidostrabajador"] = "";
    m_idtrabajador->m_valores["nomtrabajador"] = "";
    m_idtrabajador->setAllowNull ( true );   
    m_idtrabajador->setId("");
    
    
}


/** Destructor de la clase */
/**
**/
AlbaranClienteList::~AlbaranClienteList()
{
    BL_FUNC_DEBUG
    
}


/** Carga el listado de la base de datos y lo presenta.
    Tambien carga el total y lo presenta.
*/
/**
**/
void AlbaranClienteList::presentar()
{
    BL_FUNC_DEBUG

    mui_list->load ( "SELECT *, totalalbaran AS total, bimpalbaran AS base, impalbaran AS impuestos FROM albaran LEFT JOIN  cliente ON albaran.idcliente = cliente.idcliente LEFT JOIN almacen ON albaran.idalmacen = almacen.idalmacen LEFT JOIN forma_pago ON albaran.idforma_pago = forma_pago.idforma_pago WHERE 1 = 1 " + generarFiltro() );

    /// Hacemos el calculo del total.
    BlDbRecordSet *cur = mainCompany() ->loadQuery ( "SELECT SUM(totalalbaran) AS total FROM albaran LEFT JOIN cliente ON albaran.idcliente=cliente.idcliente LEFT JOIN almacen ON almacen.idalmacen = albaran.idalmacen where 1 = 1 " + generarFiltro() );
    /// Esta consulta podria resultar NULA y debe tratarse el caso
    if ( cur ) {
        m_total->setText ( cur->value( "total" ) );
        delete cur;
    } // end if

    
}


/**  Este metodo se activa cuando bien pulsando sobre el boton de editar
     o bien haciendo doble click en el modo de edicion se desea invocar la accion
     Editar el elemento si estamos en modo editmode o cerrar la ventana y emitir
     un signal selected() si estamos en el modo selector.

     Primero determina el idalbaran seleccionado, luego crea la instancia de
     la ventana de edicion AlbaranClienteView y lo mete en el workspace.
     Por ultimo hace que dicha ventana carge de la base de datos el idalbaran
     seleccionado.
*/
/**
\param row
\return
**/
void AlbaranClienteList::editar ( int row )
{
    BL_FUNC_DEBUG
    mdb_idalbaran = mui_list->dbValue ( QString ( "idalbaran" ), row );
    if ( editMode() ) {
        AlbaranClienteView * prov = new AlbaranClienteView ( ( BfCompany * ) mainCompany(), 0 );
        if ( prov->load ( mdb_idalbaran ) ) {
            delete prov;
            return;
        } // end if
        mainCompany() ->m_pWorkspace->addSubWindow ( prov );
        prov->show();
    } else {
        emit ( selected ( mdb_idalbaran ) );
    } // end if
    
}



/** Responde a la pulsacion del boton borrar en el formulario.
    Carga el elemento seleccionado e invoca a su metodo de borrar.
*/
/// \todo: Comprobar que se libera bien la memoria.
/// \todo: Intentar que no se tenga que recargar todo el listado y que simplemente se borre la fila seleccionada.
/**
\return
**/
void AlbaranClienteList::remove()
{
    BL_FUNC_DEBUG
    int a = mui_list->currentRow();
    if ( a < 0 ) {
        blMsgInfo ( _ ( "Debe seleccionar una linea" ), this );
        return;
    } // end if
    try {
        mdb_idalbaran = mui_list->dbValue ( QString ( "idalbaran" ) );
        if ( editMode() ) {
            AlbaranClienteView * acv = new AlbaranClienteView ( ( BfCompany * ) mainCompany(), 0 );
            if ( acv->load ( mdb_idalbaran ) )
                throw - 1;
            acv->on_mui_borrar_clicked();
            acv->close();
        } // end if
        presentar();
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al borrar el albaran a cliente" ), this );
    } // end try
    
}


/** La impresion de listados es estandarizada por la clase BlDbRecord
*/
void AlbaranClienteList::imprimir()
{
    BL_FUNC_DEBUG
    mui_list->printPDF ( _ ( "Albaranes a clientes" ) );
    
}


/** Este metodo ayuda a construir el query de filtrado construyendo la clausula
    WHERE a partir de las opciones de filtrado.
*/
/**
\return
**/
QString AlbaranClienteList::generarFiltro()
{
    /// Tratamiento de los filtros.
    BL_FUNC_DEBUG
    QString filtro = "";

    /// Hacemos el filtrado like del campo m_filtro
    filtro += mui_list->likeFilterSQL(m_filtro->text());
    

    if ( m_cliente->id() != "" )
        filtro += " AND albaran.idcliente='" + m_cliente->id() + "'";

    if ( m_articulo->idarticulo() != "" )
        filtro += " AND idalbaran IN (SELECT DISTINCT idalbaran FROM lalbaran WHERE idarticulo='" + m_articulo->idarticulo() + "')";

    /// Tratamos los elementos procesados y no procesados.
    if ( mui_procesada->currentIndex() == 1 ) {
        /// Muestra solo las procesadas.
        filtro += " AND procesadoalbaran";
    } else if ( mui_procesada->currentIndex() == 2 ) {
        /// Muestra solo las NO procesadas.
        filtro += " AND NOT procesadoalbaran ";
    } // end if

    if ( m_fechain->text() != "" )
        filtro += " AND fechaalbaran >= '" + m_fechain->text() + "' ";

    if ( m_fechafin->text() != "" )
        filtro += " AND fechaalbaran <= '" + m_fechafin->text() + "' ";

    if (m_idtrabajador->id() != "") {
	filtro += " AND idtrabajador = " + m_idtrabajador->id();
    } // end if
    
    
    return ( filtro );
}


/// =============================================================================
///                    SUBFORMULARIO
/// =============================================================================
/** Constructor del subformulario de albaranes a cliente.
    Configura el subformulario para que use la tabla albaran.
*/
/**
\param parent
\return
**/
AlbaranClienteListSubform::AlbaranClienteListSubform ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG

    /// Disparamos los plugins.
    int res = g_plugins->run ( "AlbaranClienteListSubform_AlbaranClienteListSubform", this );
    if ( res != 0 )
        return;
    setDbTableName ( "albaran" );
    setDbFieldId ( "idalbaran" );
    addSubFormHeader ( "refalbaran", BlDbField::DbVarChar, BlDbField::DbNotNull | BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "Referencia de albaran" ) );
    addSubFormHeader ( "codigoalmacen", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Codigo de almacen" ) );
    addSubFormHeader ( "numalbaran", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero de albaran" ) );
    addSubFormHeader ( "descalbaran", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Descripcion" ) );
    addSubFormHeader ( "fechaalbaran", BlDbField::DbDate, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Fecha de albaran" ) );
    addSubFormHeader ( "nomcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Nombre de cliente" ) );
    addSubFormHeader ( "idforma_pago", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID forma de pago" ) );
    addSubFormHeader ( "descforma_pago", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Descripcion de forma de pago" ) );
    addSubFormHeader ( "idtrabajador", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID trabajador" ) );
    addSubFormHeader ( "idcliente", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID cliente" ) );
    addSubFormHeader ( "idalbaran", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID albaran" ) );
    addSubFormHeader ( "comentalbaran", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Comentarios del albaran" ) );
    addSubFormHeader ( "idalmacen", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID almacen" ) );
    addSubFormHeader ( "base", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Base imponible" ) );
    addSubFormHeader ( "impuestos", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Impuestos" ) );
    addSubFormHeader ( "total", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Total" ) );
    setInsert ( false );
    setDelete ( false );
    setSortingEnabled ( true );
    
}


///
/**
**/
AlbaranClienteListSubform::~AlbaranClienteListSubform()
{
    BL_FUNC_DEBUG
    
}
