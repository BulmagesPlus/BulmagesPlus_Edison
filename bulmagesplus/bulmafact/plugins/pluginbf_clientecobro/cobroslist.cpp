/***************************************************************************
 *   Copyright (C) 2004 by Tomeu Borras Riera                              *
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

#include <QMessageBox>
#include <QCheckBox>
#include <QFile>
#include <QCheckBox>
#include <QTextStream>
#include <QMenu>

#include "blconfiguration.h"
#include "cobroslist.h"
#include "bfcompany.h"
#include "cobroview.h"
#include "blfunctions.h"


/** Inicializa todos los componentes.
    Mete la ventana en el workSpace.
    Este constructor no es completo, debe inicializarse con setcompany para que la clase pueda operar.
*/
CobrosList::CobrosList ( QWidget *parent, Qt::WindowFlags flag, edmode editmodo )
        : BlFormList ( NULL, parent, flag, editmodo )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "CobrosList_CobrosList", this );
    if ( res != 0 )
        return;
    mdb_idcobro = "";
    setSubForm ( mui_list );
    /// Establecemos los parametros de busqueda del Cliente
    m_cliente->setLabel ( _ ( "Cliente:" ) );
    m_cliente->setTableName ( "cliente" );
    m_cliente->m_valores["cifcliente"] = "";
    m_cliente->m_valores["nomcliente"] = "";    hideBusqueda();

    mui_tipocobro->insertItem ( 0, _ ( "Todos los cobros" ) );
    mui_tipocobro->insertItem ( 1, _ ( "Solo previsiones" ) );
    mui_tipocobro->insertItem ( 2, _ ( "Cobro real" ) );
    mui_tipocobro->setCurrentIndex ( 1 );
    /// Llamamos a los scripts
    blScript(this);
    
}


/** Inicializa todos los componentes.
    Hace una presentacion inicial.
    Mete la ventana en el workSpace.
*/
CobrosList::CobrosList ( BfCompany *comp, QWidget *parent, Qt::WindowFlags flag, edmode editmodo )
        : BlFormList ( comp, parent, flag, editmodo )
{
    BL_FUNC_DEBUG
    setupUi ( this );
    /// Disparamos los plugins.
    int res = g_plugins->run ( "CobrosList_CobrosList", this );
    if ( res != 0 )
        return;
    m_cliente->setMainCompany ( comp );
    mui_list->setMainCompany ( comp );
    mui_formapago->setMainCompany ( comp );
    mui_formapago->setId ( "" );
    setSubForm ( mui_list );
    /// Establecemos los parametros de busqueda del Cliente
    m_cliente->setLabel ( _ ( "Cliente:" ) );
    m_cliente->setTableName ( "cliente" );
    m_cliente->m_valores["cifcliente"] = "";
    m_cliente->m_valores["nomcliente"] = "";
    
    /// Cargamos los filtros guardados.
    cargaFiltrosXML();
    
    presentar();
    mdb_idcobro = "";
    if ( editMode() )
        mainCompany() ->insertWindow ( windowTitle(), this );
    hideBusqueda();
    /// Hacemos el tratamiento de los permisos que desabilita botones en caso de no haber suficientes permisos.
    trataPermisos ( "cobro" );
    /// Llamamos a los scripts
    blScript(this);
    
}


/** No requiere acciones especiales en el destructor.
*/
CobrosList::~CobrosList()
{
    BL_FUNC_DEBUG
    
}


/** Hace la carga del listado.
    Para ello genera los SELECTS con ayuda de generaFiltro y los pasa al SubFormulario para que los presente.
    Tambien hace un select de calculo de totales y lo presenta en el textEdit correspondiente.
*/
void CobrosList::presentar()
{
    BL_FUNC_DEBUG
    if ( mainCompany() != NULL ) {
	QString query = "SELECT * FROM cobro LEFT JOIN cliente ON cliente.idcliente = cobro.idcliente LEFT JOIN forma_pago ON cobro.idforma_pago = forma_pago.idforma_pago LEFT JOIN trabajador ON trabajador.idtrabajador = cobro.idtrabajador WHERE 1 = 1 " + generaFiltro();
        mui_list->load ( query );
        /// Hacemos el calculo del total.
        BlFixed total = mui_list->sumarCampo ( "cantcobro" );
        m_total->setText ( total.toQString('0', 2) );
    } // end if
    
}


/** Metodo auxiliar que genera la clausula WHERE del listado con las opciones de filtrado especificadas.
*/
const QString CobrosList::generaFiltro()
{
    BL_FUNC_DEBUG
    QString filtro = "";
    
    /// Hacemos el filtrado like del campo m_filtro
    filtro += mui_list->likeFilterSQL(m_filtro->text());
    

    if ( m_cliente->id() != "" ) {
        filtro += " AND cobro.idcliente = " + m_cliente->id();
    } // end if

    if ( mui_tipocobro->currentIndex() == 1 ) {
        /// Muestra solo las de prevision.
        filtro += " AND previsioncobro ";
    } else if ( mui_tipocobro->currentIndex() == 2 ) {
        /// Muestra solo las de NO prevision.
        filtro += " AND NOT previsioncobro ";
    } // end if

    if ( m_fechain->text() != "" )
        filtro += " AND fechacobro >= '" + m_fechain->text() + "' ";

    if ( m_fechafin->text() != "" )
        filtro += " AND fechacobro <= '" + m_fechafin->text() + "' ";

    if ( mui_formapago->id() != "" )
        filtro += " AND cobro.idforma_pago = " + mui_formapago->id() ;

    
    return ( filtro );
}



/** SLOT que responde a la pulsacion del boton de crear en el listado.
    Instancia la clase CobroView, y la presenta.
*/
void CobrosList::crear()
{
    BL_FUNC_DEBUG
    CobroView *cv = new CobroView ( ( BfCompany * ) mainCompany(), 0 );
    mainCompany() ->m_pWorkspace->addSubWindow ( cv );
    cv->pintar();
    
    /// Si se crea el nuevo cobro desde la lista de cobros de un cliente,
    /// entonces se crea el cobro para este cliente.
    if (m_cliente->id() != "") {
	cv->mui_idcliente->setId( m_cliente->id() );
    } // end if

    cv->show();
}


/** La impresion de listados esta completamente delegada a la clase BlSubForm
*/
void CobrosList::imprimir()
{
    BL_FUNC_DEBUG
    mui_list->printPDF ( _ ( "Cobros a clientes" ) );
    
}


/** SLOT que responde a la pulsacion del boton borrar.
    Comprueba que exista un elemento seleccionado.
    Instancia la clase CobroView, la inicializa y le lanza el metodo borrar.
    Esta es la forma correcta de implementar un borrado a partir de un listado
    ya que de esta forma si existen plugins que alteren el borrado tambien seran invocados.
*/
void CobrosList::remove()
{
    BL_FUNC_DEBUG
    int a = mui_list->currentRow();
    if ( a < 0 ) {
        blMsgInfo ( _ ( "Debe seleccionar una linea" ) );
        return;
    } // end if
    try {
        mdb_idcobro = mui_list->dbValue ( "idcobro" );
        if ( editMode() ) {
            CobroView * cv = new CobroView ( ( BfCompany * ) mainCompany(), 0 );
            if ( cv->load ( mdb_idcobro ) )
                throw - 1;
            cv->on_mui_borrar_clicked();
            cv->close();
        } // end if
        presentar();
    } catch ( ... ) {
        blMsgInfo ( _ ( "Error al borrar el cobro a cliente" ) );
    } // end try
    
}


/** SLOT que responde al doble click en el subformulario.
    Dependiendo del modo (Edicion o Seleccion) hace unas operaciones u otras.
*/
/// \TODO: Deberia crearse el metodo editar y este llamar a ese.
void CobrosList::editar ( int )
{
    BL_FUNC_DEBUG
    try {
        mdb_idcobro = mui_list->dbValue ( "idcobro" );
        if ( editMode() ) {
            CobroView * bud = new CobroView ( ( BfCompany * ) mainCompany(), 0 );
            if ( bud->load ( mdb_idcobro ) ) {
                delete bud;
                return;
            } // end if
            mainCompany() ->m_pWorkspace->addSubWindow ( bud );
            bud->show();
        } else {
            emit ( selected ( mdb_idcobro ) );
        } // end if
    } catch ( ... ) {
        blMsgInfo ( _ ( "Debe seleccionar una fila primero" ) );
    } // end try
    

}

/** SLOT que responde a la peticion de menu contextual en el subformulario.
*/
/// \TODO: Revisar si este metodo es util.
void CobrosList::submenu ( const QPoint & )
{
    BL_FUNC_DEBUG
    int a = mui_list->currentRow();
    if ( a < 0 )
        return;
    QMenu *popup = new QMenu ( this );
    QAction *edit = popup->addAction ( _ ( "Editar cobro" ) );
    QAction *del = popup->addAction ( _ ( "Borrar cobro" ) );
    QAction *opcion = popup->exec ( QCursor::pos() );
    if ( opcion == del )
        on_mui_borrar_clicked();
    if ( opcion == edit )
        on_mui_editar_clicked();
    delete popup;
    
}

/** Inicializa la clase con el puntero a la company que se esta utilizando
**/
void CobrosList::setMainCompany ( BfCompany *comp )
{
    BlMainCompanyPointer::setMainCompany ( comp );
    m_cliente->setMainCompany ( comp );
    mui_list->setMainCompany ( comp );
    mui_formapago->setMainCompany ( comp );
    mui_formapago->setId ( "" );
}

/** Devuelve el identificador del cobro seleccionado
**/
QString CobrosList::idcobro()
{
    BL_FUNC_DEBUG
    
    return mdb_idcobro;
}

/** Inicializa la la case con un cliente determiando. Lo establece en las opciones de filtrado
    no actualiza el listado
**/
void CobrosList::setidcliente ( QString val )
{
    BL_FUNC_DEBUG
    m_cliente->setId ( val );
    
}

/// =============================================================================
///                    SUBFORMULARIO
/// =============================================================================
/** Prepara el subformulario para trabajar con la tabla cobro.
    Establece que no se puedan insertar elementos ya que es un listado.
*/
CobrosListSubForm::CobrosListSubForm ( QWidget *parent ) : BfSubForm ( parent )
{
    BL_FUNC_DEBUG
    /// Disparamos los plugins.
    int res = g_plugins->run ( "CobrosListSubForm_CobrosListSubForm", this );
    if ( res != 0 )
        return;
    setDbTableName ( "cobro" );
    setDbFieldId ( "idcobro" );
    addSubFormHeader ( "idcobro", BlDbField::DbInt, BlDbField::DbNotNull | BlDbField::DbPrimaryKey, BlSubFormHeader::DbHideView | BlSubFormHeader::DbNoWrite, _ ( "ID cobro" ) );
    addSubFormHeader ( "idcliente", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID cliente" ) );
    addSubFormHeader ( "nomcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Nombre" ) );
    addSubFormHeader ( "cifcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "C.I.F." ) );
    addSubFormHeader ( "telcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Numero de telefono" ) );
    addSubFormHeader ( "mailcliente", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Direccion de correo electronico" ) );
    addSubFormHeader ( "fechacobro", BlDbField::DbDate, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Fecha de cobro" ) );
    addSubFormHeader ( "fechavenccobro", BlDbField::DbDate, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Fecha de vencimiento" ) );
    addSubFormHeader ( "cantcobro", BlDbField::DbNumeric, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Cantidad" ) );
    addSubFormHeader ( "descforma_pago", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Forma de Pago" ) );
    addSubFormHeader ( "refcobro", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Referencia del cobro" ) );
    addSubFormHeader ( "previsioncobro", BlDbField::DbBoolean, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Prevision de cobro" ) );
    addSubFormHeader ( "comentcobro", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Comentarios" ) );
    addSubFormHeader ( "idtrabajador", BlDbField::DbInt, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "ID trabajador" ) );
    addSubFormHeader ( "nomtrabajador", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Nombre del trabajador" ) );
    addSubFormHeader ( "apellidostrabajador", BlDbField::DbVarChar, BlDbField::DbNoSave, BlSubFormHeader::DbNone | BlSubFormHeader::DbNoWrite, _ ( "Apellidos del trabajador" ) );
    setInsert ( false );
    setDelete ( false );
    setSortingEnabled ( true );
    
}


CobrosListSubForm::~CobrosListSubForm()
{
    BL_FUNC_DEBUG
    
}

